cc=gcc
cflags=-Wall -Werror -std=c11
# Please find good c version.
diskspeed: main.o write.o read.o
	mkdir -p build
	$(cc) main.o write.o read.o -o build/diskspeed $(cflags)

main.o: src/main.c
	$(cc) -c src/main.c $(cflags)

write.o: src/write.c
	$(cc) -c src/write.c $(cflags)
read.o: src/read.c
	$(cc) -c src/read.c $(cflags)
clean:
	rm -rf *.o build/*