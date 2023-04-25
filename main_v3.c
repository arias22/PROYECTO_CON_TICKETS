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
#define pagos_anulaciones 0
#define reservas 1
#define administracion 2
#define consultas 3

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
	int id_pid_pend[100];
	int cont_prioridades[4];
	int prioridad_procesos[100];
	int ack;
	int consultas_activas;
	int espera;
}datos_comp;
	
struct msg{
	long mtype;
	int mi_ticket;
	int mi_pid;
	int mi_id;
	char text[100];
	int id_nodo;
	int ack;
	int prioridad;
	int cancelar;
	int consulta;
	int fin_consultas;
}mensaje;





 void handle_sigint(int signal) {
	key_t clave1 = ftok(".",posicionv); //creamos la clave que utilizaremos para vincularnos a la zona de memoria ya creada
	 
	int shmid1 = shmget(clave1,8*sizeof(int),0);//Creación de zona_mem1, Al no poner el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está creada.
	 	 
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
	int prioridad=atoi(argv[3]);
	int id_nodos=1235+posicion;
	int buzon=getpid();
	int anteriores;
	int mayor=0;
	int contadores = 0;
	int max_prioridad=0;
	char nombres_prioridades[][30]={"PAGOS/ANULACIONES","RESERVAS","ADMINISTRACION","CONSULTAS"};
	
	 if (argc != 4){
		printf("formato incorrecto: ./v1_main posicion N prioridad\n");
		exit(-1);
	}

	

	//-----------------------CREACION DE BUZONES DE MENSAJES-----------------------------------------------------------------

	int msqid = msgget(500,0666 | IPC_CREAT);

	//-----------------------FIN DE CREACION DE BUZONES DE MENSAJES----------------------------------------------------------




	//-------------INICIALIZAMOS ZONA DE MEMORIA COMPARTIDA-------------------------------------------------------------------

	key_t clave1; 
	int shmid1; 
	clave1 = ftok(".",posicion); 
	 
	shmid1 = shmget(clave1,8*sizeof(int),0);
	 
	if(shmid1 !=-1){
		printf("Zona de memoria vinculada OK! con identificador %d \n", shmid1);
	}else{
		printf("ERROR al crear zona de memoria 1 !! \n");
		exit(1);
	}
	 
	datos_comp *datos =(datos_comp *)shmat(shmid1,0,0);
	 
	if(datos == (datos_comp*)-1){
		printf("Error al vincular el proceso a la zona de memoria 1! \n");
		exit(1);
	} 

	//-------------FIN INICIALIZAMOS ZONA DE MEMORIA COMPARTIDA-----------------------------------------------------------------




	//---------------------------------------DECLARACION SEMÁFOROS--------------------------------------------------------------

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


	char name_paso[50];
	sprintf(name_paso, "/MUTEXPASO%s", argv[1]);
	sem_t *sem_name_paso;
	sem_name_paso = sem_open(name_paso, O_CREAT, 0777, 0);
	if (sem_name_paso == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}

		char name_paso_consulta[50];
	sprintf(name_paso_consulta, "/MUTEXPASOCONSULTA%s", argv[1]);
	sem_t *sem_name_paso_consulta;
	sem_name_paso_consulta = sem_open(name_paso_consulta, O_CREAT, 0777, 0);
	if (sem_name_paso_consulta == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}

		char name_paso_administracion[50];
	sprintf(name_paso_administracion, "/MUTEXPASOADMINISTRACION%s", argv[1]);
	sem_t *sem_name_paso_administracion;
	sem_name_paso_administracion = sem_open(name_paso_administracion, O_CREAT, 0777, 0);
	if (sem_name_paso_administracion == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}

		char name_paso_reservas[50];
	sprintf(name_paso_reservas, "/MUTEXPASORESERVAS%s", argv[1]);
	sem_t *sem_name_paso_reservas;
	sem_name_paso_reservas = sem_open(name_paso_reservas, O_CREAT, 0777, 0);
	if (sem_name_paso_reservas == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}

		char name_paso_pagos_anulaciones[50];
	sprintf(name_paso_pagos_anulaciones, "/MUTEXPASOPAGOSANULACIONES%s", argv[1]);
	sem_t *sem_name_paso_pagos_anulaciones;
	sem_name_paso_pagos_anulaciones = sem_open(name_paso_pagos_anulaciones, O_CREAT, 0777, 0);
	if (sem_name_paso_pagos_anulaciones == SEM_FAILED) {
	     perror("Failed to open semphore for empty");
	     exit(-1);
	}

	//---------------------------------------DECLARACION SEMÁFOROS----------------------------------------






	//-------------------INICIALIZACIÓN DE LAS VARIABLES COMPARTIDAS-------------------------------------

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
	mensaje.prioridad=prioridad;

	mensaje.mi_ticket=0;
	
	printf("Mi PID %d\n",getpid());
	printf("Numero de procesos %d\n",datos->procesos);
	printf("PROCESO %s\n",nombres_prioridades[prioridad]);

	//-------------------FIN INICIALIZACION DE LAS VARIABLES COMPARTIDAS----------------------------------


	//-----------------------------INICIO PROGRAMA--------------------------------------------------------



	while (1) { 
		// Sección no crítica
		printf("Esperando para entrar en la Sección Critica\n");
		getchar();
		// Sección crítica
	 	printf("Quiero entrar en la Sección Critica\n");

	 	sem_wait(sem_mutex);
		datos->quiero = 1;
		datos->espera++;
		datos->mi_ticket = datos->max_ticket + 1;								// GENERACIÓN DE UN TICKET MAYOR AL ANTERIOR
		sem_post(sem_mutex);
		
		mensaje.mi_ticket=datos->mi_ticket;
		printf("ticket origen %d\n",mensaje.mi_ticket);
		mensaje.mi_pid=getpid();
		mensaje.mi_id=id_nodos;
		
		if(datos->consultas_activas==1 && prioridad!=consultas){
				datos->consultas_activas=0;
				for (i = 0; i <=N-1; i++){
					if(1235+i!=id_nodos){
					
						mensaje.ack = 0;
						mensaje.cancelar=0;
						mensaje.fin_consultas=1;
						mensaje.consulta=0;
						mensaje.mtype=1235+i;
						mensaje.mi_id=id_nodos;
						mensaje.prioridad=prioridad;
						mensaje.mi_pid=getpid();
						

						msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
						
						printf("[FIN CONSULTAS] enviado a %ld\n",mensaje.mtype);
					
					}
				}

				for (i = 0; i <=N-1; i++){

							if(1235+i!=id_nodos){
							printf("AQJI\n");
							mensaje.ack = 0;
							mensaje.cancelar=0;
							mensaje.fin_consultas=0;
							mensaje.consulta=0;
								mensaje.mtype=1235+i;
								mensaje.mi_pid=getpid();
								mensaje.prioridad=prioridad;
								

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}

				datos->primero=1;
		}



		//COMPROBAMOS SI EN MI NODO SI HA HABIDO ALGUN REQUEST DE UN PROCESO DE MENOR PRIORIDAD QUE YO
					//SI ES AFIRMATIVO ENTONCES CANCELO EL REQUEST ANTERIOR Y ENVIO EL MÍO
					//LAS PRIORIDADES VAN AL REVÉS (1 MÁS PRIORITARIO QUE 5)

		if(datos->cont_prioridades[pagos_anulaciones]!=0){
						max_prioridad=pagos_anulaciones;
			}else if(datos->cont_prioridades[reservas]!=0){
						max_prioridad=reservas;
			}else if(datos->cont_prioridades[administracion]!=0){
						max_prioridad=administracion;
			}else if (datos->cont_prioridades[consultas]!=0){
						max_prioridad=consultas;
		}
		
		if (max_prioridad>prioridad){
				max_prioridad=1;
		} else max_prioridad=0;


		// IF PARA ENVIAR REQUEST 


		if(datos->consultas_activas==0){
				if((datos->primero==0 || max_prioridad==1) && datos->dentro==0){

								
				//IF PARA ENVIAR CANCELACIONES

				if(datos->primero!=0  && max_prioridad==1){
					for (i = 0; i <=N-1; i++){
						if(1235+i!=id_nodos){
						
							mensaje.ack = 0;
							mensaje.cancelar = 1;
							mensaje.mtype=1235+i;
							mensaje.mi_pid=getpid();

							msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
							
							printf("[CANCELACIÓN] enviado a %ld\n",mensaje.mtype);
						
						}
					}
					datos->ack=0;
				}

				//ENVIAMOS REQUEST 



				for (i = 0; i <=N-1; i++){
					if(1235+i!=id_nodos){
					
						mensaje.ack = 0;
						mensaje.cancelar=0;
						mensaje.mtype=1235+i;
						mensaje.mi_id=id_nodos;
						mensaje.prioridad=prioridad;
						mensaje.mi_pid=getpid();
						

						msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
						
						printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
					}
				}
				datos->primero=1;
				mayor=0;
			}
			
			


			//BUCLE EN EL QUE ESPERAMOS EN LA COLA QUE NOS CORRESPONDE 

			while(1){


				//COLA PAGOS & ANULACIONES

				if(prioridad==pagos_anulaciones){
					printf("Semaforo pagos y anulaciones\n");
					datos->cont_prioridades[pagos_anulaciones] = datos->cont_prioridades[pagos_anulaciones] + 1 ;
					mayor=0;
					sem_wait(sem_name_paso_pagos_anulaciones);
					datos->cont_prioridades[pagos_anulaciones] = datos->cont_prioridades[pagos_anulaciones] -1 ;
					

				}
				
				//COLA RESERVAS 
				
				
				else if(prioridad==reservas){
					printf("Semaforo reservas\n");
					datos->cont_prioridades[reservas] = datos->cont_prioridades[reservas] + 1 ;
					mayor=0;
					sem_wait(sem_name_paso_reservas);
					printf("AQUI\n");
					datos->cont_prioridades[reservas] = datos->cont_prioridades[reservas] - 1 ;
				}
				
				
				//COLA ADMINISTRACION
				
				
				else if(prioridad==administracion){
					printf("Semaforo administración\n");
					datos->cont_prioridades[administracion] = datos->cont_prioridades[administracion] + 1 ;
					mayor=0;
					sem_wait(sem_name_paso_administracion);
					printf("AQUI\n");
					datos->cont_prioridades[administracion] = datos->cont_prioridades[administracion] - 1 ;
				}
				
				
				//COLA CONSULTAS 
				
				
				
				else{
						printf("Semaforo consultas\n");
						datos->cont_prioridades[consultas] = datos->cont_prioridades[consultas] + 1 ;
						mayor=0;
						sem_wait(sem_name_paso_consulta);
						printf("AQUI\n");
						datos->cont_prioridades[consultas] = datos->cont_prioridades[consultas] - 1 ;
						
				}



				//VEMOS SI HAY UN PROCESO DE MAYOR PRIORIDAD PENDIENTE DE ATENDER 

				for (i = 0; i <datos->num_pend; i++){
						if(datos->prioridad_procesos[i]<prioridad){
							mayor=1;
							break;
						}
				}

				//SI LO HAY ENVIAMOS ACK A TODOS LOS PENDIENTES CANCELAMOS LA REQUEST ACTUAL Y ENVIAMOS OTRO REQUEST

				if(mayor==1){
						


					//ENVÍO ACKS

					for (i = 0; i <datos->num_pend; i++){
						mensaje.ack = 1;
						mensaje.mtype=datos->id_nodos_pend[i];
						if(datos->id_nodos_pend[i]!=0) msgsnd(msqid, &mensaje,sizeof(struct msg), 0);
						
						printf("[ACK] Enviando OK  a %d \n",datos->id_nodos_pend[i]);
					}


					datos->num_pend = 0; 
					mayor=0;	
					contadores = 0;
					printf("Todos los OK pendientes enviados\n");

					//ENVIO PETICION CANCELACIÓN

					for (i = 0; i <=N-1; i++){
						if(1235+i!=id_nodos){
							
							mensaje.ack = 0;
							mensaje.cancelar = 1;
							mensaje.mtype=1235+i;
							mensaje.mi_pid=getpid();

							msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
							printf("[CANCELACIÓN] enviado a %ld\n",mensaje.mtype);
							
							}
					}
					
					//ENVIO NUEVO REQUEST

					for (i = 0; i <=N-1; i++){
						if(1235+i!=id_nodos){
							
							mensaje.ack = 0;
							mensaje.cancelar=0;
							mensaje.mtype=1235+i;
							mensaje.mi_id=id_nodos;
							mensaje.prioridad=prioridad;
							mensaje.mi_pid=getpid();
								

							msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
							printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
							
							}
					}
				}else{break;}	
			}



			printf("Tengo todos los permisos,entrando en la Sección Crítica...\n");
		

		}
		
		//ENTRADA  SECCIÓN CRÍTICA 
		datos->dentro++;
		if(prioridad==consultas && datos->consultas_activas==0){
			datos->consultas_activas=1;
			for (i = 0; i <=N-1; i++){
							
							
								mensaje.ack = 0;
								mensaje.cancelar = 0;
								mensaje.consulta=1;
								mensaje.mtype=1235+i;
								mensaje.mi_pid=getpid();
								mensaje.prioridad=consultas;
								mensaje.mtype = 1235+i;

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
								printf("[CONSULTAS] enviado a %ld\n",mensaje.mtype);
					
							
					}
		}
		printf("En la Seccion Crítica\n");
		//printf("Dentro %d\n",datos->dentro);
		
		getchar();
		
		printf("Saliendo de la sección crítica...\n");
		
		//SALIDA SECCIÓN CRÍTICA 





		sem_wait(sem_mutex2);
		datos->quiero=0;
		datos->dentro--;
		datos->espera--;
		sem_post(sem_mutex2);
		

		//COMPARAMOS PRIORIDADES DE PROCESOS DE OTROS NODOS CON LOS NUESTROS QUE ESTÁN ESPERANDO
						//CASO TENEMOS ALGUNO DE PRIORIDAD MAYOR ESPERANDO  ======> 
						//CASO HAY PROCESOS MÁS PRIORITARIOS EN OTROS NODOS ======>



		//ASIGNAMOS A max_prioridad EL VALOR DE LA PRIORIDAD MÁS ALTA ENTRES LOS PROCESOS DE LAS COLAS 

		if(datos->cont_prioridades[pagos_anulaciones]!=0){
						max_prioridad=pagos_anulaciones;
			}else if(datos->cont_prioridades[reservas]!=0){
						max_prioridad=reservas;
			}else if(datos->cont_prioridades[administracion]!=0){
						max_prioridad=administracion;
			}else if (datos->cont_prioridades[consultas]!=0){
						max_prioridad=consultas;
		}
		


		//SI ENCONTRAMOS UN PROCESO DE MAYOR PRIORIDAD ENTRE LAS PENDIENTES MARCAMOS mayor COMO 1

		for (i = 0; i <datos->num_pend; i++){
				if(max_prioridad>datos->prioridad_procesos[i]){
					mayor=1;
					break;
				}
		}

		//SIN contadores LLEGA A 4 ESO QUIERE DECIR QUE TODAS LAS COLAS ESTÁN VACÍAS

		for (int i = 0; i < 4; i++){
			if(datos->cont_prioridades[i]==0)  contadores++;			
		}
		

		//SI O BIEN LAS COLAS ESTÁN VACÍA O ENCONTRAMOS EN OTRO NODO OTRO PROCESO DE PRIORIDAD MAYOR  EJECUTAMOS EL IF

		if(mayor==1 || contadores==4 ){
				
			//ENVIAMOS ACK PENDIENTES
		
			for (i = 0; i <datos->num_pend; i++){
				mensaje.ack = 1;
				mensaje.mtype=datos->id_nodos_pend[i];
				if(datos->id_nodos_pend[i]!=0) msgsnd(msqid, &mensaje,sizeof(struct msg), 0);
				
				printf("[ACK] Enviando OK  a %d \n",datos->id_nodos_pend[i]);
			}




				datos->num_pend = 0; 
				mayor=0;	
				contadores = 0;
				printf("Todos los OK pendientes enviados\n");
			


				//ENCUENTRAS EL PROCESO EN ESPERA CON MAYOR PRIORIDAD Y HACES UN REQUEST POR ÉL

				if(datos->cont_prioridades[pagos_anulaciones]!=0){
					for (i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.ack = 0;
								mensaje.cancelar = 0;
								mensaje.mtype=1235+i;
								mensaje.mi_pid=getpid();
								mensaje.prioridad=pagos_anulaciones;
								mensaje.mtype = 1235+i;

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}else if(datos->cont_prioridades[reservas]!=0){
					for (i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.ack = 0;
								mensaje.cancelar = 0;
								mensaje.mtype=1235+i;
								mensaje.mi_pid=getpid();
								mensaje.prioridad=reservas;
								mensaje.mtype = 1235+i;

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}else if (datos->cont_prioridades[administracion]!=0){
					for (i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.ack = 0;
								mensaje.cancelar = 0;
								mensaje.mtype=1235+i;
								mensaje.mi_pid=getpid();
								mensaje.mtype = 1235+i;
								mensaje.prioridad=administracion;

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}else if(datos->cont_prioridades[consultas]!=0){
					for (i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.ack = 0;
								mensaje.cancelar = 0;
								mensaje.mtype=1235+i;
								mensaje.mi_pid=getpid();
								mensaje.mtype = 1235+i;
								mensaje.prioridad=consultas;

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								
								printf("[REQUEST]enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}
		}else{

			//EN CASO CONTRARIO DESPIERTAS AL PROCESO DE MAYOR PRIORIDAD 


			if(datos->cont_prioridades[pagos_anulaciones]!=0){
						sem_post(sem_name_paso_pagos_anulaciones);
			}else if(datos->cont_prioridades[reservas]!=0){
						sem_post(sem_name_paso_reservas);
			}else if(datos->cont_prioridades[administracion]!=0){
						sem_post(sem_name_paso_administracion);
			}else if (datos->cont_prioridades[consultas]!=0){
						sem_post(sem_name_paso_consulta);
			}

		}
		
	

		//ARREGLAR !!!!!!!!!!!!!!!!!!!!!

		for (int i = 0; i < 4; i++){
			if(datos->cont_prioridades[i]==0)  contadores++;
		}
		if(contadores==4){
	
			datos->primero=0;
			contadores = 0;
		}
		
		printf("Fuera de la Sección Critica\n");		
		
	}
	
}







