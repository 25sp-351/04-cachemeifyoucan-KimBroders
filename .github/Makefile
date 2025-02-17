CC      = clang
CFLAGS  = -g -Wall
RM      = rm -f


default: all

all: main.exe
segments.o: segments.c segments.h
	$(CC) $(CFLAGS) -c segments.c

rodcut.o: rodcut.c rodcut.h segments.h cache.h
	$(CC) $(CFLAGS) -c rodcut.c

LRUcache.o: cache.c cache.h segments.h
	$(CC) $(CFLAGS) -c cache.c -o LRUcache.o -DLRU

RRcache.o: cache.c cache.h segments.h
	$(CC) $(CFLAGS) -c cache.c -o RRcache.o

LRUmain.exe: main.c rodcut.o segments.o LRUcache.o
	$(CC) $(CFLAGS) -o LRUmain.exe rodcut.o segments.o LRUcache.o main.c

RRmain.exe: main.c rodcut.o segments.o RRcache.o
	$(CC) $(CFLAGS) -o RRmain.exe rodcut.o segments.o RRcache.o main.c

clean veryclean:
	$(RM) *.exe
	$(RM) *.ilk
	$(RM) *.pdb
	$(RM) *.o
	$(RM) *.json

LRUtest: LRUmain.exe
	./LRUmain.exe -q data.txt < rodlengths.txt > LRUoutput.json

RRtest: RRmain.exe
	./RRmain.exe -q data.txt < rodlengths.txt > RRoutput.json