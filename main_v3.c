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
int posicionv;
#define SIZE 50
// -------------VARIABLES COMPARTIDAS--------------
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



 void handle_sigint(int signal) {
	key_t clave1 = ftok(".",posicionv); //creamos la clave que utilizaremos para vincularnos a la zona de memoria ya creada
	 
	int shmid1 = shmget(clave1,8*sizeof(int),0);//Creación de zona_mem1, Al no poner el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está creada.
	 
	if(shmid1 !=-1){
		printf("Zona de memoria vinculada OK! con identificador %d \n", shmid1);
	}else{
		printf("ERROR al crear zona de memoria 1 !! \n");
		exit(1);
	}
	 
	datos_comp *datos =(datos_comp *)shmat(shmid1,0,0);//Vinculamos el proceso con la zona de memoria
	datos->procesos--;
    exit(0);
}

pid_t getpid(void);
int main(int argc,char *argv[]) {
struct sigaction ss;
ss.sa_handler = handle_sigint;
ss.sa_flags = 0;
sigaction(2,&ss,NULL);

	//----------VARIABLES PROPIAS-------------------------------
	int i=0;
	int posicion=atoi(argv[1]);
	posicionv = posicion;
	int N = atoi(argv[2]);
	int id_nodos=1235+posicion;
	int buzon=getpid();

	char* cadena="Quiero entrar en la SC";
	strcpy(mensaje.text,cadena);
	
	 if (argc != 3){
		printf("formato incorrecto: ./v1_main posicion N\n");
		exit(-1);
	}

	
	//----------VARIABLES DE LA MEMORIA COMPARTIDA---------------
	key_t clave1; //clave de acceso a la memoria 1
	int shmid1; //identificador de la zona de memoria 1
		//-----------------------CREACION DE BUZONES DE MENSAJES-----------------------------------------------------------------
	int msqid = msgget(500,0666 | IPC_CREAT);
	struct msgbuf msgs;

	//-----------------------FIN DE CREACION DE BUZONES DE MENSAJES----------------------------------------------------------
	//-------------INICIALIZAMOS ZONA DE MEMORIA COMPARTIDA--------------

	clave1 = ftok(".",posicion); //creamos la clave que utilizaremos para vincularnos a la zona de memoria ya creada
	 
	shmid1 = shmget(clave1,8*sizeof(int),0);//Creación de zona_mem1, Al no poner el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está creada.
	 
	if(shmid1 !=-1){
		printf("Zona de memoria vinculada OK! con identificador %d \n", shmid1);
	}else{
		printf("ERROR al crear zona de memoria 1 !! \n");
		exit(1);
	}
	 
	datos_comp *datos =(datos_comp *)shmat(shmid1,0,0);//Vinculamos el proceso con la zona de memoria
	 
	if(datos == (datos_comp*)-1){
		printf("Error al vincular el proceso a la zona de memoria 1! \n");
		exit(1);
	} 

	//---------------------------------------DECLARACION SEMÁFOROS---------------------
	char name_mutex[50];
	sprintf(name_mutex, "/MUTEX%s", argv[1]);
	sem_t *sem_mutex;
	sem_mutex = sem_open(name_mutex, O_CREAT, 0777, 1);
	if (sem_mutex == SEM_FAILED) {
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
	char name_mutex_between_main[50];
	sprintf(name_mutex_between_main, "/MUTEXMAIN%s", argv[1]);
	sem_t *sem_mutex_between_main;
	sem_mutex_between_main = sem_open(name_mutex_between_main, O_CREAT, 0777, 0);
	if (sem_mutex_between_main == SEM_FAILED) {
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
		
	//-------------------INICIALIZACIÓN DE LAS VARIABLES COMPARTIDAS--------------------
	datos->mi_id=id_nodos;
	datos->mi_ticket=0;
	datos->quiero=0;
	for(int i=0;i<N;i++){
		datos->id_nodos_pend[i]=0;
	}
	datos->num_pend=0;
	datos->max_ticket=0;
	datos->procesos++;
	mensaje.ack=0;

	mensaje.mi_ticket=0;
	
	printf("Mi PID %d\n",getpid());
	printf("Numero de procesos %d\n",datos->procesos);

	//--------------------------BUCLE DE ACCIONES DEL PROGRAMA-----------------------
	while (1) { 
		// Sección no crítica
		printf("Esperando para entrar en la Sección Critica\n");
		getchar();
		// Sección crítica
	 	printf("Quiero entrar en la Sección Critica\n");
	 	


	 	sem_wait(sem_mutex);
		datos->quiero = 1;
		//datos->dentro++; 
		datos->mi_ticket = datos->max_ticket + 1; // generas un ticket mayor que el ultimo 
		sem_post(sem_mutex);
		
		mensaje.mi_ticket=datos->mi_ticket;
		mensaje.mi_pid=getpid();
		mensaje.mi_id=id_nodos;

		

		if(datos->primero==0 || datos->num_pend!=datos->estado_anterior){

				//ENVIA REQUEST

			
	
			for (i = 0; i <=N-1; i++){
				if(1235+i!=id_nodos){
				
					mensaje.ack = 0;
					mensaje.mtype=1235+i;

					mensaje.mtype = 1235+i;

					msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
					
					printf("Mensaje enviado a %ld\n",mensaje.mtype);
				}
			}

			datos->estado_anterior=datos->num_pend;
			printf("Numero estado pendientes: %d\n",datos->estado_anterior);

			if(datos->primero==0){
			datos->primero=1;
			datos->contador_paso_1++;
			printf("Entro semáforo paso 1\n");
			sem_wait(sem_mutex_between_main);
			datos->contador_paso_1--;
			}else{
			datos->ultimo=getpid();
			datos->contador_paso_2++;
			printf("Me marco como ultimo\n");
			printf("Entro semáforo paso 2\n");
			sem_wait(sem_name_paso);
			datos->contador_paso_2--;
			}

		}else{
			datos->contador_paso_2++;
			printf("Entro semáforo paso 2\n");
			sem_wait(sem_name_paso);
			datos->contador_paso_2--;
		}
		
		if(datos->ultimo==getpid()){
			datos->ultimo=0;
			
			for (i = 0; i <datos->num_pend; i++){
				mensaje.ack = 1;
				mensaje.mi_pid=getpid();
				mensaje.mtype=datos->id_nodos_pend[i];
					
				msgsnd(msqid, &mensaje,sizeof(struct msg), 0);
				
				printf("Enviando OK pendiente a %d \n",datos->id_nodos_pend[i]);
			}
		
			datos->num_pend = 0; 
			
			printf("Todos los OK pendientes enviados\n");
			

			sem_wait(sem_mutex_between_main);

		}
		
		sem_wait(sem_mutex3);
		datos->dentro++;
		sem_post(sem_mutex3);
		
		printf("Tengo todos los permisos,entrando en la Sección Crítica...\n");
		
		
		//SECCIÓN CRÍTICA; 

		printf("En la Seccion Crítica\n");
		//printf("Dentro %d\n",datos->dentro);
		
		getchar();
		
		printf("Saliendo de la sección crítica...\n");
		
		sem_wait(sem_mutex2);
		datos->quiero=0;
		datos->dentro--;
		sem_post(sem_mutex2);

		//sleep(1);
		printf("Contador cola 1 %d, contador cola 2 %d\n",datos->contador_paso_1,datos->contador_paso_2);
		if(datos->contador_paso_1==0 & datos->contador_paso_2==0){
			datos->primero=0;
		}

		if(datos->contador_paso_2!=0){
			sem_post(sem_name_paso);
		}
			if(datos->ultimo==getpid() || datos->contador_paso_1==0 & datos->contador_paso_2==0){
			datos->ultimo=0;
			
			for (i = 0; i <datos->num_pend; i++){
				mensaje.ack = 1;
				mensaje.mi_pid=getpid();
				mensaje.mtype=datos->id_nodos_pend[i];
					
				msgsnd(msqid, &mensaje,sizeof(struct msg), 0);
				
				printf("Enviando OK pendiente a %d \n",datos->id_nodos_pend[i]);
			}
		
			datos->num_pend = 0; 
			
			printf("Todos los OK pendientes enviados\n");
			

			//sem_wait(sem_mutex_between_main);

		}
		
		
		printf("Fuera de la Sección Critica\n");		
		
	
	 
}
}







