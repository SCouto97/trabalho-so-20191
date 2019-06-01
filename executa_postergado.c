/*
Marcos Paulo - 15/0154208
Samuel Couto - 15/0021623
Nur Corezzi - 15/0143290
Raphael Soares - 140160299
*/

#include "definitions.c"

int main(int argc, char* argv[]){
	/*parametro de tempo*/
	if(argc != 3){
		printf("Quantidade de parametros incorreta.\n");
		printf("Uso correto: executa_postergado <seg> <arq_executavel>\n");
	}else{
		if(access(argv[2], F_OK) == -1){
			printf("Arquivo a ser executado nao existe ou é inacessivel\n");
			exit(0);
		} 
		
		int qid = msgget(KEY, 0777);
		if(errno == ENOENT){
			printf("Fila de mensagens ainda nao foi criada, escalonador nao esta rodando errno: %d\n", errno);
			exit(0);
		}else{
			
			MessageEscalonador msg;
			msg.msg_type = ESC_MSG_TYPE;
			msg.sec = atoi(argv[1]);
			msg.submission = time(NULL);
			strcpy(msg.prog_name,argv[2]);
			msgsnd(qid, &msg, sizeof(msg)-sizeof(long int), 0);
		}
	}
	
    return 0;
}
