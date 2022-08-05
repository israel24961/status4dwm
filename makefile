include config.mk
SRCS= ${SRCDIR}/${NAME}.c			\
			${SRCDIR}/stat_stuff.c	\
			${SRCDIR}/tasks.c \
			${SRCDIR}/utils.c

all: config.mk info build ${SRCS}



build: dirs ${SRCS}
	@${CC} ${SRCS} -o ${BINDIR}/${NAME} ${CFLAGS} ${CLIBS}

clean:
	@echo "Cleaning"
	@rm -rd ${BINDIR} || echo "Error removing"

info:
	@echo "${NAME} v${VERSION} compiled with:"
	@echo "CC       = ${CC}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CLIBS    = ${CLIBS}"

debug: Os =
debug: CFLAGS += -Wall -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5  -Wwrite-strings -Waggregate-return \
	-ggdb3  -DDEBUG
debug: all

dirs:
		@mkdir -p ${BINDIR}
