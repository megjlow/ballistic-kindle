CC=gcc
LIBS=-l bluetooth
INC=-I.
ARGS=-c -O0 -g3 -Wall

ballistic-kindle : serial.o firmata.o servo.o request.o main.o
	${CC} -g3 -l bluetooth -o ballistic-kindle *.o

serial.o : serial.c
	${CC} ${ARGS} ${LIBS} ${INC} serial.c
	
firmata.o : firmata.c
	${CC} ${ARGS} ${LIBS} ${INC} firmata.c

servo.o : servo.c
	${CC} ${ARGS} ${LIBS} ${INC} servo.c

request.o : request.c
	${CC} ${ARGS} ${LIBS} ${INC} request.c
	
main.o : main.c
	${CC} ${ARGS} ${LIBS} ${INC} main.c
	
clean: 
	rm *.o
	rm ballistic-kindle