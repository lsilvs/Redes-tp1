#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include "funcoes.h"
#include "servidor.h"
#include "socket6.h"

int main(int argc, char** argv)
{
	// Cria o socket server
	inicializaServer();

	// Inicializa o vetor de posicao de clientes
	int i;
	for (i = 0; i < MAX_CLIENTES; i++)
		vPosicaoClientes[i] = 0;

	// Guarda o tamanho da estrutura...
	vTamanho = sizeof(struct sockaddr_in);

	while (1)
	{
		// Busca o proxima posicao vazia para um cliente
		vID = buscaPosicao();

		if (vID >= 0)
		{
		
			// Aguarda a conexao de um cliente
			if ((vClientes[vID].vSocket = accept(vSocketServer, (struct sockaddr *) &vClientes[vID].vRemoto, &vTamanho)) < 0)
			{
				perror("accept");
				close(vClientes[vID].vSocket);
			}
		
			/* DEBUG */
			printf("Servidor: chegando conexao de %s\n",(char *)inet_ntoa(vClientes[vID].vRemoto.sin_addr));

			if (send(vClientes[vID].vSocket, "+OK POP3 server ready!\n", strlen("+OK POP3 server ready!\n"), 0) == -1)
			{
				/* DEBUG */
				printf("Erro ao enviar boas-vindas!\n");
				perror("send");
				close(vClientes[vID].vSocket);
				exit(0);
			}

			// Aguarda mensagem
			processaMensagem(vID);

			printf("Fechando conexao de %s...\n",(char *)inet_ntoa(vClientes[vID].vRemoto.sin_addr));
		
			// Libera a memoria armazenada
			free(vClientes[vID].vUsuario);
			free(vClientes[vID].vSenha);
			//free(vClientes[vID].vListaEmails);
		
			close(vClientes[vID].vSocket);
		}
	}
}