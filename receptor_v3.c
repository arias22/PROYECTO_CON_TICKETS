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
#include <signal.h>


int posicionv;
#define MAX(i,j) (((i)>(j)) ? (i) : (j))
#define SIZE 50

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
	int primero;
	int ultimo;
	int contador_paso_1;
	int contador_paso_2;
	int estado_anterior;
}datos_comp;

struct msg{
	long mtype;
	int mi_ticket;
	int mi_pid;
	int mi_id;
	char text[100];
	int id_nodo;
	int ack;
}mensaje;

struct msgbuf {
    long mtype;
    char mtext[SIZE];
};


int msqid2_glob;
int argv_nodo;
void handle_sigint(int signal) {


key_t clave1 = ftok(".",posicionv);
//BORRAR MEMORIA COMPARTIDA
 int shmid1 = shmget(clave1, sizeof(datos_comp), IPC_CREAT|0660);
  if (shmctl(shmid1, IPC_RMID, NULL) == -1) {
     perror("Error al borrar la memoria compartida");
      exit(1);
   }
//BORRAR BUZON MENSAJES
if (msgctl(msqid2_glob, IPC_RMID, NULL) == -1) {
        perror("msgctl");

    }
//BORRAR SEMÁFOROS

char name_mutex[50];
sprintf(name_mutex, "/MUTEX%d", argv_nodo);;  
if(sem_unlink(name_mutex)==-1) printf("NO SE DESTRUYO BIEN MUTEX\n");


char name_mutex_between_main[50];
sprintf(name_mutex_between_main, "/MUTEXMAIN%d", argv_nodo);
if(sem_unlink(name_mutex_between_main)==-1) printf("NO SE DESTRUYO BIEN MUTEXMAIN\n");


char name_mutex2[50];
sprintf(name_mutex2, "/MUTEX1%d",argv_nodo);
if(sem_unlink(name_mutex2)==-1) printf("NO SE DESTRUYO BIEN MUTEX1\n");


char name_mutex3[50];
sprintf(name_mutex3, "/MUTEX2%d", argv_nodo);
if(sem_unlink(name_mutex3)==-1) printf("NO SE DESTRUYO BIEN MUTEX2\n");

char name_paso[50];
sprintf(name_paso, "/MUTEXPASO%d", argv_nodo);
if(sem_unlink(name_paso)==-1) printf("NO SE DESTRUYO BIEN MUTEXPASO\n");

    exit(0);
}


int main(int argc,char *argv[]) {
struct sigaction ss;
ss.sa_handler = handle_sigint;
ss.sa_flags = 0;
sigaction(2,&ss,NULL);
	//-------------VARIABLE PROPIAS--------------------------
	
	int id_nodo_origen = 0, ticket_origen = 0, pid_origen=0;
	
	 if (argc != 3){
		printf("formato incorrecto: ./v1_receptor posicion N\n");
		exit(-1);
	}
	
	int posicion=atoi(argv[1]);
	argv_nodo = posicion;
	int buzon=1235+posicion;
	posicionv = posicion;
	int N = atoi(argv[2]);
	int ack = 0;

	
	//----------VARIABLES DE LA MEMORIA COMPARTIDA---------------
	key_t clave1; //clave de acceso a la memoria 1
	int shmid1; //identificador de la zona de memoria 1
	//-----------------------CREACION DE BUZONES DE MENSAJES-----------------------------------------------------------------
	int msqid = msgget(500,0666 | IPC_CREAT);
	msqid2_glob = msqid;
	struct msgbuf msgs;
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
	datos->primero=0;
	datos->ultimo=0;
	datos->contador_paso_1=0;
	datos->contador_paso_2=0;
	datos->estado_anterior=0;
		
	//---------------------------------------DECLARACION SEMÁFOROS---------------------
	char name_mutex[50];
	sprintf(name_mutex, "/MUTEX%s", argv[1]);
	sem_t *sem_mutex;
	sem_mutex = sem_open(name_mutex, O_CREAT, 0777, 1);
	if (sem_mutex == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}

	char name_mutex_between_main[50];
	sprintf(name_mutex_between_main, "/MUTEXMAIN%s", argv[1]);
	sem_t *sem_mutex_between_main;
	sem_mutex_between_main = sem_open(name_mutex_between_main, O_CREAT, 0777, 1);
	if (sem_mutex_between_main == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}
	char name_mutex2[50];
	sprintf(name_mutex2, "/MUTEX1%s", argv[1]);
	sem_t *sem_mutex2;
	sem_mutex2 = sem_open(name_mutex2, O_CREAT, 0777, 1);
	if (sem_mutex2 == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}
	char name_mutex3[50];
	sprintf(name_mutex3, "/MUTEX2%s", argv[1]);
	sem_t *sem_mutex3;
	sem_mutex3 = sem_open(name_mutex3, O_CREAT, 0777, 1);
	if (sem_mutex3 == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}	
	char name_paso[50];
	sprintf(name_paso, "/MUTEXPASO%s", argv[1]);
	sem_t *sem_name_paso;
	sem_name_paso = sem_open(name_paso, O_CREAT, 0777, 0);
	if (sem_name_paso == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}
	printf("Mi ID %d\n",buzon);
	//--------------------------BUCLE DE ACCIONES DEL PROGRAMA-----------------------
	
	printf("Esperando por mensajes...\n");
	
	while (1) { 
	
		
		//RECIBE LOS MENSAJE EN SU ID DE NODO

		
		msgrcv(msqid, &mensaje,sizeof(struct msg),buzon, 0); 
		printf("RECOJO MENSAJE\n");



		ticket_origen=mensaje.mi_ticket;
		id_nodo_origen=mensaje.mi_id;
		pid_origen=mensaje.mi_pid;

		
		
		if(mensaje.ack!=0){
			printf("Me llegó un mensaje de %d con el ticket %i\n",pid_origen,ticket_origen);
			
			sem_wait(sem_mutex);
			
			datos->max_ticket = MAX(datos->max_ticket, ticket_origen);//compara su ticket con el ticket del que le llego 

			
			if ((!(datos->quiero) || ticket_origen < datos->mi_ticket|| (ticket_origen == datos->mi_ticket & (id_nodo_origen <datos->mi_id))) & datos->dentro==0){
			
					mensaje.ack = 1;
					mensaje.id_nodo=buzon;
					mensaje.mtype=id_nodo_origen;
					
					msgsnd(msqid, &mensaje,sizeof(struct msg), 0);
					
					printf("Envio OK a buzon %ld\n",mensaje.mtype);
				
			}else {
				
				datos->id_nodos_pend[datos->num_pend++]= pid_origen;
				printf("Numero de pendientes: %d\n",datos->num_pend);
			
			}
			printf("Esperando por mensajes...\n");
			sem_post(sem_mutex);

		}else{
			printf("OK recibido de %d", id_nodo_origen);
			ack++;
			if(ack==datos->procesos){
				sem_post(sem_mutex_between_main);
			}
		}
		
		}
		}
	

