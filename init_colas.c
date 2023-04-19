#include <stdio.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 

int main(int argc,char *argv[]) 
{ 	
	key_t key; 
	int enviar;
	int msgid;
	//creamos cola de receptores y main
	for(int i=1;i<=2;i++){
	
	 key = ftok("./",i ); 
	
	msgid = msgget(key, 0777| IPC_CREAT);
	
	if (msgid==-1) printf("Error al crear la cola de mensajes");
	else {
		printf("[Cola %d] ID: %d\n",i,msgid);
	}
	}		
	 
	return 0; 
} 

