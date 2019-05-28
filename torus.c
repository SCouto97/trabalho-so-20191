#include "definitions.c"

#define N 16
int redirect[N+1] = {0,15,11,1,0,
					 	2, 8,5,0,
					 	1, 1,3,1,
					 	0, 0,0,0};

int childs[N+1] = {0,2,2,1,0,
					 1,2,2,0,
					 1,1,2,1,
					 0,0,0,0};

int father[N+1] = {0,0, 1, 2, 3,
					 1, 2, 6, 7,
					 5, 6, 7,11,
					 9,10,11,12};

/*
SCHEME
1 <  2  < 3 <  4
^    ^  
5    6 <  7 <  8
^    ^    ^
9   10   11 < 12
^    ^    ^    ^ 
13  14   15   16
*/

int child_pid[N+1];
int id = 1;

void terminate(){
	printf("Gerenciadores terminando...\n");
	fflush(stdout);
	
	int info;
	for(int i = 2; i <= N; ++i) {
		kill(child_pid[i], SIGKILL);
		wait(&info);
		printf("kill gerente pid: %d\n", child_pid[i]);
		fflush(stdout);
	}	
	
	exit(0);
}

int main(int argc, char* argv[]){
	argc = argc;
	argv = argv;
	signal(SIGTERM, terminate);
	
	int qid = msgget(KEY, IPC_CREAT | 0777);
	if(qid == -1){
		printf("Problema ao criar fila errno: %d\n", errno);
		exit(0);
	}
	
	for(int i = 2; i <= N; ++i){
		id = i;
		child_pid[i] = fork();
		if(child_pid[i] < 0){
			printf("Fork nao permitiu criar gerentes, programa nao ira executar corretamente\n");
			exit(0);
		}
		
		if(child_pid[i] == 0)break;
		id = 1;
	}
	
	while(TRUE){
		MessageEscalonador msge;
		msgrcv(qid, &msge, sizeof(msge)-sizeof(long int), ESCGER_MSG_TYPE+father[id], 0);
		
		if(redirect[id]){
			msge.msg_type = ESCGER_MSG_TYPE+id;
			int snd_child = childs[id];
			while(snd_child-->0)
				msgsnd(qid, &msge, sizeof(msge)-sizeof(long int), 0);
		}
		
		int exec_pid = fork();
		MessageGerente info;
		info.begin = time(NULL);
		if(exec_pid < 0){
			printf("Erro no fork %d\n", errno);
			exit(0);
		}else if(exec_pid == 0){
			char path[80] = "./";
			strcat(path, msge.prog_name);
			printf("pid %d executando: %s | %s\n", getpid(), path, msge.prog_name);
			int ret = execl(path, msge.prog_name,(char*) NULL);
			if(ret == -1){
				printf("Erro no execl %d\n", errno);
				exit(0);
			}
		}
		int ret;
		wait(&ret);
		info.end = time(NULL);
		info.pid = exec_pid;
		info.status = INFO;
		info.msg_type = ESCGER_MSG_TYPE + OFFSET + father[id] ;
				
		msgsnd(qid, &info, sizeof(info)-sizeof(long int), 0);
		int count_redirect = redirect[id];
		while(count_redirect-->0){
			msgrcv(qid, &info, sizeof(info)-sizeof(long int),ESCGER_MSG_TYPE + OFFSET + id, 0);
			info.msg_type = ESCGER_MSG_TYPE + OFFSET + father[id];
			msgsnd(qid, &info, sizeof(info)-sizeof(long int), 0);
		}
		
		if(id == 1){
			MessageGerente msg;
			msg.status = CONTROL;
			msg.msg_type = ESCGER_MSG_TYPE + OFFSET;
			msgsnd(qid, &msg, sizeof(msg)-sizeof(long int), 0);
		}
	}
	
	return 0;
}

