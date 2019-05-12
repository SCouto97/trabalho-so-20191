#include "definitions.c"

int main(int argc, char* argv[]){
	/*parametro de tempo*/
	if(argc != 3){
		printf("Quantidade de parametros incorreta.\n");
		printf("Uso correto: executa_postergado <seg> <arq_executavel>\n");
	}else{
		int qid = msgget(KEY, IPC_CREAT | 0777);
		if(qid == -1){
			printf("Problema ao criar fila errno: %d\n", errno);
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