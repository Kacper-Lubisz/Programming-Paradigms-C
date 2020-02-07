
./gameoflife: libgol.so
	gcc -Wall -Wextra -pedantic -std=c11 gameoflife.c -o gameoflife

libgol.so:
	gcc -Wall -Wextra -pedantic -std=c11

clean:
	rm ./gameoflife