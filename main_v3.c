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
	int prioridad_request;
	int procesos;
	int primero;
	int cont_prioridades[4];
	int prioridad_procesos[100];
	int ack;
	int espera;
	int numero_consultas;
	int grifo;
}datos_comp;
	
struct msg{
	long mtype;                                          //TIPO DE MENSAJE
	int mi_ticket;										 //TICKET MENSAJE
	int mi_id;										     //ID NODO
	int tipo_mensaje;									 //TIPO DEL MENSAJE   '0' ===> REQUEST         '1' ======> ACK
	int prioridad;										 //PRIORIDAD MENSAJE 
}mensaje;





 void handle_sigint(int signal) {
	key_t clave1 = ftok(".",posicionv); //creamos la clave que utilizaremos para vincularnos a la zona de memoria ya creada
	 
	int shmid1 = shmget(clave1,8*sizeof(int),0);//Creación de zona_mem1, Al no poner el flag IPC_CREAT, estamos suponiendo que dicha memoria ya está creada.
	 	 
	datos_comp *datos =(datos_comp *)shmat(shmid1,0,0);//Vinculamos el proceso con la zona de memoria
	datos->procesos--;
    exit(0);
}

int main(int argc,char *argv[]) {

//--------------------------ctrl+c----------------------------


struct sigaction ss;
ss.sa_handler = handle_sigint;
ss.sa_flags = 0;
sigaction(2,&ss,NULL);


//--------------------------FIN ctrl+c----------------------------



	//----------VARIABLES PROPIAS-------------------------------
	int posicion=atoi(argv[1]);													//POSICION DEL NODO
	posicionv = posicion;														//"" ""
	int N = atoi(argv[2]);														//NUMERO NODOS
	int prioridad=atoi(argv[3]);												//PRIORIDAD DEL PROCESO
	int id_nodos=1235+posicion;													//ID DEL NODO	

	char array_prioridades[4][20] = {"Pagos&Anulaciones","Reservas", "Administración","Consultas"};
	//DE APOYO						
	int anteriores;
	int mayor=0;	
	int contadores = 0;
	int max_prioridad=0;
	

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
	
	// for(int i=0;i<N;i++){
	// 	datos->id_nodos_pend[i]=0;
	// }

	datos->num_pend=0;
	datos->max_ticket=0;


	datos->procesos++;
	mensaje.prioridad=prioridad;
	mensaje.mi_ticket=0;
	
	printf("Numero de procesos %d\n",datos->procesos);

	//-------------------FIN INICIALIZACION DE LAS VARIABLES COMPARTIDAS----------------------------------


	//-----------------------------INICIO PROGRAMA--------------------------------------------------------

	printf("SOY UN PROCESO DE TIPO %s DEL NODO %d\n",array_prioridades[prioridad],id_nodos);

	while (1) { 



		// Sección no crítica
		printf("Esperando para entrar en la Sección Critica\n");


		getchar();


	 	printf("Quiero entrar en la Sección Critica\n");
	 	
		

	 
		datos->quiero = datos->quiero +1;;													//INDICAS QUE QUIERES ENTRAR
		datos->espera++;														//INDICAS QIE ESTAS DENTRO
		datos->mi_ticket = datos->max_ticket + 1; 								// GENERACIÓN DE UN TICKET MAYOR AL ANTERIOR




		
		//SI LLEGA UN PROCESO EN EL MISMO NODO CON MAYOR PRIORIDAD DE CONSULTAS Y EL GRIFO ESTÁ ABIERTO CERRAMOS EL GRIFO

		if(prioridad <consultas && datos->grifo == 1){
			datos->grifo = 0;
		}
		



		//COMPRUEBAS SI HAY EN ALGUNA COLA UN PROCESO DE MAYOR DE PRIORIDAD Y LOS GUARDAS EN "max_prioridad"

		if(datos->cont_prioridades[pagos_anulaciones]!=0){
						max_prioridad=pagos_anulaciones;
			}else if(datos->cont_prioridades[reservas]!=0){
						max_prioridad=reservas;
			}else if(datos->cont_prioridades[administracion]!=0){
						max_prioridad=administracion;
			}else if (datos->cont_prioridades[consultas]!=0){
						max_prioridad=consultas;
		}


		
		//ENVIAS TU REQUEST SI SE CUMPLE EL IF


		if(datos->primero==0 || prioridad<max_prioridad){



			//ENVIAMOS REQUEST QUE SIRVE PARA CANCELAR EN EL CASO QUE HUBIERA QUE CANCELAR

			datos->ack=0;

			for (int i = 0; i <=N-1; i++){
				if(1235+i!=id_nodos){

					mensaje.mtype=1235+i;										//INDICA AL NODO  QUE ENVÍAS EL MENSAJE
					mensaje.mi_id=id_nodos;										//INIDICA TU NODO
					mensaje.prioridad=prioridad;								//INDICA TU PRIORIDAD
					mensaje.tipo_mensaje = 0;									//INDICA EL TIPO DE MENSAJE
					mensaje.mi_ticket=datos->mi_ticket;							//ENVIAS EL MENSAJE


					msgsnd(msqid, &mensaje, sizeof(struct msg), 0);
					datos->prioridad_request = prioridad;
					printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
				
				}
			}
			datos->primero=1;
			

		}


	//-----------------------------------------COLAS--------------------------------------------------

		


			//COLA PAGOS & ANULACIONES

			if(prioridad==pagos_anulaciones){
				printf("Semaforo pagos y anulaciones\n");
				datos->cont_prioridades[pagos_anulaciones] = datos->cont_prioridades[pagos_anulaciones] + 1 ;			//ACTUALIZAS NUMERO PROCESOS EN LA COLA
				
				sem_wait(sem_name_paso_pagos_anulaciones);
				
				datos->cont_prioridades[pagos_anulaciones] = datos->cont_prioridades[pagos_anulaciones] -1 ;			//ACTUALIZAS NUMERO PROCESOS EN LA COLA
				

			}
			
			//COLA RESERVAS 
			
			
			if(prioridad==reservas){
				printf("Semaforo reservas\n");
				datos->cont_prioridades[reservas] = datos->cont_prioridades[reservas] + 1 ;								//ACTUALIZAS NUMERO PROCESOS EN LA COLA

				sem_wait(sem_name_paso_reservas);

				datos->cont_prioridades[reservas] = datos->cont_prioridades[reservas] - 1 ;								//ACTUALIZAS NUMERO PROCESOS EN LA COLA
			}
			
			
			//COLA ADMINISTRACION
			
			
			if(prioridad==administracion){
				printf("Semaforo administración\n");
				datos->cont_prioridades[administracion] = datos->cont_prioridades[administracion] + 1 ;					//ACTUALIZAS NUMERO PROCESOS EN LA COLA

				sem_wait(sem_name_paso_administracion);

				datos->cont_prioridades[administracion] = datos->cont_prioridades[administracion] - 1 ;					//ACTUALIZAS NUMERO PROCESOS EN LA COLA
			}
			
			
			//COLA CONSULTAS 
			
			
			
			if(prioridad==consultas){
				if(datos->grifo != 1){
				printf("Semaforo consultas\n");
				datos->cont_prioridades[consultas] = datos->cont_prioridades[consultas] + 1 ;							//ACTUALIZAS NUMERO PROCESOS EN LA COLA

				sem_wait(sem_name_paso_consulta);

				datos->cont_prioridades[consultas] = datos->cont_prioridades[consultas] - 1 ;							//ACTUALIZAS NUMERO PROCESOS EN LA COLA

				}


				if(datos->grifo == 0) {
					datos->grifo = 1;
					datos -> grifo = 1;
				int x = datos->cont_prioridades[consultas];
				while(x!=0){
					sem_post(sem_name_paso_consulta);
					x--;
				}
				}
				datos->numero_consultas = datos->numero_consultas +1;



			}





		//-----------------------------------------FIN COLAS--------------------------------------------------


		

		
			


		printf("Tengo todos los permisos,entrando en la Sección Crítica...\n");
		
			datos->espera--;

		
		//ENTRADA  SECCIÓN CRÍTICA 





		printf("En la Seccion Crítica\n");
		
		
		
		datos->dentro = 1;
		
		getchar();
		
		printf("Saliendo de la sección crítica...\n");

		datos->numero_consultas = datos->numero_consultas -1;
		if(datos->numero_consultas == 0) datos->grifo = 0;



		
		//SALIDA SECCIÓN CRÍTICA 





		sem_wait(sem_mutex2);
		datos->quiero = datos->quiero -1;
		datos->dentro = 0;
	
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

		for (int i = 0; i <N-1; i++){
				if(max_prioridad>datos->prioridad_procesos[i] && datos->id_nodos_pend[i]!=0){
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

		
			for (int i = 0; i <N; i++){
					printf("EL NODO %d TIENE PRIORIDAD %d\n",datos->id_nodos_pend[i],datos->prioridad_procesos[i]);
									
					mensaje.mi_id=id_nodos;										//INIDICA TU NODO

					mensaje.tipo_mensaje = 1;									//INDICA EL TIPO DE MENSAJE

				    mensaje.mtype=datos->id_nodos_pend[i];						//INDICA AL NODO  QUE ENVÍAS EL MENSAJE

					mensaje.prioridad = datos->prioridad_procesos[i];			//ENVÍAS LA PRIORIDAD DEL REQUEST A QUIEN VA DIRIGIDO EL ACK

				if(datos->id_nodos_pend[i]!=0){
					 msgsnd(msqid, &mensaje,sizeof(struct msg), 0);
				
				    printf("[ACK] Enviando OK  a %d \n",datos->id_nodos_pend[i]);
				}

			}
				//BORRAMOS LOS ACK PENDIENTES

				for (int i = 0; i <N; i++){
						datos->id_nodos_pend[i] = 0;
						datos->prioridad_procesos[i] = 0;

				}


			



				mayor=0;	
				contadores = 0;
				
				printf("Todos los OK pendientes enviados\n");
			


				//ENCUENTRAS EL PROCESO EN ESPERA CON MAYOR PRIORIDAD Y HACES UN REQUEST POR ÉL
				//ACTUALIZO EL TICKET
				datos->mi_ticket = datos->max_ticket + 1;

				if(datos->cont_prioridades[pagos_anulaciones]!=0){
					for (int i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
							mensaje.mtype=1235+i;										//INDICA AL NODO  QUE ENVÍAS EL MENSAJE
							mensaje.mi_id=id_nodos;										//INIDICA TU NODO
							mensaje.prioridad=pagos_anulaciones;						//INDICA TU PRIORIDAD
							mensaje.tipo_mensaje = 0;									//INDICA EL TIPO DE MENSAJE
							mensaje.mi_ticket=datos->mi_ticket;							//ENVIAS EL MENSAJE


								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								datos->prioridad_request = pagos_anulaciones;
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}else if(datos->cont_prioridades[reservas]!=0){
					for (int i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.mtype=1235+i;										//INDICA AL NODO  QUE ENVÍAS EL MENSAJE
								mensaje.mi_id=id_nodos;										//INIDICA TU NODO
								mensaje.prioridad=reservas;									//INDICA TU PRIORIDAD
								mensaje.tipo_mensaje = 0;									//INDICA EL TIPO DE MENSAJE
								mensaje.mi_ticket=datos->mi_ticket;							//ENVIAS EL MENSAJE

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								datos->prioridad_request = reservas;
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}else if (datos->cont_prioridades[administracion]!=0){
					for (int i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.mtype=1235+i;										//INDICA AL NODO  QUE ENVÍAS EL MENSAJE
								mensaje.mi_id=id_nodos;										//INIDICA TU NODO
								mensaje.prioridad=administracion;							//INDICA TU PRIORIDAD
								mensaje.tipo_mensaje = 0;									//INDICA EL TIPO DE MENSAJE
								mensaje.mi_ticket=datos->mi_ticket;							//ENVIAS EL MENSAJE

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								datos->prioridad_request = administracion;
								printf("[REQUEST] enviado a %ld\n",mensaje.mtype);
					
							}
					}
	
				}else if(datos->cont_prioridades[consultas]!=0){
					for (int i = 0; i <=N-1; i++){
							if(1235+i!=id_nodos){
							
								mensaje.mtype=1235+i;										//INDICA AL NODO  QUE ENVÍAS EL MENSAJE
								mensaje.mi_id=id_nodos;										//INIDICA TU NODO
								mensaje.prioridad=consultas;						//INDICA TU PRIORIDAD
								mensaje.tipo_mensaje = 0;									//INDICA EL TIPO DE MENSAJE
								mensaje.mi_ticket=datos->mi_ticket;							//ENVIAS EL MENSAJE

								msgsnd(msqid, &mensaje, sizeof(struct msg), 0);//envias mensajes request a todos los nodos	
								datos->prioridad_request = consultas;
								printf("[[REQUEST] ]enviado a %ld\n",mensaje.mtype);
					
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
		
	


		for (int i = 0; i < 4; i++){
			if(datos->cont_prioridades[i]==0)  contadores++;
		}
		if(contadores==4){
	
			datos->primero=0;
			
		}
		contadores = 0;

		printf("Fuera de la Sección Critica\n");		
		
	}
	
}







