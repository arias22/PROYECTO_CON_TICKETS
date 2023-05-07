#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>



int main(int argc, char* argv[]) {



    int msqid2_glob;

    msqid2_glob=atoi(argv[1]);

    // BORRAR BUZON MENSAJES
	if (msgctl(msqid2_glob, IPC_RMID, NULL) == -1)
	{
		perror("msgctl");
	}
}