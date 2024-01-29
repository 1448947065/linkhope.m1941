CC := gcc
LIBNAME := m1941
MAJOR := 1
MINOR := 0
RELEASE := 2

all: static

	
static:
	@if [ ! -d "obj" ]; then mkdir obj; fi
	cd obj && $(CC) -c -o m1941lib.o ../m1941lib.c
	@if [ ! -d "lib" ]; then mkdir lib; fi
	cd lib && ar rcs lib$(LIBNAME).a ../obj/m1941lib.o

.PHONY: clean

clean:
	rm -rf obj lib

