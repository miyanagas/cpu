CC = gcc
OBJS = main.o cpuboard.o
PROGRAM = cpu

all: ${PROGRAM}

${PROGRAM}: ${OBJS}
			${CC} ${OBJS} -o ${PROGRAM}
			
clean:		
			del *.o