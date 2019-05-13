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

void endChild(){
	kill(gerente_pid, SIGTERM);
	int info;
	wait(&info);
}


void printJobInfo(NodeJob job){
	#define SZ 50
	char sub_date[SZ];
	char begin_date[SZ];
	char end_date[SZ];
	char pattern[SZ]="%d/%m/%y,%H:%M:%S";
	
	strftime (sub_date,SZ,pattern,localtime(&job.submission));
	strftime (begin_date,SZ,pattern,localtime(&job.begin));
	strftime (end_date,SZ,pattern,localtime(&job.end));
	
	printf("\t%d\t%d\t%s\t%s\t%s\t%s\t%f\n", job.id, job.sec, job.prog_name, sub_date, begin_date, end_date, job.makespan);
	NodeProcess *next = job.exec_info;
	printf("\t\t   PID\tINICIO\tFIM\n");
	while(next != NULL){
		strftime (begin_date,SZ,pattern,localtime(&(next->begin)));
		strftime (end_date,SZ,pattern,localtime(&(next->end)));
		printf("\t\t|->%d\t%s\t%s\n", next->pid, begin_date, end_date);
		next = next->next;
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
				char buffer[10];
				sprintf(buffer, "%d", getpid());
				int ret = execl("./tree", "tree", buffer,(char*) NULL);
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
				
				printf("Enviando informacoes do job %d\n", next->id);
				fflush(stdout);
				/*Enviar dados sobre o programa a ser executado*/
				MessageEscalonador msge;
				msge.msg_type = ESCGER_MSG_TYPE;
				strcpy(msge.prog_name, next->prog_name);
				while(msgsnd(qid, &msge, sizeof(msge)-sizeof(long int), 0) == -1){
					if(errno == EINTR) continue;
					else{
						/*Deu erro cabuloso nao tem o que fazer*/
						endChild();
						exit(0);
					}
				};
				
				
				/*Fetch de informacoes dos processos executados*/
				printf("Fetching informacoes de execucao\n");
				fflush(stdout);	
				while(TRUE){
					MessageGerente info;
					while(msgrcv(qid, &info, sizeof(info)-sizeof(long int), ESCGER_MSG_TYPE + OFFSET, 0) == -1){
						if(errno == EINTR) continue;
						else{
							/*Deu erro cabuloso nao tem o que fazer*/
							printf("Mensagem recebida %d\n", errno);
							fflush(stdout);	
					
							endChild();
							exit(0);
						}
					}
					
					
					if(info.status == INFO){
						NodeProcess *new = getNodeProcess(&info);
						if(next->exec_info == NULL)next->exec_info=new;
						else addNodeProcess(next->exec_info, new);
					}else{
						break;
					}
				}
				
				printf("job %d terminado\n", next->id);
				next->status = FINISHED;
				printJobInfo(*next);
			}
			
			fflush(stdout);
		}
		
		endChild();
		if(msgctl(qid, IPC_RMID, NULL) == -1){
			printf("Erro ao excluir fila errno: %d\n", errno);
		}
	}

    return 0;
}
