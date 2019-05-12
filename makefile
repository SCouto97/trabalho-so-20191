ARGS=-Wall -Wshadow -Wextra -Werror
GCC=gcc

all: c_exec c_tree c_shutdown c_escalonador

c_escalonador:
	$(GCC) $(ARGS) escalonador.c -o escalonador
	
c_exec:
	$(GCC) $(ARGS) executa_postergado.c -o executa_postergado

c_tree:
	$(GCC) $(ARGS) tree.c -o tree

c_shutdown:
	$(GCC) $(ARGS) shutdown.c -o shutdown

term:
	./shutdown

r:
	./executa_postergado 1 prog
#	./escalonador tree

clean:
	rm tree
	rm shutdown
	rm executa_postergado
	rm escalonador