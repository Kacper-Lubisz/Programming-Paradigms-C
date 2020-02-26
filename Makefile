C_ARGS = -Wall -Wextra -pedantic -std=c11

# easy port to winMake
SO_SUFFIX = .so
SO_NAME = "libgol$(SO_SUFFIX)"

gameoflife: $(SO_NAME)
	@(gcc $(C_ARGS)  gameoflife.c -o ./gameoflife -L. -lgol  && echo "SUCCESS gameoflife Linked") || (echo "FAIL gameoflife Linked" && exit 1)


$(SO_NAME): gol.o
	@(gcc $(C_ARGS) -shared gol.o -o $(SO_NAME) && echo "SUCCESS $(SO_NAME) Assembled") || (echo "FAIL $(SO_NAME) Assembled" && exit 1)

gol.o: gol.c
	@(gcc $(C_ARGS) -fPIC -c gol.c -o gol.o && echo "SUCCESS gol.o Assembled") || (echo "FAIL gol.o Assembled" && exit 1)

clean:
	@rm *.o || echo "rm gol.o failed" || exit 0
	@rm *$(SO_SUFFIX)  || echo "rm $(SO_NAME) failed"  || exit 0
	@rm gameoflife || echo "rm gameoflife failed"  || exit 0

