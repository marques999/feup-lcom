
COMPILER_TYPE= gnu

CC=gcc

PROG=	arkanix

SRCS+=	arkanix.c
SRCS+=  input.c timer.c rtc.c serial.c
SRCS+=  vbe.c video.c memset.S
SRCS+=  bitmap.c sprite.c menu.c
SRCS+=  generate.c level.c ss.c score.c

CCFLAGS= -Wall -O3 -march=native

DPADD+=	${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
