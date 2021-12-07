CFLAGS = -Wall -Wshadow -O3 -g -march=native
LDLIBS = -lsndfile -lm -lfftw3
CC = gcc

main: decimate.o filter.o
	$(CC) $(CFLAGS) $(LDLIBS) decimate.o filter.o -o build/decimate

clean:
	$(RM) *.o
	$(RM) build/decimate