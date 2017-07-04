CC=g++
#CFLAGS=-I/usr/include -I/usr/local/include -static
CFLAGS=-I/usr/include -I/usr/local/include -I./src/
BUILDDIR=bin
OBJECTS=GaborRecMain.o ReadFile.o ExtractGaborjet.o Similarity.o qface.o bpnn.o	

GaborRecMain:$(OBJECTS)
	$(CC) $(CFLAGS) -o GaborRecMain $(OBJECTS) -lm -pthread
GaborRecMain.o:src/ReadFile.h src/GaborRecMain.c
	$(CC) -c src/GaborRecMain.c 
ReadFile.o:src/ReadFile.h src/ExtractGaborjet.h src/ReadFile.c
	$(CC) -c src/ReadFile.c 
ExtractGaborjet.o:src/ExtractGaborjet.h src/ExtractGaborjet.c
	$(CC) -c src/ExtractGaborjet.c
Similarity.o:src/Similarity.h src/Similarity.c src/ExtractGaborjet.h
	$(CC) -c src/Similarity.c
qface.o:src/qface.h  src/qface.cpp
	$(CC) -c src/qface.cpp
bpnn.o:src/bpnn.h  src/bpnn.cpp
	$(CC) -c src/bpnn.cpp

clean:
	rm -rf *.o
