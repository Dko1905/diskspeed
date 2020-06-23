cc=gcc
cflags=-Wall -Werror -std=c11

diskspeed: main.o write.o read.o opt.o
	mkdir -p dist
	$(cc) main.o write.o read.o opt.o -o dist/diskspeed $(cflags)

main.o: src/main.c
	$(cc) -c src/main.c $(cflags)
write.o: src/write.c
	$(cc) -c src/write.c $(cflags)
read.o: src/read.c
	$(cc) -c src/read.c $(cflags)
opt.o: src/opt.c
	$(cc) -c src/opt.c $(clfags)
clean:
	rm -rf *.o build/*
