#include "definitions.c"

void terminate(){
	printf("Gerenciadores terminando...\n");
	fflush(stdout);
	exit(0);
}

int main(){
	signal(SIGTERM, terminate);
	
	int qid = msgget(KEY, IPC_CREAT | 0777);
	if(qid == -1){
		printf("Problema ao criar fila errno: %d\n", errno);
		exit(0);
	}
	
	while(TRUE){
		MessageGerente msg;
		msg.msg_type = ESCGER_CONTROL_MSG_TYPE;
		msgsnd(qid, &msg, sizeof(msg)-sizeof(long int), 0);
		
		MessageEscalonador msge;
		msgrcv(qid, &msg, sizeof(msge)-sizeof(long int), ESCGER_INFO_MSG_TYPE, 0);
		
		sleep(5);
		
		msgsnd(qid, &msg, sizeof(msg)-sizeof(long int), 0);
	}
	
	return 0;
}

