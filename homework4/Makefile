.PHONY: all clean run	

all: main
	
sort.o: sort.c
	gcc -c $< -shared -fpic -o $@

libsort.so: sort.o
	gcc $< -shared -fpic -o $@
	
output.o: output.c
	gcc -c $< -shared -fpic -o $@

liboutput.so: output.o
	gcc $< -shared -fpic -o $@

main: main.o input.o libsort.so liboutput.so
	gcc input.o -L . -Wl,-rpath=. $< -o $@ -lsort -ldl -loutput

main.o: main.c
	gcc -c $< -o $@

input.o: input.c
	gcc -c $< -o $@

run: main
	./main

clean:
	rm -f *.o *.so main
