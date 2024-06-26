/*
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */


/*
 * Copyright (C) 1998 by the FundsXpress, INC.
 *
 * All rights reserved.
 *
 * Export of this software from the United States of America may require
 * a specific license from the United States Government.  It is the
 * responsibility of any person or organization contemplating export to
 * obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of FundsXpress. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  FundsXpress makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "k5-int.h"

/*
 * Solaris Kerberos defines memory management macros in <krb5.h>,
 * which is included by <k5-int.h>, so we need not include <memory.h>
 */
/* #include <memory.h> */

/*
n-fold(k-bits):
  l = lcm(n,k)
  r = l/k
  s = k-bits | k-bits rot 13 | k-bits rot 13*2 | ... | k-bits rot 13*(r-1)
  compute the 1's complement sum:
	n-fold = s[0..n-1]+s[n..2n-1]+s[2n..3n-1]+..+s[(k-1)*n..k*n-1]
*/

/* representation: msb first, assume n and k are multiples of 8, and
   that k>=16.  this is the case of all the cryptosystems which are
   likely to be used.  this function can be replaced if that
   assumption ever fails.  */

/* input length is in bits */

void
krb5_nfold(unsigned int inbits, const unsigned char *in, unsigned int outbits,
	   unsigned char *out)
{
    int a,b,c,lcm;
    int byte, i, msbit;

    /* the code below is more readable if I make these bytes
       instead of bits */

    inbits >>= 3;
    outbits >>= 3;

    /* first compute lcm(n,k) */

    a = outbits;
    b = inbits;

    while(b != 0) {
	c = b;
	b = a%b;
	a = c;
    }

    lcm = outbits*inbits/a;

    /* now do the real work */

    (void) memset(out, 0, outbits);
    byte = 0;

    /* this will end up cycling through k lcm(k,n)/k times, which
       is correct */
    for (i=lcm-1; i>=0; i--) {
	/* compute the msbit in k which gets added into this byte */
	msbit = (/* first, start with the msbit in the first, unrotated
		    byte */
		 ((inbits<<3)-1)
		 /* then, for each byte, shift to the right for each
		    repetition */
		 +(((inbits<<3)+13)*(i/inbits))
		 /* last, pick out the correct byte within that
		    shifted repetition */
		 +((inbits-(i%inbits))<<3)
		 )%(inbits<<3);

	/* pull out the byte value itself */
	byte += (((in[((inbits-1)-(msbit>>3))%inbits]<<8)|
		  (in[((inbits)-(msbit>>3))%inbits]))
		 >>((msbit&7)+1))&0xff;

	/* do the addition */
	byte += out[i%outbits];
	out[i%outbits] = byte&0xff;

#if 0
	printf("msbit[%d] = %d\tbyte = %02x\tsum = %03x\n", i, msbit,
	       (((in[((inbits-1)-(msbit>>3))%inbits]<<8)|
		 (in[((inbits)-(msbit>>3))%inbits]))
		>>((msbit&7)+1))&0xff, byte);
#endif

	/* keep around the carry bit, if any */
	byte >>= 8;

#if 0
	printf("carry=%d\n", byte);
#endif
    }

    /* if there's a carry bit left over, add it back in */
    if (byte) {
	for (i=outbits-1; i>=0; i--) {
	    /* do the addition */
	    byte += out[i];
	    out[i] = byte&0xff;

	    /* keep around the carry bit, if any */
	    byte >>= 8;
	}
    }
}

