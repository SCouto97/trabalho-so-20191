#include "definitions.c"

int running = TRUE;

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
		
		int qid = msgget(KEY, IPC_CREAT | 0777);
		if(qid == -1){
			printf("Problema ao criar fila errno: %d\n", errno);
			exit(0);
		}
		
		if(strcmp(argv[1], "tree")){
			
		}else if(strcmp(argv[1], "hypercube")){
			
			/*Iniciar programa gerenciador aqui*/
		
		}else if(strcmp(argv[1], "torus")){
		
			/*Iniciar programa gerenciador aqui*/
		
		}else{
			printf("Topologia incorreta informada utilize: tree, hypercube, torus\n");
			exit(0);
		}
		
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
				next->status = FINISHED;
				printf("Executando job %d\n", next->id);
			}
			
			fflush(stdout);
		}
	}

    return 0;
}