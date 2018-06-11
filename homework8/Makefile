all: main.o

main.o: main.c
	g++ -o main main.c

run:
	./main

clean:
	rm -f main

pack:
	make clean
	zip hw8.zip *