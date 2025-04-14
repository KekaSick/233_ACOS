all: main

salam_lib.so: salam_lib.o
	gcc -shared -o salam_lib.so salam_lib.o

salam_lib.o: salam_lib.c salam_lib.h
	gcc -Wall -fPIC -c salam_lib.c -o salam_lib.o

main: main.o salam_lib.so
	gcc -o main main.o salam_lib.so -Wl,-rpath,'$$ORIGIN'

main.o: main.c salam_lib.h
	gcc -Wall -fPIC -c main.c -o main.o

clean:
	rm -f *.o main salam_lib.so
