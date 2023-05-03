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
#define MAX(i, j) (((i) > (j)) ? (i) : (j))
#define SIZE 50
#define pagos_anulaciones 0
#define reservas 1
#define administracion 2
#define consultas 3

// --------- VARIABLES COMPARTIDAS----------
typedef struct datos_comp
{
	int mi_ticket;
	int mi_id;
	int id_nodos_pend[100];
	int num_pend;
	int quiero;
	int max_ticket;
	// añadidas
	int dentro;
	int prioridad_request;
	int procesos;
	int primero;
	int cont_prioridades[4];
	int prioridad_procesos[100];
	int ack;
	int espera;
	int consultas_dentro;
} datos_comp;

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

	exit(0);
}

int main(int argc, char *argv[])
{
	struct sigaction ss;
	ss.sa_handler = handle_sigint;
	ss.sa_flags = 0;
	sigaction(2, &ss, NULL);

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

	int msqid = msgget(500, 0666 | IPC_CREAT);
	msqid2_glob = msqid;

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

	for (int i = 0; i < N; i++)
	{
		datos->id_nodos_pend[i] = 0;
	}

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
	printf("Mi ID %d\n", buzon);

	//---------------------------------------DECLARACION SEMÁFOROS----------------------------------------

	//--------------------------BUCLE DE ACCIONES DEL PROGRAMA-----------------------

	printf("Esperando por mensajes...\n");

	while (1)
	{

		// RECIBE LOS MENSAJE EN SU ID DE NODO

		msgrcv(msqid, &mensaje, sizeof(struct msg), buzon, 0);

		printf("ME LLEGA UN MENSAJE DEL NODO %d CON TIPO DE MENSAJE %d\n", mensaje.mi_id, mensaje.tipo_mensaje);

		ticket_origen = mensaje.mi_ticket;
		id_nodo_origen = mensaje.mi_id;
		prioridad_mensaje = mensaje.prioridad;

		// COMPROBAMOS EL TIPO DE MENSAJE (CANCELACION, ACK O REQUEST)tipo_mensaje

		// CASO REQUEST

		if (mensaje.tipo_mensaje == 0)
		{
			printf("Me llegó un mensaje de %d con el ticket %i y prioridad %d\n", id_nodo_origen, ticket_origen, max_prioridad);

			sem_wait(sem_mutex);

			datos->max_ticket = MAX(datos->max_ticket, ticket_origen); // compara su ticket con el ticket del que le llego

			// COMPROBAMOS SI ENVIAMOS OK O LO AÑADIMOS A PENDIENTES

			// SI EL REQUEST LLEGA DE MAYOR PRIORIDAD DE LOS QUE ESTÁN EN LA COLA ENTONCES BORRAMOS TODOS LOS ACK

			// SI LA PRIORIDAD DEL REQUEST ES MAYOR QUE EL MIO ENTONCES HAY QUE BORRAR TUS ACKS Y VOLVER A ENVIAR REQUEST

			printf("PRIORIDAD DEL MENSAJE %d\n", prioridad_mensaje);
			printf("PRIORIDAD DE MI REQUEST %d\n", datos->prioridad_request);
			printf("MI TICKET:%d\n", datos->mi_ticket);

			if (prioridad_mensaje < datos->prioridad_request && datos->espera != 0 && datos->dentro == 0)
			{

				printf("BORRO ACKS\n");
				datos->ack = 0;
				printf("Envio OK a buzon %d\n", id_nodo_origen);
				mensaje.mtype = id_nodo_origen; // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
				mensaje.mi_id = buzon;			// INIDICA TU NODO
				mensaje.tipo_mensaje = 1;		// INDICA EL TIPO DE MENSAJE

				msgsnd(msqid, &mensaje, sizeof(struct msg), 0);

				//ACTUALIZO EL TICKET
				datos->mi_ticket = datos->max_ticket + 1;
				for (int i = 0; i <= N - 1; i++)
				{
					if (1235 + i != buzon)



						

						mensaje.mtype = 1235 + i;					  // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
						mensaje.mi_id = buzon;						  // INIDICA TU NODO
						mensaje.prioridad = datos->prioridad_request; // INDICA TU PRIORIDAD
						mensaje.tipo_mensaje = 0;					  // INDICA EL TIPO DE MENSAJE
						mensaje.mi_ticket = datos->mi_ticket;		  // ENVIAS EL MENSAJE

						msgsnd(msqid, &mensaje, sizeof(struct msg), 0);
						printf("[REQUEST] enviado a %ld\n", mensaje.mtype);
					}
				}
			
			else if ((!(datos->quiero) || ticket_origen < datos->mi_ticket || (ticket_origen == datos->mi_ticket & (id_nodo_origen < buzon))) && datos->dentro == 0 )
			{

				printf("Envio OK a buzon %d\n", id_nodo_origen);
				mensaje.mtype = id_nodo_origen; // INDICA AL NODO  QUE ENVÍAS EL MENSAJE
				mensaje.mi_id = buzon;			// INIDICA TU NODO
				mensaje.tipo_mensaje = 1;		// INDICA EL TIPO DE MENSAJE

				msgsnd(msqid, &mensaje, sizeof(struct msg), 0);
			}

			// CASO AÑADIR A PENDIENTES
			else
			{

				datos->id_nodos_pend[id_nodo_origen - 1235] = id_nodo_origen; // id_nodo_origen -1235 = posicion del nodo
				datos->prioridad_procesos[id_nodo_origen - 1235] = prioridad_mensaje;
				printf("PENDIENTE NDOD %d CON PRIORIDAD %d\n", datos->id_nodos_pend[id_nodo_origen - 1235], datos->prioridad_procesos[id_nodo_origen - 1235]);

				int ctr = 0;

				for (int i = 0; i < N; i++)
				{
					if (datos->id_nodos_pend[i] != 0)
						ctr++;
				}
				printf("Nº NODOS PENDIENTES %d\n", ctr);

				for (int i = 0; i < N; i++)
				{
					printf("PENDIENTE NDOD %d CON PRIORIDAD %d\n", datos->id_nodos_pend[i], datos->prioridad_procesos[i]);
				}
			}

			sem_post(sem_mutex);
		}else if (mensaje.tipo_mensaje == 1) // CASO ACK
		{

			printf("OK recibido de %d\n", id_nodo_origen);
			printf("Prioridad destinatario %d\n", mensaje.prioridad);

			if (mensaje.prioridad == datos->prioridad_request)
				datos->ack++; // SOLO SUMAS LOS ACKS SI TE LLEGAN DE LA PRIORIDAD CORRESPONDIENTE

			printf("ack %d\n", datos->ack);

			// COMO SOLO PUDO HABER PEDIDO UN PROCESO ENTONCES SI NOS HAN LLEGADO N-1 ACKS SIMPLEMENTE DESPERTAMOS AL DE MAYOR PRIORIDAD

			if (datos->ack == N - 1)
			{
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
				printf("Esperando por mensajes...\n");
			}
		}
		printf("\n\n\n");
	}
}
