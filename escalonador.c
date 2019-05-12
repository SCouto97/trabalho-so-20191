#include "definitions.c"

int running = TRUE;
int gerente_pid;

void terminate(){
	printf("Escalonador terminando...\n");
	running = FALSE;
}

NodeJob* poolNextJob(NodeJob *job){
	if(job == NULL) return NULL;
	else{
		if(job->status == WAIT && (int)difftime(time(NULL),job->submission) > job->sec) return job;
		else return poolNextJob(job->next);
	}
}

int main(int argc, char* argv[]){
	signal(SIGTERM, terminate);
	
    /*parametro de tempo*/
	if(argc != 2){
		printf("Quantidade de parametros incorreta.\n");
		printf("Uso correto: executa_postergado <topologia>\n");
	}else{
		
		if(strcmp(argv[1], "tree") == 0){
			gerente_pid = fork();
			if(gerente_pid < 0){
				printf("Erro no fork %d\n", errno);
				exit(0);
			}
			
			if(gerente_pid == 0){
				int ret = execl("./tree", "tree", (char*) NULL);
				if(ret == -1){
					printf("Erro no execl %d\n", errno);
				}
			}
				
		}else if(strcmp(argv[1], "hypercube")){
			
			/*Iniciar programa gerenciador aqui*/
		
		}else if(strcmp(argv[1], "torus")){
		
			/*Iniciar programa gerenciador aqui*/
		
		}else{
			printf("Topologia incorreta informada utilize: tree, hypercube, torus\n");
			exit(0);
		}
		
		int qid = msgget(KEY, IPC_CREAT | 0777);
		if(qid == -1){
			printf("Problema ao criar fila errno: %d\n", errno);
			exit(0);
		}
		
		struct shutdown{long int type; int pid;} shut_msg;
		shut_msg.type = SHUTDOWN_MSG_TYPE;
		shut_msg.pid = getpid();
		msgsnd(qid, &shut_msg, sizeof(shut_msg)-sizeof(long int), 0);
		
		NodeJob *jobList = NULL;
		int njobs = 1;
		/*pooling de mensagens*/
		while(running){
			MessageEscalonador msg;
			int ret = msgrcv(qid, &msg, sizeof(msg)-sizeof(long int), ESC_MSG_TYPE, IPC_NOWAIT);
			if(ret != -1){
				printf("Mensagem recebida de executar_postergado: %d %s\n", msg.sec, msg.prog_name);
				
				/*Adicionar novo no a lista de jobs*/
				NodeJob *new = getNodeJob(&msg);
				new->id = njobs++;
				if(jobList == NULL)jobList=new;
				else addNodeJob(jobList, new);
			}
			
			NodeJob *next = poolNextJob(jobList);
			if(next != NULL){
				MessageGerente msgg;
				printf("Esperando liberar gerenciador\n");
				fflush(stdout);
				msgrcv(qid, &msgg, sizeof(msgg)-sizeof(long int), ESCGER_CONTROL_MSG_TYPE, 0);
				
				MessageEscalonador msge;
				msge.msg_type = ESCGER_INFO_MSG_TYPE;
				strcpy(msge.prog_name, next->prog_name);
				msgsnd(qid, &msge, sizeof(msge)-sizeof(long int), 0);
				
				printf("Esperando finalizacao gerenciador\n");
				fflush(stdout);
				msgrcv(qid, &msgg, sizeof(msgg)-sizeof(long int), ESCGER_CONTROL_MSG_TYPE, 0);
				
				printf("job %d terminado\n", next->id);
				next->status = FINISHED;
			}
			
			fflush(stdout);
		}
		
		kill(gerente_pid, SIGTERM);
		int info;
		wait(&info);
		if(msgctl(qid, IPC_RMID, NULL) == -1){
			printf("Erro ao excluir fila errno: %d\n", errno);
		}
	}

    return 0;
}
