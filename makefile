ARGS=-Wall -Wshadow -Wextra -Werror
GCC=gcc

all: c_exec c_tree c_shutdown c_escalonador c_torus c_hypercube

c_escalonador:
	$(GCC) $(ARGS) escalonador.c -o escalonador
	
c_exec:
	$(GCC) $(ARGS) executa_postergado.c -o executa_postergado

c_tree:
	$(GCC) $(ARGS) tree.c -o tree

c_torus:
	$(GCC) $(ARGS) torus.c -o torus
c_hypercube:
	$(GCC) $(ARGS) hypercube.c -o hypercube

c_shutdown:
	$(GCC) $(ARGS) shutdown.c -o shutdown

term:
	./shutdown

r:
	./executa_postergado 10 test1
	
e:
	./escalonador torus

clean:
	rm -f tree
	rm -f shutdown
	rm -f executa_postergado
	rm -f escalonador
	rm -f torus
	rm -f hypercube