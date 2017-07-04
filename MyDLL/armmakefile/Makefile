CC=arm-linux-gcc
#CFLAGS=-I/usr/include -I/usr/local/include
#CFLAGS=-I/usr/local/arm/2.95.3/include -I/usr/local/arm/2.95.3/arm-linux/include
CFLAGS=-g -O2 -Wall -I
BUILDDIR=bin

OBJECTS=GaborRecMain.o ReadFile.o ExtractGaborjet.o Similarity.o	
#OBJECTS=GaborRecMain.o
GaborRecMain:$(OBJECTS)
	$(CC)  -o GaborRecMain $(OBJECTS) -lm -pthread
GaborRecMain.o:src/ReadFile.h src/GaborRecMain.c
	$(CC) -c src/GaborRecMain.c 
ReadFile.o:src/ReadFile.h src/ExtractGaborjet.h src/ReadFile.c
	$(CC) -c src/ReadFile.c 
ExtractGaborjet.o:src/ExtractGaborjet.h src/ExtractGaborjet.c
	$(CC) -c src/ExtractGaborjet.c
Similarity.o:src/Similarity.h src/Similarity.c src/ExtractGaborjet.h
	$(CC) -c src/Similarity.c

.PHONY:clean
clean:
	rm -rf *.o


