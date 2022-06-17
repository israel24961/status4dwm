include config.mk
SRCS= ${SRCDIR}/${NAME}.c

all: config.mk info build ${SRCS}



build: dirs ${SRCS}
	@${CC} ${SRCS} -o ${BINDIR}/${NAME}

clean: 
	@echo "Cleaning"
	@rm -rd ${BINDIR} || echo "Error removing"

info: 
	@echo "${NAME} v${VERSION} compiled with:"
	@echo "CC       = ${CC}"
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CLIBS    = ${CLIBS}"

debug: CFLAGS += -g -DDEBUG
debug: all

dirs:
		@mkdir -p ${BINDIR}
