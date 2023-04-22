#include <stdio.h> 
#include <stdlib.h> 
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>



#define MAX(i,j) (((i)>(j)) ? (i) : (j))

// --------- VARIABLES COMPARTIDAS----------
typedef struct datos_comp{
	int mi_ticket;
	int mi_id;
	int id_nodos_pend[100];
	int num_pend;
	int quiero;
	int max_ticket;
	//añadidas
	int dentro;
	int procesos;
}datos_comp;

struct recibir{
	int mi_ticket;
	int mi_pid;
	int mi_id;
	char text[100];
	long type;
}datos_recibir;

struct enviar{
	int id_aux;
	long type;
}datos_enviar;

struct enviar_ack{ 
    long type;
    int id_nodo;
}ack;



int main(int argc,char *argv[]) {

	//-------------VARIABLE PROPIAS--------------------------
	
	int id_nodo_origen = 0, ticket_origen = 0, pid_origen=0;
	
	 if (argc != 2){
		printf("formato incorrecto: ./v1_receptor posicion \n");
		exit(-1);
	}
		
	int posicion=atoi(argv[1]);
	int buzon=1235+posicion;
	

	
	//----------VARIABLES DE LA MEMORIA COMPARTIDA---------------
	key_t clave1; //clave de acceso a la memoria 1
	int shmid1; //identificador de la zona de memoria 1
	//-----------------------CREACION DE BUZONES DE MENSAJES-----------------------------------------------------------------
	int msqid = msgget(500,0777 | IPC_CREAT);


	//-----------------------FIN DE CREACION DE BUZONES DE MENSAJES----------------------------------------------------------
	//-------------CREACION MEMORIA COMPARTIDA-------------------------------------
	clave1 = ftok(".",posicion); //creamos la clave que utilizaremos para crear la zona de memoria y luego poder vincularla
	 
	shmid1 = shmget(clave1, sizeof(datos_comp), IPC_CREAT|0660);//Creación de zona_mem1
	 
	if(shmid1 !=-1){
		printf("Zona de memoria creada OK! con identificador %d \n", shmid1);
	}else{
		printf("ERROR al crear zona de memoria 1 !! \n");
		exit(-1);
	}
	 	 
	datos_comp *datos =(datos_comp *)shmat(shmid1,0,0);//Vinculamos el proceso con la zona de memoria
	 
	if(datos == (datos_comp*)-1){
		printf("Error al vincular el proceso a la zona de memoria 1! \n");
		exit(1);
	}
	
	datos->dentro=0;
	datos->procesos=0;
		
	//---------------------------------------DECLARACION SEMÁFOROS---------------------
	char name_mutex[50];
	sprintf(name_mutex, "/MUTEX%s", argv[1]);
	sem_t *sem_mutex;
	sem_mutex = sem_open(name_mutex, O_CREAT, 0777, 1);
	if (sem_mutex == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}
	
	printf("Mi ID %d\n",buzon);
	//--------------------------BUCLE DE ACCIONES DEL PROGRAMA-----------------------
	
	printf("Esperando por mensajes...\n");
	
	while (1) { 
	
		
		
		msgrcv(msqid, &datos_recibir, sizeof(datos_recibir.text)+3*sizeof(int),0, 0); 
		
		ticket_origen=datos_recibir.mi_ticket;
		id_nodo_origen=datos_recibir.mi_id;
		pid_origen=datos_recibir.mi_pid;
		
		if(datos_recibir.type!=buzon){msgsnd(msqid, &datos_recibir, sizeof(datos_recibir.text)+3*sizeof(int), 0); }
		else{
		printf("Me llegó un mensaje de %d con el ticket %i\n",pid_origen,ticket_origen);
		
		sem_wait(sem_mutex);
		
		datos->max_ticket = MAX(datos->max_ticket, ticket_origen);//compara su ticket con el ticket del que le llego 

		
		if ((!(datos->quiero) || ticket_origen < datos->mi_ticket|| (ticket_origen == datos->mi_ticket & (id_nodo_origen <datos->mi_id))) & datos->dentro==0){
		
		
				ack.id_nodo=buzon;
				ack.type=pid_origen;
				
				//SEMAFORO PASO !!!!!!!!!!!!!!!!!!
				
				printf("Envio OK a buzon %ld\n",ack.type);
			
		}else {
			 
			 datos->id_nodos_pend[datos->num_pend++]= pid_origen;
			 printf("Numero de pendientes: %d\n",datos->num_pend);
		
		}
		printf("Esperando por mensajes...\n");
		sem_post(sem_mutex);
		
		}}
	
}
