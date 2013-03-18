#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include "servidor.h"
#include "funcoes.h"
#include "arquivo.h"

void processaMensagem(int idCliente)
{
	vClientes[idCliente].vFluxo = 1;
	
	while (vClientes[idCliente].vFluxo == 1)
	{
		// Le o socket
		leSocket(idCliente);

		// Trata a mensagem
		leComando(idCliente);

		bzero((char *)&vBuffer, sizeof(vBuffer));
		vBuffer[0] = '\0';
	}
}

void leSocket(idCliente)
{
	if ((vNumBytes = recv(vClientes[idCliente].vSocket, vBuffer, MAXDATASIZE, 0)) == -1) 
	{
		// DEBUG
		printf("Erro ao receber mensagens!\n");
		perror("recv");
		exit(1);
	}
	// TODO
	vBuffer[vNumBytes] = '\0';
}

void leComando(idCliente)
{
	int vValidaEstado = 0;
	int vIdMsg;
	
	if (strncmp(vBuffer,"USER",4) == 0)
	{
		extraiUsuario(idCliente);

		// Altera a maquina de estados
		vValidaEstado = alteraEstado(1,idCliente);
		
		//DEBUG
		//printf("O nome de usuario do cliente %d eh %s, ok?\n", idCliente, vClientes[idCliente].vUsuario);
	}
	else if (strncmp(vBuffer,"PASS",4) == 0)
	{
		extraiSenha(idCliente);

		// Altera a maquina de estados
		vValidaEstado = alteraEstado(2,idCliente);
		if (vValidaEstado == 1)
		{			
			//DEBUG
			printf("USER e PASS = %s do cliente %d OK, lendo mensagens...\n", vClientes[idCliente].vSenha, idCliente);

			// Marca o numero de emails como 0
			vClientes[idCliente].vNumEmails = 0;
			
			// Le as mensagens do usuario
			leArquivosMensagens(idCliente);

			// Envia resumo de usuario
			enviaResumoMensagens(idCliente);
		}
	}
	else if (strncmp(vBuffer,"STAT",4) == 0)
	{
		//DEBUG
		printf("Chegou STAT do cliente %d\n",idCliente);

		// Recupera a STAT das mensagens do cliente
		enviaStatMensagens(idCliente);
	}
	else if (strncmp(vBuffer,"LIST",4) == 0)
	{
		//DEBUG
		printf("Chegou LIST do cliente %d\n",idCliente);

		vIdMsg = extraiIdMsg(idCliente);

		//printf("Aqui vIdMsg = %d ...\n",vIdMsg);

		// Recupera a LIST das mensagens do cliente
		enviaListMensagens(idCliente,vIdMsg);
	}
	else if (strncmp(vBuffer,"RETR",4) == 0)
	{
		//DEBUG
		printf("Chegou RETR do cliente %d\n",idCliente);

		vIdMsg = extraiIdMsg(idCliente);

		if (vIdMsg > 0)
		{
			// Recupera a RETR da mensagen do cliente
			enviaRetrMensagens(idCliente,vIdMsg);
		}
	}
	else if (strncmp(vBuffer,"DELE",4) == 0)
	{
		//DEBUG
		printf("Chegou DELE do cliente %d\n",idCliente);

		vIdMsg = extraiIdMsg(idCliente);

		if (vIdMsg > 0)
		{
			// Recupera a DELE das mensagens do cliente
			enviaDeleMensagens(idCliente,vIdMsg);
		}
	}
	else if (strncmp(vBuffer,"QUIT",4) == 0)
	{
		//DEBUG
		printf("Chegou QUIT do cliente %d!\n",idCliente);
		vClientes[idCliente].vFluxo = 0;

		// Recupera a QUIT das mensagens do cliente
		enviaQuitMensagens(idCliente);
	}
}

void extraiUsuario(int idCliente)
{
	//printf("O buffer eh %s e o tamanho eh %d letras\n",vBuffer,strlen(vBuffer));
	int vTamTexto = strlen(vBuffer)-5;
	
	vClientes[idCliente].vUsuario = (char *) malloc(vTamTexto+1);
	strncpy(vClientes[idCliente].vUsuario,vBuffer+5,vTamTexto);
	vClientes[idCliente].vUsuario[vTamTexto+1] = "\0";
}

void extraiSenha(int idCliente)
{
	int vTamTexto = strlen(vBuffer)-5;
	
	vClientes[idCliente].vSenha = (char *) malloc(vTamTexto+1);
	strncpy(vClientes[idCliente].vSenha,vBuffer+5,vTamTexto);
	vClientes[idCliente].vSenha[vTamTexto+1] = "\0";
}

int extraiIdMsg(int idCliente)
{
	char *vId;
	if (strlen(vBuffer)>4)
	{
		int vTamTexto = strlen(vBuffer)-5;
	
		vId = (char *) malloc(vTamTexto);
	
		strncpy(vId,vBuffer+5,vTamTexto);
		return atoi(vId);
	}
	else
		return 0;
}

int alteraEstado(int estado, int idCliente)
{
	// Verifica se um estado esta sendo pulado
	if ((vClientes[idCliente].vEstado == (estado-1)) || (vClientes[idCliente].vEstado == estado))
		vClientes[idCliente].vEstado = estado;
	else
		return 0;
	return 1;
}

int buscaPosicao()
{
	int i;
	// Procura uma posicao livre
	for (i = 0; i < MAX_CLIENTES; i++)
		if (vPosicaoClientes[i] == 0)
			return i;

	return -1;
}
