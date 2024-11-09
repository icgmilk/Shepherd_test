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

$(CC): shecc
	cd shecc; \
	make; \
	cd ..;

update-shecc:
	git submodule foreach git pull origin master

shecc:
	git submodule add https://github.com/sysprog21/shecc.git shecc

run: out/shepherd
	./out/shepherd

clean:
	rm -rf src/*.o out/

rebuild: clean all
