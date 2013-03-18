#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "socket4.h"
#include "servidor.h"

void inicializaServer()
{
	// Cria o socket
	vSocketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (vSocketServer == -1) {
		perror("socket");
		exit(1);
	}

	// Configura o socket
	memset(&vConfigServer, 0, sizeof(vConfigServer));
	vConfigServer.sin_family = AF_INET;
	vConfigServer.sin_addr.s_addr = INADDR_ANY;
	vConfigServer.sin_port = htons(PORTA_TP);
	
	// Atribui a porta ao socket
	if (bind(vSocketServer, (struct sockaddr *) &vConfigServer, sizeof(vConfigServer)) == -1)
	{
		perror("bind");
		close(vSocketServer);
		exit(1);
	}

	// Espera algum contato na porta
	if (listen(vSocketServer, 5) == -1)
	{
		perror("listen");
		close(vSocketServer);
	}
}
