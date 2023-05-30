all: simulator

submit: cashier.c
	tar cf hw7.tar $<

simulator: cashier.c main.c clean
	gcc -Wall -Wextra -Wpedantic  -std=c11 -g cashier.c main.c -o $@

clean:
	rm -rf simulator *.tar

mem: simulator
	valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-error-list=yes \
./simulator

run: simulator
	./simulator

.PHONY: clean mem run
