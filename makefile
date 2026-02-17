include config.mk
SRCS= ${SRCDIR}/${NAME}.c			\
			${SRCDIR}/stat_stuff.c	\
			${SRCDIR}/tasks.c

TARGET = ${BINDIR}/${NAME}

all: config.mk info ${TARGET}

${TARGET}: ${SRCS} | dirs

build: dirs ${SRCS}
	@${CC} ${CFLAGS} ${SRCS} -o ${BINDIR}/${NAME}  ${CLIBS}

clean: 
	@echo "Cleaning"
	@rm -rf ${BINDIR} || echo "Error removing"

info: 
	@echo "${NAME} v${VERSION} compiled with:"
	@echo "CC       = ${CC}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CLIBS    = ${CLIBS}"

debug: CFLAGS += -g -DDEBUG
debug: all

dirs:
		@mkdir -p ${BINDIR}
