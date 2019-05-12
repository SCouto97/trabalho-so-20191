#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <unistd.h>
#include <signal.h>
#include <sys/signal.h>

#include <time.h>

 #include <sys/wait.h>

#define TRUE 1
#define FALSE 0

/*Chave da fila de mensagens entre escalonador e executa postergdo*/
const int KEY = 150143290;
/*Tipo de mensagens entre escalonador e executa postergado*/
const int ESC_MSG_TYPE = 1;
/*Mensagens de free e busy entre escalonador e gerente*/
const int ESCGER_CONTROL_MSG_TYPE = 2;
const int ESCGER_INFO_MSG_TYPE = 3;

const int SHUTDOWN_MSG_TYPE = 4;

typedef struct Msg0{
	long int msg_type;
	int sec;
	time_t submission;
	char prog_name[30];
}MessageEscalonador;

typedef struct Msg1{
	long int msg_type;
	int pid;
	time_t begin;
	time_t end;
}MessageGerente;

typedef struct nodeprocess{
	struct nodeprocess *next;
	MessageGerente msg;
}NodeProcess;

const int WAIT = 0;
const int RUNNIG = 1;
const int FINISHED = 2;

typedef struct nodejob{
	struct nodejob *next;
	int id;
	int status;
	int sec;
	char prog_name[30];
	time_t submission;
	time_t begin;
	time_t end;
	double makespan;
}NodeJob;

NodeJob* getNodeJob(MessageEscalonador *msg){
	NodeJob *new = (NodeJob*)malloc(sizeof(NodeJob));
	new->status = WAIT;
	new->sec = msg->sec;
	new->submission = msg->submission;
	strcpy(new->prog_name, msg->prog_name);
	return new;
}

void addNodeJob(NodeJob *node, NodeJob *other){
	if(node->next == NULL)node->next = other;
	else addNodeJob(node->next, other);
}

