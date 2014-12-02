CC=icpc
LD=icpc

CFLAGS=-O2 -msse3 -Wall -Werror -DDEBUG -ansi -no-opt-matmul

all : generator

gen_objs = GeneratorCSC.o ReaderCSC.o GeneratorDense.o

GeneratorDriver.o : GeneratorDriver.cpp
	$(CC) $(CFLAGS) -c GeneratorDriver.cpp
		
GeneratorCSC.o : GeneratorCSC.cpp
	$(CC) $(CFLAGS) -c GeneratorCSC.cpp

GeneratorDense.o : GeneratorDense.cpp
	$(CC) $(CFLAGS) -c GeneratorDense.cpp
	
ReaderCSC.o : ReaderCSC.cpp
	$(CC) $(CFLAGS) -c ReaderCSC.cpp
	
generator : $(gen_objs) GeneratorDriver.o
	$(CC) $(CFLAGS) $(gen_objs) GeneratorDriver.o -o generator.exe
	
clean :
	rm -rfv *.o *.mod *.exe
