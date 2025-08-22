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


int suma_rcv=0;
int posicionv;
#define MAX(i, j) (((i) > (j)) ? (i) : (j))
#define pagos_anulaciones 1
#define reservas 2
#define administracion 3
#define consultas 4

// --------- VARIABLES COMPARTIDAS----------
typedef struct datos_comp{
	int mi_ticket;
	int mi_id;
	int id_nodos_pend[100];
	int quiero;
	int max_ticket;
	//añadidas
	int dentro;
	int prioridad_request;
	int procesos;
	int primero;
	int cont_prioridades[10];
	int prioridad_procesos[100];
	int tickets_procesos[100];
	int ack;
	int numero_consultas;
	int grifo;
	double tiempos[100];
	int tiempos_prio[100];
	int vuelta_n;
}datos_comp;

struct msg
{
	long mtype;		  // TIPO DE MENSAJE
	int mi_ticket;	  // TICKET MENSAJE
	int mi_id;		  // ID NODO
	int tipo_mensaje; // TIPO DEL MENSAJE   '0' ===> REQUEST         '1' ======> ACK
	int prioridad;	  // PRIORIDAD MENSAJE
} mensaje;

int msqid2_glob;
void handle_sigint(int signal)
{

	printf("posicion al borrar es = %d\n", posicionv);
	key_t clave1 = ftok(".", posicionv);
	// BORRAR MEMORIA COMPARTIDA
	int shmid1 = shmget(clave1, sizeof(datos_comp), IPC_CREAT | 0660);

	datos_comp *datos = (datos_comp *)shmat(shmid1, 0, 0);

	char current_dir[50];
	getcwd(current_dir, sizeof(current_dir));
		FILE *archivo;
		FILE *archivo2;
		FILE *archivo3;
	char name_fichero[100];
	char name_fichero2[100];
	char name_fichero3[100];
	sprintf(name_fichero, "%s/mis_archivos/datos_p%d.txt", current_dir, posicionv);
	sprintf(name_fichero3, "%s/mis_archivos/datos_n%d.txt", current_dir, posicionv);
	sprintf(name_fichero2, "%s/mis_archivos/datos_t%d.txt", current_dir, posicionv);
	archivo = fopen(name_fichero, "a"); // Abre el archivo para escribir
	archivo2 = fopen(name_fichero2, "a"); // Abre el archivo para escribir
	archivo3 = fopen(name_fichero3, "a"); // Abre el archivo para escribir
	fprintf(archivo, "\n\n");
	fprintf(archivo2, "\n\n");
	fprintf(archivo3, "\n\n");
	fprintf(archivo3, "El numero de mensaje es %d\n",suma_rcv);
	for(int i =0;datos->tiempos_prio[i]!=0;i++){
	fprintf(archivo, "%d\n",datos->tiempos_prio[i]);
	}
	for(int i =0;datos->tiempos_prio[i]!=0;i++){
	fprintf(archivo2, "%f\n",datos->tiempos[i]);
	}
	fclose(archivo);
	fclose(archivo2);
	fclose(archivo3);
	
	if (shmctl(shmid1, IPC_RMID, NULL) == -1)
	{
		perror("Error al borrar la memoria compartida");
		exit(1);
	}
	// BORRAR BUZON MENSAJES
	if (msgctl(msqid2_glob, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
	}
	// BORRAR SEMÁFOROS

	char name_mutex[50];
	sprintf(name_mutex, "/MUTEX%d", posicionv);
	if (sem_unlink(name_mutex) == -1)
		perror("Name_mutex"); // printf("NO SE DESTRUYO BIEN MUTEX\n");

	char name_mutex2[50];
	sprintf(name_mutex2, "/MUTEX1%d", posicionv);
	if (sem_unlink(name_mutex2) == -1)
		printf("NO SE DESTRUYO BIEN MUTEX1\n");

	char name_paso[50];
	sprintf(name_paso, "/MUTEXPASO%d", posicionv);
	if (sem_unlink(name_paso) == -1)
		printf("NO SE DESTRUYO BIEN MUTEXPASO\n");

	char name_paso_consulta[50];
	sprintf(name_paso_consulta, "/MUTEXPASOCONSULTA%d", posicionv);
	if (sem_unlink(name_paso_consulta) == -1)
		printf("NO SE DESTRUYO BIEN MUTEXPASOCONSULTA\n");

	char name_paso_administracion[50];
	sprintf(name_paso_administracion, "/MUTEXPASOADMINISTRACION%d", posicionv);
	if (sem_unlink(name_paso_administracion) == -1)
		printf("NO SE DESTRUYO BIEN MUTEXPASOADMINISTRACION\n");

	char name_paso_reservas[50];
	sprintf(name_paso_reservas, "/MUTEXPASORESERVAS%d", posicionv);
	if (sem_unlink(name_paso_reservas) == -1)
		printf("NO SE DESTRUYO BIEN MUTEXPASORESERVAS\n");

	char name_paso_pagos_anulaciones[50];
	sprintf(name_paso_pagos_anulaciones, "/MUTEXPASOPAGOSANULACIONES%d", posicionv);
	if (sem_unlink(name_paso_pagos_anulaciones) == -1)
		printf("NO SE DESTRUYO BIEN MUTEXPASOPAGOSANULACIONES\n");

	char name_var_mi_ticket[50];
	sprintf(name_var_mi_ticket, "/name_var_mi_ticket%d", posicionv);
	if (sem_unlink(name_var_mi_ticket) == -1)
		printf("NO SE DESTRUYO BIEN name_var_mi_ticket\n");

	char name_var_mi_id[50];
	sprintf(name_var_mi_id, "/name_var_mi_id%d", posicionv);
	if (sem_unlink(name_var_mi_id) == -1)
		printf("NO SE DESTRUYO BIEN name_var_mi_id\n");


	char name_var_id_nodos_pend[50];
	sprintf(name_var_id_nodos_pend, "/name_var_id_nodos_pend%d", posicionv);
	if (sem_unlink(name_var_id_nodos_pend) == -1)
		printf("NO SE DESTRUYO BIEN MUTEXPASOPAGOSANULACIONES\n");

	char name_var_quiero[50];
	sprintf(name_var_quiero, "/name_var_quiero%d", posicionv);
	if (sem_unlink(name_var_quiero) == -1)
		printf("NO SE DESTRUYO BIEN name_var_mi_ticket\n");

	char name_var_dentro[50];
	sprintf(name_var_dentro, "/name_var_dentro%d", posicionv);
	if (sem_unlink(name_var_dentro) == -1)
		printf("NO SE DESTRUYO BIEN name_var_dentro\n");


	char name_var_prioridad_request[50];
	sprintf(name_var_prioridad_request, "/name_var_prioridad_request%d", posicionv);
	if (sem_unlink(name_var_prioridad_request) == -1)
		printf("NO SE DESTRUYO BIEN name_var_prioridad_request\n");


	char name_var_procesos[50];
	sprintf(name_var_procesos, "/name_var_procesos%d", posicionv);
	if (sem_unlink(name_var_procesos) == -1)
		printf("NO SE DESTRUYO BIEN name_var_procesos\n");


	char name_var_primero[50];
	sprintf(name_var_primero, "/name_var_primero%d", posicionv);
	if (sem_unlink(name_var_primero) == -1)
		printf("NO SE DESTRUYO BIEN name_var_primero\n");



	char name_var_cont_prioridades[50];
	sprintf(name_var_cont_prioridades, "/name_var_cont_prioridades%d", posicionv);
	if (sem_unlink(name_var_cont_prioridades) == -1)
		printf("NO SE DESTRUYO BIEN name_var_cont_prioridades\n");

	char name_var_prioridad_procesos[50];
	sprintf(name_var_prioridad_procesos, "/name_var_prioridad_procesos%d", posicionv);
	if (sem_unlink(name_var_prioridad_procesos) == -1)
		printf("NO SE DESTRUYO BIEN name_var_prioridad_procesos\n");


	char name_var_tickets_procesos[50];
	sprintf(name_var_tickets_procesos, "/name_var_tickets_procesos%d", posicionv);
	if (sem_unlink(name_var_tickets_procesos) == -1)
		printf("NO SE DESTRUYO BIEN name_var_tickets_procesos\n");

	char name_var_ack[50];
	sprintf(name_var_ack, "/name_var_ack%d", posicionv);
	if (sem_unlink(name_var_ack) == -1)
		printf("NO SE DESTRUYO BIEN name_var_ack\n");


		char name_var_numero_consultas[50];
	sprintf(name_var_numero_consultas, "/name_var_numero_consultas%d", posicionv);
	if (sem_unlink(name_var_numero_consultas) == -1)
		printf("NO SE DESTRUYO BIEN name_var_numero_consultas\n");

	char name_var_grifo[50];
	sprintf(name_var_grifo, "/name_var_grifo%d", posicionv);
	if (sem_unlink(name_var_grifo) == -1)
		printf("NO SE DESTRUYO BIEN name_var_grifo\n");

	char char_nutex_request[50];
	sprintf(char_nutex_request, "/char_nutex_request%d",posicionv);
	if (sem_unlink(char_nutex_request) == -1)
		printf("NO SE DESTRUYO BIEN char_nutex_request\n");
	



	exit(0);

	
}

int main(int argc, char *argv[])
{

	


	struct sigaction ss;
	ss.sa_handler = handle_sigint;
	ss.sa_flags = 0;
	sigaction(1, &ss, NULL);

	//-------------VARIABLE PROPIAS--------------------------

	int id_nodo_origen = 0, ticket_origen = 0;

	if (argc != 3)
	{
		printf("formato incorrecto: ./v1_receptor posicion N\n");
		exit(-1);
	}

	int posicion = atoi(argv[1]); // POSICION DEL NODO
	posicionv = posicion;
	int buzon = 1235 + posicion; // ID DEL NODO
	int N = atoi(argv[2]);		 // NUMERO DE NODOS

	// VARIABLES DE APOYO

	int ack_procesos[255] = {};
	int ack = 0;
	int max_prioridad = 0;
	int prioridad_mensaje;
	

	//-------------FIN VARIABLE PROPIAS--------------------------

	//-----------------------CREACION DE BUZONES DE MENSAJES-----------------------------------------------------------------
	struct msqid_ds info;
	int msqid = msgget(500, 0666 | IPC_CREAT);
	msqid2_glob = msqid;

	
	if(posicion == 0){ 
	if (msgctl(msqid, IPC_STAT, &info) == -1) 
	{ perror("msgctl"); exit(1); } 
	info.msg_qbytes = (info.msg_qbytes+(1000*1024*1024)); 
		info.msg_qbytes = (info.msg_qbytes+(1000*1024*1024)); 
	if (msgctl(msqid, IPC_SET, &info) == -1) 
	{ perror("msgctl"); exit(1); } 
	}

	//-----------------------FIN DE CREACION DE BUZONES DE MENSAJES----------------------------------------------------------

	//-------------CREACION MEMORIA COMPARTIDA-------------------------------------

	key_t clave1;				  // clave de acceso a la memoria 1
	int shmid1;					  // identificador de la zona de memoria 1
	clave1 = ftok(".", posicion); // creamos la clave que utilizaremos para crear la zona de memoria y luego poder vincularla

	shmid1 = shmget(clave1, sizeof(datos_comp), IPC_CREAT | 0660); // Creación de zona_mem1

	if (shmid1 != -1)
	{
		printf("Zona de memoria creada OK! con identificador %d \n", shmid1);
	}
	else
	{
		printf("ERROR al crear zona de memoria 1 !! \n");
		exit(-1);
	}

	datos_comp *datos = (datos_comp *)shmat(shmid1, 0, 0); // Vinculamos el proceso con la zona de memoria

	if (datos == (datos_comp *)-1)
	{
		printf("Error al vincular el proceso a la zona de memoria 1! \n");
		exit(1);
	}

	datos->dentro = 0;
	datos->procesos = 0;
	datos->primero = 0;

	datos->mi_ticket=0;
	datos->quiero=0;
	
	 for(int i=0;i<N;i++){
	 	datos->id_nodos_pend[i]=0;
	 }


	datos->max_ticket=0;


	//---------------------------------------DECLARACION SEMÁFOROS--------------------------------------------------------------
	char name_mutex[50];
	sprintf(name_mutex, "/MUTEX%s", argv[1]);

	sem_t *sem_mutex;
	sem_mutex = sem_open(name_mutex, O_CREAT, 0777, 1);
	if (sem_mutex == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_mutex2[50];
	sprintf(name_mutex2, "/MUTEX1%s", argv[1]);
	sem_t *sem_mutex2;
	sem_mutex2 = sem_open(name_mutex2, O_CREAT, 0777, 1);
	if (sem_mutex2 == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_paso[50];
	sprintf(name_paso, "/MUTEXPASO%s", argv[1]);
	sem_t *sem_name_paso;
	sem_name_paso = sem_open(name_paso, O_CREAT, 0777, 0);
	if (sem_name_paso == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_paso_consulta[50];
	sprintf(name_paso_consulta, "/MUTEXPASOCONSULTA%s", argv[1]);
	sem_t *sem_name_paso_consulta;

	sem_name_paso_consulta = sem_open(name_paso_consulta, O_CREAT, 0777, 0);
	if (sem_name_paso_consulta == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_paso_administracion[50];
	sprintf(name_paso_administracion, "/MUTEXPASOADMINISTRACION%s", argv[1]);
	sem_t *sem_name_paso_administracion;
	sem_name_paso_administracion = sem_open(name_paso_administracion, O_CREAT, 0777, 0);
	if (sem_name_paso_administracion == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_paso_reservas[50];
	sprintf(name_paso_reservas, "/MUTEXPASORESERVAS%s", argv[1]);
	sem_t *sem_name_paso_reservas;
	sem_name_paso_reservas = sem_open(name_paso_reservas, O_CREAT, 0777, 0);
	if (sem_name_paso_reservas == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_paso_pagos_anulaciones[50];
	sprintf(name_paso_pagos_anulaciones, "/MUTEXPASOPAGOSANULACIONES%s", argv[1]);
	sem_t *sem_name_paso_pagos_anulaciones;
	sem_name_paso_pagos_anulaciones = sem_open(name_paso_pagos_anulaciones, O_CREAT, 0777, 0);
	if (sem_name_paso_pagos_anulaciones == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_mi_ticket[50];
	sprintf(name_var_mi_ticket, "/name_var_mi_ticket%s", argv[1]);
	sem_t *sem_var_mi_ticket;
	sem_var_mi_ticket = sem_open(name_var_mi_ticket, O_CREAT, 0777, 1);
	if (sem_var_mi_ticket == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_mi_id[50];
	sprintf(name_var_mi_id, "/name_var_mi_id%s", argv[1]);
	sem_t *sem_var_mi_id;
	sem_var_mi_id = sem_open(name_var_mi_id, O_CREAT, 0777, 1);
	if (sem_var_mi_id == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_id_nodos_pend[50];
	sprintf(name_var_id_nodos_pend, "/name_var_id_nodos_pend%s", argv[1]);
	sem_t *sem_var_id_nodos_pend;
	sem_var_id_nodos_pend = sem_open(name_var_id_nodos_pend, O_CREAT, 0777, 1);
	if (sem_var_mi_ticket == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}


	char name_var_quiero[50];
	sprintf(name_var_quiero, "/name_var_quiero%s", argv[1]);
	sem_t *sem_var_quiero;
	sem_var_quiero = sem_open(name_var_quiero, O_CREAT, 0777, 1);
	if (sem_var_quiero == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_dentro[50];
	sprintf(name_var_dentro, "/name_var_dentro%s", argv[1]);
	sem_t *sem_var_dentro;
	sem_var_dentro = sem_open(name_var_dentro, O_CREAT, 0777, 1);
	if (sem_var_dentro == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_prioridad_request[50];
	sprintf(name_var_prioridad_request, "/name_var_prioridad_request%s", argv[1]);
	sem_t *sem_var_prioridad_request;
	sem_var_prioridad_request = sem_open(name_var_prioridad_request, O_CREAT, 0777, 1);
	if (sem_var_prioridad_request == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_procesos[50];
	sprintf(name_var_procesos, "/name_var_procesos%s", argv[1]);
	sem_t *sem_var_procesos;
	sem_var_procesos = sem_open(name_var_procesos, O_CREAT, 0777, 1);
	if (sem_var_procesos == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_primero[50];
	sprintf(name_var_primero, "/name_var_primero%s", argv[1]);
	sem_t *sem_var_primero;
	sem_var_primero = sem_open(name_var_primero, O_CREAT, 0777, 1);
	if (sem_var_primero == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_cont_prioridades[50];
	sprintf(name_var_cont_prioridades, "/name_var_cont_prioridades%s", argv[1]);
	sem_t *sem_var_cont_prioridades;
	sem_var_cont_prioridades = sem_open(name_var_cont_prioridades, O_CREAT, 0777, 1);
	if (sem_var_cont_prioridades == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}
	
	char name_var_prioridad_procesos[50];
	sprintf(name_var_prioridad_procesos, "/name_var_prioridad_procesos%s", argv[1]);
	sem_t *sem_var_prioridad_procesos;
	sem_var_prioridad_procesos = sem_open(name_var_prioridad_procesos, O_CREAT, 0777, 1);
	if (sem_var_prioridad_procesos == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}	
	
	char name_var_tickets_procesos[50];
	sprintf(name_var_tickets_procesos, "/name_var_tickets_procesos%s", argv[1]);
	sem_t *sem_var_tickets_procesos;
	sem_var_tickets_procesos = sem_open(name_var_tickets_procesos, O_CREAT, 0777, 1);
	if (sem_var_tickets_procesos == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_ack[50];
	sprintf(name_var_ack, "/name_var_ack%s", argv[1]);
	sem_t *sem_var_ack;
	sem_var_ack = sem_open(name_var_ack, O_CREAT, 0777, 1);
	if (sem_var_ack == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_numero_consultas[50];
	sprintf(name_var_numero_consultas, "/name_var_numero_consultas%s", argv[1]);
	sem_t *sem_var_numero_consultas;
	sem_var_numero_consultas = sem_open(name_var_numero_consultas, O_CREAT, 0777, 1);
	if (sem_var_numero_consultas == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char name_var_grifo[50];
	sprintf(name_var_grifo, "/name_var_grifo%s", argv[1]);
	sem_t *sem_var_grifo;
	sem_var_grifo = sem_open(name_var_grifo, O_CREAT, 0777, 1);
	if (sem_var_grifo == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}

	char char_nutex_request[50];
	sprintf(char_nutex_request, "/char_nutex_request%s", argv[1]);
	sem_t *mutex_request;
	mutex_request = sem_open(char_nutex_request, O_CREAT, 0777, 1);
	if (mutex_request == SEM_FAILED)
	{
		perror("Failed to open semphore for empty");
		exit(-1);
	}
	printf("Mi ID %d\n", buzon);

	//---------------------------------------DECLARACION SEMÁFOROS----------------------------------------

	//--------------------------BUCLE DE ACCIONES DEL PROGRAMA-----------------------

	printf("Esperando por mensajes...\n");

	while (1)
	{

		// RECIBE LOS MENSAJE EN SU ID DE NODO

		msgrcv(msqid, &mensaje, sizeof(struct msg), buzon, 0);
		suma_rcv++;
		printf("ME LLEGA UN MENSAJE DEL NODO %d CON TIPO DE MENSAJE %d\n", mensaje.mi_id, mensaje.tipo_mensaje);
        sem_wait(mutex_request);


		ticket_origen = mensaje.mi_ticket;
		id_nodo_origen = mensaje.mi_id;
		prioridad_mensaje = mensaje.prioridad;

		// COMPROBAMOS EL TIPO DE MENSAJE (ACK O REQUEST)tipo_mensaje

		// CASO REQUEST

		if (mensaje.tipo_mensaje == 0)
		{
			printf("Me llegó un mensaje de %d con el ticket %i y prioridad %d\n", id_nodo_origen, ticket_origen, prioridad_mensaje);



			//SI NOS LLEGA UN REQUEST DE MAYOR PRIORIDAD CUANDO HAY CONCURRENCIA DE CONSULTAS CIERRAS EL GRIFO
			sem_wait(sem_var_grifo);
			if(datos->grifo==1 && mensaje.prioridad<consultas) {
				
				printf("COMO ME LLEGA UN PROCESO MAS PRIORITARIO QUE CONSULTAS CIERRO GRIFO!!!!!!!!!!!!!!!\n");	
				datos->grifo = 0;
			}
			sem_post(sem_var_grifo);


			//CASO CONCURRENCIA CONSULTAS =====> SI EL ULTIMO REQUEST ES DE CONSULTAS ENTONCES PUEDES RESPONDER CONSULTAS


			sem_wait(sem_var_prioridad_request);
			if(mensaje.prioridad == consultas && datos->prioridad_request == consultas){
				printf("ME LLEGA UN REQUEST DE UN CONSULTAS Y COMO TENGO CONSULTAS LE RESPONDO jejejej\n");
				printf("Envio OK a buzon %d\n", id_nodo_origen);
				mensaje.mtype = id_nodo_origen; // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
				mensaje.mi_id = buzon;			// INIDICA TU NODO
				mensaje.tipo_mensaje = 1;		// INDICA EL TIPO DE MENSAJE
				mensaje.mi_ticket= ticket_origen;

				msgsnd(msqid, &mensaje, sizeof(struct msg), 0);
				sem_post(sem_var_prioridad_request);
			}else{
			sem_post(sem_var_prioridad_request);			


			sem_wait(sem_var_mi_ticket);
			datos->max_ticket = MAX(datos->max_ticket, ticket_origen); // compara su ticket con el ticket del que le llego
			sem_post(sem_var_mi_ticket);
			// COMPROBAMOS SI ENVIAMOS OK O LO AÑADIMOS A PENDIENTES

			// SI EL REQUEST LLEGA DE MAYOR PRIORIDAD DE LOS QUE ESTÁN EN LA COLA ENTONCES BORRAMOS TODOS LOS ACK

			// SI LA PRIORIDAD DEL REQUEST ES MAYOR QUE EL MIO ENTONCES HAY QUE BORRAR TUS ACKS Y VOLVER A ENVIAR REQUEST

			printf("PRIORIDAD DEL MENSAJE %d\n", prioridad_mensaje);
			printf("PRIORIDAD DE MI REQUEST %d\n", datos->prioridad_request);
			printf("MI TICKET:%d\n", datos->mi_ticket);
			printf("DENTRO:%d\n",datos->dentro);
			printf("QUIERO:%d\n",datos->quiero);

			sem_wait(sem_var_dentro);
			sem_wait(sem_var_prioridad_request);
			sem_wait(sem_var_quiero);
			if (prioridad_mensaje < datos->prioridad_request && datos->dentro == 0)
			{
			printf("HAGO PRIMER IF\n");
			sem_post(sem_var_dentro);
			sem_post(sem_var_prioridad_request);
			sem_post(sem_var_quiero);
				printf("BORRO ACKS\n");

				sem_wait(sem_var_ack);
				datos->ack = 0;
				sem_post(sem_var_ack);
				printf("ENVIO OK AL BUZON  %d PORQUE ME SUPERA EN PRIORIDAD\n", id_nodo_origen);
				mensaje.mtype = id_nodo_origen; // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
				mensaje.mi_id = buzon;			// INIDICA TU NODO
				mensaje.tipo_mensaje = 1;		// INDICA EL TIPO DE MENSAJE
				mensaje.mi_ticket= ticket_origen;

				msgsnd(msqid, &mensaje, sizeof(struct msg), 0);

				//ACTUALIZO EL TICKET
				sem_wait(sem_var_mi_ticket);
				datos->mi_ticket = datos->max_ticket + 1;
				sem_post(sem_var_mi_ticket);
				printf("VUELVO A PEDIR UN REQUEST\n");
				sem_wait(sem_var_prioridad_request);
				for (int i = 0; i <N ; i++)
				{
					if (1235 + i != buzon) {

						mensaje.mtype = 1235 + i;					  // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
						mensaje.mi_id = buzon;						  // INIDICA TU NODO
						mensaje.prioridad = datos->prioridad_request; // INDICA TU PRIORIDAD
						mensaje.tipo_mensaje = 0;					  // INDICA EL TIPO DE MENSAJE
						mensaje.mi_ticket = datos->mi_ticket;		  // ENVIAS EL MENSAJE

						msgsnd(msqid, &mensaje, sizeof(struct msg), 0);
						printf("[REQUEST] enviado a %ld\n", mensaje.mtype);
					}
					}
				sem_post(sem_var_prioridad_request);
				}
			 
			else if ((!(datos->quiero) || ticket_origen < datos->mi_ticket || (ticket_origen == datos->mi_ticket & (id_nodo_origen < buzon))) && datos->dentro == 0 
			&&( datos->prioridad_request>=mensaje.prioridad || datos->prioridad_request==0) )
			{
			printf("HAGO SEGUNDO ELSE\n");
			sem_post(sem_var_quiero);
			sem_post(sem_var_dentro);
			sem_post(sem_var_prioridad_request);

				printf("ENVIO OK AL BUZON %d PORQUE TIENE MAYOR PRIORIDAD O BIEN NO TENGO NADA\n", id_nodo_origen);
				mensaje.mtype = id_nodo_origen; // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
				mensaje.mi_id = buzon;			// INIDICA TU NODO
				mensaje.tipo_mensaje = 1;		// INDICA EL TIPO DE MENSAJE
				mensaje.mi_ticket = ticket_origen;

				msgsnd(msqid, &mensaje, sizeof(struct msg), 0);
			}

			// CASO AÑADIR A PENDIENTES
			else
			{
			printf("HAGO ULTIMO ELSE\n");
			sem_post(sem_var_quiero);
			sem_post(sem_var_dentro);
			sem_post(sem_var_prioridad_request);

			sem_wait(sem_var_prioridad_procesos);
			printf("Prioridad del mensaje = %d\n",mensaje.prioridad);
			printf("Prioridad del ya guardado = %d\n",datos->prioridad_procesos[id_nodo_origen - 1235]);
			if(mensaje.prioridad<=datos->prioridad_procesos[id_nodo_origen - 1235] || datos->prioridad_procesos[id_nodo_origen - 1235] == 0){
			sem_post(sem_var_prioridad_procesos);
	
				printf("COMO ESTOY EN LA SECCIÓN CRÍTICA SUMO A PENDIENTES EN TODOS LOS CASOS\n");
			sem_wait(sem_var_tickets_procesos);
				datos->tickets_procesos[id_nodo_origen - 1235] = ticket_origen;               //TAMBIÉN GUARDAMOS EL TICKET
			sem_post(sem_var_tickets_procesos);
			sem_wait(sem_var_id_nodos_pend);
				datos->id_nodos_pend[id_nodo_origen - 1235] = id_nodo_origen; // id_nodo_origen -1235 = posicion del nodo
			sem_post(sem_var_id_nodos_pend);
			sem_wait(sem_var_prioridad_procesos);
				datos->prioridad_procesos[id_nodo_origen - 1235] = prioridad_mensaje;
			sem_post(sem_var_prioridad_procesos);

			
				for (int i = 0; i < N; i++)
				{ if (datos->id_nodos_pend!=0){
    					printf("PENDIENTE NDOD %d CON PRIORIDAD %d\n", datos->id_nodos_pend[i], datos->prioridad_procesos[i]);
				}
					
				}
			}else sem_post(sem_var_prioridad_procesos);
			}
			}
			
		}
		
		
		
		
		
		else if (mensaje.tipo_mensaje == 1) // CASO ACK
		{

			printf("OK recibido de %d\n", id_nodo_origen);
			printf("PRIORIDAD CON QUE ME LLEGA ES %d Y MI PRIORIDAD ES %d\n", mensaje.prioridad,datos->prioridad_request );
			printf("ME LLEGA CON TICKET %d y MI TICKET ACTUAL ES %d\n",mensaje.mi_ticket, datos->mi_ticket);


			sem_wait(sem_var_ack);
			sem_wait(sem_var_prioridad_request);
			sem_wait(sem_var_mi_ticket);

			if (mensaje.prioridad == datos->prioridad_request && datos->mi_ticket== ticket_origen){
			 printf("SE SATISFACEN LAS CONDICIONES ENTONCES GUARDAS ACK\n");
				datos->ack++; // SOLO SUMAS LOS ACKS SI TE LLEGAN DE LA PRIORIDAD CORRESPONDIENTE Y EL MISMO TICKET CON EL QUE SE ENVIÓ
			}else{ 
				printf("NO SUMO ACK NO SAISFACE CONDICIONES\n");
			}

			sem_post(sem_var_ack);
			sem_post(sem_var_prioridad_request);
			sem_post(sem_var_mi_ticket);
			printf("ack %d\n", datos->ack);

			// COMO SOLO PUDO HABER PEDIDO UN PROCESO ENTONCES SI NOS HAN LLEGADO N-1 ACKS SIMPLEMENTE DESPERTAMOS AL DE MAYOR PRIORIDAD

			sem_wait(sem_var_cont_prioridades);
			sem_wait(sem_var_ack);
			
			if (datos->ack == N - 1)
			{	
				printf("%d\n\n",datos->cont_prioridades[reservas]);
				if (datos->cont_prioridades[pagos_anulaciones] != 0)
				{
					printf("[PAGOS / ANULACIONES] Concediendo acceso a SC\n");
					sem_post(sem_name_paso_pagos_anulaciones);
				}
				else if (datos->cont_prioridades[reservas] != 0)
				{

					printf("[RESERVAS] Concediendo acceso a SC\n");
					sem_post(sem_name_paso_reservas);
				}
				else if (datos->cont_prioridades[administracion] != 0)
				{
					printf("[ADMINISTRACIÓN] Concediendo acceso a SC\n");
					sem_post(sem_name_paso_administracion);
				}
				else if (datos->cont_prioridades[consultas] != 0)
				{
					printf("[CONSULTAS] Concediendo acceso a SC\n");
					sem_post(sem_name_paso_consulta);
				}

				datos->ack = 0;
				
			}
			sem_post(sem_var_ack);
			sem_post(sem_var_cont_prioridades);
		}
		
		
		
		
		
		
		
		
		printf("Esperando por mensajes...\n");
		printf("\n\n\n");
		sem_post(mutex_request);
	}
}
