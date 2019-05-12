all:
	gcc -Wall escalonador.c -o escalonador
	
exec:
	gcc -Wall executa_postergado.c -o executa_postergado

r:
	./executa_postergado 1 prog
#	./escalonador tree
	