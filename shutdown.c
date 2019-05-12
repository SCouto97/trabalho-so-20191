#include "definitions.c"

int main(){
	int qid = msgget(KEY, 0777);
	if(errno == ENOENT){
		printf("Fila para termino de processo ainda n existe errno: %d\n", errno);
		exit(0);
	}
	
	struct shutdown{long int type; int pid;}msg;
	int ret = msgrcv(qid, &msg, sizeof(msg)-sizeof(long int), SHUTDOWN_MSG_TYPE, IPC_NOWAIT);
	if(ret == -1){
		printf("Nao ha processo para finalizar\n");
	}else{
		printf("Kill enviado para pid: %d\n", msg.pid);
		kill(msg.pid,SIGTERM);
	}

	return 0;
}