CC=./shecc/out/shecc
CFLAGS=

SOURCES = $(wildcard src/*.c)

out/shepherd: a.out out
	cp a.out out/shepherd; \
	chmod 777 out/shepherd; \
	rm -f a.out

a.out: src/main.c $(CC)
	$(CC) $(CFLAGS) src/main.c

out:
	mkdir out

$(CC):
	git clone https://github.com/sysprog21/shecc.git; \
	cd shecc; \
	make; \
	cd ..;

run: out/shepherd
	./out/shepherd

clean:
	rm -rf src/*.o out/

clean-shecc:
	rm -rf shecc/

rebuild: clean all
