#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet
# at http://www.illumos.org/license/CDDL.
#

#
# Copyright 2010 Nexenta Systems, Inc.  All rights reserved.
#


PROG=		tr
XPG4PROG=	$(PROG)
XPG6PROG=	$(PROG)

OBJS=		tr.o str.o cset.o cmap.o
SRCS=	 	$(OBJS:%.o=%.c)

include ../Makefile.cmd

CLOBBERFILES=	$(PROG)


CSTD=	$(CSTD_GNU99)
CPPFLAGS +=	-D_ILLUMOS_PRIVATE -I.

# install rules
$(ROOTINC)/% : %
	$(INS.file)

.KEEP_STATE:

.PARALLEL: $(OBJS)

all:		$(PROG)

$(PROG):	$(OBJS)
	$(LINK.c) $(OBJS) -o $@ $(LDLIBS)
	$(POST_PROCESS)

install: all .WAIT $(ROOTPROG) $(ROOTXPG4PROG) $(ROOTXPG6PROG)

$(ROOTXPG4PROG) $(ROOTXPG6PROG):
	-$(RM) $@
	-$(LN) -s ../../bin/$(PROG) $@

clean:
	$(RM) $(OBJS)

include ../Makefile.targ
