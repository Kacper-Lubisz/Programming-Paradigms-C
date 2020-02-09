
all: bin/ bin/gameoflife libgol.dll
	echo "Project Built"

bin/gameoflife: bin/ libgol.dll
	gcc -Wall -Wextra -pedantic -std=c11 src/gameoflife.c -o gameoflife -L. -lgol

libgol.dll: bin/ bin/gol.o
	gcc -Wall -Wextra -pedantic -std=c11 -shared bin/gol.o -o libgol.dll

bin/gol.o: bin/
	gcc -Wall -Wextra -pedantic -std=c11 -fPIC -c src/gol.c -o bin/gol.o

bin/:
	[ -d bin ] || mkdir bin

clean:
	rm bin/*
	rm ./libgol.dll
