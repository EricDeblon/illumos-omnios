/*
 * Copyright 2013 Garrett D'Amore <garrett@damore.org>
 * Copyright 2011 Nexenta Systems, Inc.  All rights reserved.
 * Copyright (c) 2002-2004 Tim J. Robbins
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "lint.h"
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "mblocal.h"
#include "lctype.h"

static size_t	_UTF8_mbrtowc(wchar_t *_RESTRICT_KYWD,
		    const char *_RESTRICT_KYWD,
		    size_t, mbstate_t *_RESTRICT_KYWD, boolean_t);
static int	_UTF8_mbsinit(const mbstate_t *);
static size_t	_UTF8_mbsnrtowcs(wchar_t *_RESTRICT_KYWD,
		    const char **_RESTRICT_KYWD, size_t, size_t,
		    mbstate_t *_RESTRICT_KYWD);
static size_t	_UTF8_wcrtomb(char *_RESTRICT_KYWD, wchar_t,
		    mbstate_t *_RESTRICT_KYWD);
static size_t	_UTF8_wcsnrtombs(char *_RESTRICT_KYWD,
		    const wchar_t **_RESTRICT_KYWD,
		    size_t, size_t, mbstate_t *_RESTRICT_KYWD);

typedef struct {
	wchar_t	ch;
	int	want;
	wchar_t	lbound;
} _UTF8State;

void
_UTF8_init(struct lc_ctype *lct)
{
	lct->lc_mbrtowc = _UTF8_mbrtowc;
	lct->lc_wcrtomb = _UTF8_wcrtomb;
	lct->lc_mbsinit = _UTF8_mbsinit;
	lct->lc_mbsnrtowcs = _UTF8_mbsnrtowcs;
	lct->lc_wcsnrtombs = _UTF8_wcsnrtombs;
	lct->lc_is_ascii = 0;
	lct->lc_max_mblen = 4;
}

static int
_UTF8_mbsinit(const mbstate_t *ps)
{

	return (ps == NULL || ((const _UTF8State *)ps)->want == 0);
}

static size_t
_UTF8_mbrtowc(wchar_t *_RESTRICT_KYWD pwc, const char *_RESTRICT_KYWD s,
    size_t n, mbstate_t *_RESTRICT_KYWD ps, boolean_t zero)
{
	_UTF8State *us;
	int ch, i, mask, want;
	wchar_t lbound, wch;

	us = (_UTF8State *)ps;

	if (us->want < 0 || us->want > 6) {
		errno = EINVAL;
		return ((size_t)-1);
	}

	if (s == NULL) {
		s = "";
		n = 1;
		pwc = NULL;
	}

	if (n == 0)
		/* Incomplete multibyte sequence */
		return ((size_t)-2);

	if (us->want == 0) {
		/*
		 * Determine the number of octets that make up this character
		 * from the first octet, and a mask that extracts the
		 * interesting bits of the first octet. We already know
		 * the character is at least two bytes long.
		 *
		 * We also specify a lower bound for the character code to
		 * detect redundant, non-"shortest form" encodings. For
		 * example, the sequence C0 80 is _not_ a legal representation
		 * of the null character. This enforces a 1-to-1 mapping
		 * between character codes and their multibyte representations.
		 */
		ch = (unsigned char)*s;
		if ((ch & 0x80) == 0) {
			/* Fast path for plain ASCII characters. */
			if (pwc != NULL)
				*pwc = ch;
			if (zero || ch != '\0') {
				return (1);
			} else {
				return (0);
			}
		}
		if ((ch & 0xe0) == 0xc0) {
			mask = 0x1f;
			want = 2;
			lbound = 0x80;
		} else if ((ch & 0xf0) == 0xe0) {
			mask = 0x0f;
			want = 3;
			lbound = 0x800;
		} else if ((ch & 0xf8) == 0xf0) {
			mask = 0x07;
			want = 4;
			lbound = 0x10000;
#if 0
		/* These would be illegal in the UTF-8 space */

		} else if ((ch & 0xfc) == 0xf8) {
			mask = 0x03;
			want = 5;
			lbound = 0x200000;
		} else if ((ch & 0xfe) == 0xfc) {
			mask = 0x01;
			want = 6;
			lbound = 0x4000000;
#endif
		} else {
			/*
			 * Malformed input; input is not UTF-8.
			 */
			errno = EILSEQ;
			return ((size_t)-1);
		}
	} else {
		want = us->want;
		lbound = us->lbound;
	}

	/*
	 * Decode the octet sequence representing the character in chunks
	 * of 6 bits, most significant first.
	 */
	if (us->want == 0)
		wch = (unsigned char)*s++ & mask;
	else
		wch = us->ch;

	for (i = (us->want == 0) ? 1 : 0; i < MIN(want, n); i++) {
		if ((*s & 0xc0) != 0x80) {
			/*
			 * Malformed input; bad characters in the middle
			 * of a character.
			 */
			errno = EILSEQ;
			return ((size_t)-1);
		}
		wch <<= 6;
		wch |= *s++ & 0x3f;
	}
	if (i < want) {
		/* Incomplete multibyte sequence. */
		us->want = want - i;
		us->lbound = lbound;
		us->ch = wch;
		return ((size_t)-2);
	}
	if (wch < lbound) {
		/*
		 * Malformed input; redundant encoding.
		 */
		errno = EILSEQ;
		return ((size_t)-1);
	}
	if (pwc != NULL)
		*pwc = wch;
	us->want = 0;
	if (zero || wch != L'\0') {
		return (want);
	} else {
		return (0);
	}
}

static size_t
_UTF8_mbsnrtowcs(wchar_t *_RESTRICT_KYWD dst, const char **_RESTRICT_KYWD src,
    size_t nms, size_t len, mbstate_t *_RESTRICT_KYWD ps)
{
	_UTF8State *us;
	const char *s;
	size_t nchr;
	wchar_t wc;
	size_t nb;

	us = (_UTF8State *)ps;

	s = *src;
	nchr = 0;

	if (dst == NULL) {
		/*
		 * The fast path in the loop below is not safe if an ASCII
		 * character appears as anything but the first byte of a
		 * multibyte sequence. Check now to avoid doing it in the loop.
		 */
		if (nms > 0 && us->want > 0 && (signed char)*s > 0) {
			errno = EILSEQ;
			return ((size_t)-1);
		}
		for (;;) {
			if (nms > 0 && (signed char)*s > 0) {
				/*
				 * Fast path for plain ASCII characters
				 * excluding NUL.
				 */
				nb = 1;
			} else if ((nb = _UTF8_mbrtowc(&wc, s, nms, ps,
			    B_FALSE)) == (size_t)-1) {
				/* Invalid sequence - mbrtowc() sets errno. */
				return ((size_t)-1);
			} else if (nb == 0 || nb == (size_t)-2) {
				return (nchr);
			}
			s += nb;
			nms -= nb;
			nchr++;
		}
		/*NOTREACHED*/
	}

	/*
	 * The fast path in the loop below is not safe if an ASCII
	 * character appears as anything but the first byte of a
	 * multibyte sequence. Check now to avoid doing it in the loop.
	 */
	if (nms > 0 && len > 0 && us->want > 0 && (signed char)*s > 0) {
		errno = EILSEQ;
		return ((size_t)-1);
	}
	while (len-- > 0) {
		if (nms > 0 && (signed char)*s > 0) {
			/*
			 * Fast path for plain ASCII characters
			 * excluding NUL.
			 */
			*dst = (wchar_t)*s;
			nb = 1;
		} else if ((nb = _UTF8_mbrtowc(dst, s, nms, ps, B_FALSE)) ==
		    (size_t)-1) {
			*src = s;
			return ((size_t)-1);
		} else if (nb == (size_t)-2) {
			*src = s + nms;
			return (nchr);
		} else if (nb == 0) {
			*src = NULL;
			return (nchr);
		}
		s += nb;
		nms -= nb;
		nchr++;
		dst++;
	}
	*src = s;
	return (nchr);
}

static size_t
_UTF8_wcrtomb(char *_RESTRICT_KYWD s, wchar_t wc, mbstate_t *_RESTRICT_KYWD ps)
{
	_UTF8State *us;
	unsigned char lead;
	int i, len;

	us = (_UTF8State *)ps;

	if (us->want != 0) {
		errno = EINVAL;
		return ((size_t)-1);
	}

	if (s == NULL)
		/* Reset to initial shift state (no-op) */
		return (1);

	/*
	 * Determine the number of octets needed to represent this character.
	 * We always output the shortest sequence possible. Also specify the
	 * first few bits of the first octet, which contains the information
	 * about the sequence length.
	 */
	if ((wc & ~0x7f) == 0) {
		/* Fast path for plain ASCII characters. */
		*s = (char)wc;
		return (1);
	} else if ((wc & ~0x7ff) == 0) {
		lead = 0xc0;
		len = 2;
	} else if ((wc & ~0xffff) == 0) {
		lead = 0xe0;
		len = 3;
	} else if ((wc & ~0x1fffff) == 0) {
		lead = 0xf0;
		len = 4;
#if 0
	/* Again, 5 and 6 byte encodings are simply not permitted */
	} else if ((wc & ~0x3ffffff) == 0) {
		lead = 0xf8;
		len = 5;
	} else if ((wc & ~0x7fffffff) == 0) {
		lead = 0xfc;
		len = 6;
#endif
	} else {
		errno = EILSEQ;
		return ((size_t)-1);
	}

	/*
	 * Output the octets representing the character in chunks
	 * of 6 bits, least significant last. The first octet is
	 * a special case because it contains the sequence length
	 * information.
	 */
	for (i = len - 1; i > 0; i--) {
		s[i] = (wc & 0x3f) | 0x80;
		wc >>= 6;
	}
	*s = (wc & 0xff) | lead;

	return (len);
}

static size_t
_UTF8_wcsnrtombs(char *_RESTRICT_KYWD dst, const wchar_t **_RESTRICT_KYWD src,
    size_t nwc, size_t len, mbstate_t *_RESTRICT_KYWD ps)
{
	_UTF8State *us;
	char buf[MB_LEN_MAX];
	const wchar_t *s;
	size_t nbytes;
	size_t nb;

	us = (_UTF8State *)ps;

	if (us->want != 0) {
		errno = EINVAL;
		return ((size_t)-1);
	}

	s = *src;
	nbytes = 0;

	if (dst == NULL) {
		while (nwc-- > 0) {
			if (0 <= *s && *s < 0x80)
				/* Fast path for plain ASCII characters. */
				nb = 1;
			else if ((nb = _UTF8_wcrtomb(buf, *s, ps)) ==
			    (size_t)-1)
				/* Invalid character - wcrtomb() sets errno. */
				return ((size_t)-1);
			if (*s == L'\0')
				return (nbytes + nb - 1);
			s++;
			nbytes += nb;
		}
		return (nbytes);
	}

	while (len > 0 && nwc-- > 0) {
		if (0 <= *s && *s < 0x80) {
			/* Fast path for plain ASCII characters. */
			nb = 1;
			*dst = *s;
		} else if (len > (size_t)MB_CUR_MAX) {
			/* Enough space to translate in-place. */
			if ((nb = _UTF8_wcrtomb(dst, *s, ps)) == (size_t)-1) {
				*src = s;
				return ((size_t)-1);
			}
		} else {
			/*
			 * May not be enough space; use temp. buffer.
			 */
			if ((nb = _UTF8_wcrtomb(buf, *s, ps)) == (size_t)-1) {
				*src = s;
				return ((size_t)-1);
			}
			if (nb > (int)len)
				/* MB sequence for character won't fit. */
				break;
			(void) memcpy(dst, buf, nb);
		}
		if (*s == L'\0') {
			*src = NULL;
			return (nbytes + nb - 1);
		}
		s++;
		dst += nb;
		len -= nb;
		nbytes += nb;
	}
	*src = s;
	return (nbytes);
}
