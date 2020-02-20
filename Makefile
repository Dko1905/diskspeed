cc=gcc
cflags=-Wall -Werror -std=gnu11
# Please find good c version.
diskspeed: main.o
	mkdir -p build
	$(cc) main.o -o build/diskspeed $(cflags)

main.o: src/main.c
	$(cc) -c src/main.c $(cflags)

clean:
	rm -rf *.o build/*