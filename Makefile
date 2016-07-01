all: hello_world.c
	gcc -g -masm=intel hello_world.c

clean: a.out
	rm a.out
