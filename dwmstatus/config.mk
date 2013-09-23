NAME = dwmstatus
VERSION = 1.2

# Customize below to fit your system

# paths
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

IWLIB  = -liw
IWFLAG = -DIW

MPDLIB  = -lmpdclient
MPDFLAG = -DMPD

X11INC = ${PREFIX}/include/X11
X11LIB = ${PREFIX}/lib/X11

# includes and libs
INCS = -I. -I/usr/include -I${X11INC}
LIBS = -L/usr/lib -lc -L${X11LIB} -lX11 ${IWLIB} ${MPDLIB}

# flags
CPPFLAGS = ${IWFLAG} ${MPDFLAG} -DVERSION=\"${VERSION}\"
#CFLAGS = -g -std=c99 -pedantic -Wall -O3 ${INCS} ${CPPFLAGS}
CFLAGS = -std=c99 -pedantic -Wall -O3 ${INCS} ${CPPFLAGS}
#LDFLAGS = -g ${LIBS}
LDFLAGS = -s ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

CC = cc
