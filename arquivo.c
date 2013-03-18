#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include "servidor.h"
#include "arquivo.h"

void leArquivosMensagens(int idCliente)
{
	// Monta o nome do usuario com o .
	char *vUsuarioBusca;
	vUsuarioBusca = (char *) malloc(strlen(vClientes[idCliente].vUsuario+2));
	strncpy(vUsuarioBusca,vClientes[idCliente].vUsuario,strlen(vClientes[idCliente].vUsuario));
	strcat(vUsuarioBusca,".\0");
	
	DIR *dir;
	struct dirent *ent;
	
	dir = opendir(DIR_LOCAL);

	int vNumArquivos;
	int contador;
	int posicao;
	vNumArquivos = 0;	
	char *ptr;
	FILE *fp;
	struct stat dadosTexto;

	char *vNomeArquivo;

	if (dir != NULL)
	{
		// Le os arquivos do disco
		while ((ent = readdir (dir)) != NULL)
		{
			// Busca aqui as mensagens do usuario
			if (strncmp(vUsuarioBusca,ent->d_name,strlen(vUsuarioBusca))==0)
			{
				// Arquivo do usuario encontrado
				printf("Encontrou o arquivo %s para o usuario %s\n",ent->d_name,vUsuarioBusca);
				vNumArquivos++;				
			}
		}
		
		// Realoca o vetor de emails
		vClientes[idCliente].vListaEmails = (Email *) malloc(vNumArquivos*sizeof(Email));
		// Seta o numero de emails
		vClientes[idCliente].vNumEmails = 0;
		
		// Volta o ponteiro para o inicio dos arquivos
		rewinddir(dir);

		contador = 0;
		while ((ent = readdir (dir)) != NULL)
		{
			// Marca o email como nao excluido
			vClientes[idCliente].vListaEmails[contador].deletado = 0;
			
			// Guarda as informacoes dos arquivos
			if (strncmp(vUsuarioBusca,ent->d_name,strlen(vUsuarioBusca))==0)
			{
				// Armazena o nome do arquivo
				strncpy(vClientes[idCliente].vListaEmails[contador].nome,ent->d_name,strlen(ent->d_name));
			
				vNomeArquivo = (char *) malloc(strlen(ent->d_name)*sizeof(char));
				strcpy(vNomeArquivo,ent->d_name);
				//printf("A comparacao entre %s e %s resulta em %d no final\n",vNomeArquivo,"meunomedeusuario.2.txt",strcmp(vNomeArquivo,"meunomedeusuario.2.txt"));
				
				// DEBUG
				printf("Comecou outro arquivo com %s como nome que tem %d caracteres\n",ent->d_name,strlen(ent->d_name));

				// Quebra o nome do arquivo nos '.'
				ptr = strtok(ent->d_name,".");
				posicao = 0;
				while (ptr != NULL)
				{
					if (posicao == 1)
					{
						// Guarda o id do arquivo
						//vClientes[idCliente].vListaEmails[contador].id = (char *) malloc((strlen(ptr)+1)*sizeof(char));
						//strcpy(vClientes[idCliente].vListaEmails[contador].id,ptr);
						//vClientes[idCliente].vListaEmails[contador].id[strlen(ptr)] = "\0";
						vClientes[idCliente].vListaEmails[contador].id = atoi(ptr);
					}
					ptr = strtok(NULL,".");
					posicao++;
				}

				// Abre o arquivo, le o tamanho do mesmo
				fp = fopen(vNomeArquivo,"r");
				if (!fp)
				{
					printf ("Erro na abertura do arquivo. Fim de programa.\n");
					exit (1);
				}
				
				if (stat(vNomeArquivo, &dadosTexto)<0)
				{
					printf ("Erro ao ler informacoes do arquivo. Fim de programa.\n");
					exit (1);
				}
				
				// Armazena o tamanho
				vClientes[idCliente].vListaEmails[contador].tamanho = dadosTexto.st_size;

				fclose(fp);
			
				// Conta mais um email
				contador++;
				vClientes[idCliente].vNumEmails++;
			}
		}
		// Fecha o diretorio
		closedir (dir);
	}
	else
	{
		// could not open directory
		perror ("");
		exit(1);
	}
	
	if (contador > 0)
	{
		// Libera a memoria temporaria
		free(vNomeArquivo);
		free(vUsuarioBusca);
	}
}

void enviaResumoMensagens(int idCliente)
{
	// Envia o resumo das mensagens do usuario

	int i;
	int vTotal;
	vTotal = 0;
	char vBufTmp[MAXDATASIZE];
	int vNumeroTotalEmails;
	vNumeroTotalEmails = 0;

	for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
	{
		if (vClientes[idCliente].vListaEmails[i].deletado == 0)
		{
			vNumeroTotalEmails++;
			vTotal = vTotal + (int) vClientes[idCliente].vListaEmails[i].tamanho;
		}
	}

	// Envia o numero total
	// +OK mrose's maildrop has 2 messages (320 octets)

	sprintf(vBufTmp,"+OK %s's maildrop has %d messages (%d octects)",vClientes[idCliente].vUsuario,vNumeroTotalEmails,vTotal);
	//printf("O buffer eh %s aqui\n",vBufTmp);

	if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
	{
		perror("send");
		close(vClientes[vID].vSocket);
		exit(0);
	}
}

void enviaStatMensagens(int idCliente)
{
	// Envia o resumo das mensagens do usuario

	int i;
	int vTotal;
	vTotal = 0;
	char vBufTmp[MAXDATASIZE];

	for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
	{
		if (vClientes[idCliente].vListaEmails[i].deletado == 0)
			vTotal = vTotal + (int) vClientes[idCliente].vListaEmails[i].tamanho;
	}

	sprintf(vBufTmp,"+OK %d %d",vClientes[idCliente].vNumEmails,vTotal);
	//printf("O buffer eh %s aqui\n",vBufTmp);

	if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
	{
		perror("send");
		close(vClientes[vID].vSocket);
		exit(0);
	}
}

void enviaListMensagens(int idCliente, int vIdMsg)
{
	// Envia o resumo das mensagens do usuario

	int i;
	int vTotal;
	vTotal = 0;
	char vBufTmp[MAXDATASIZE];
	int vNumeroTotalEmails;
	vNumeroTotalEmails = 0;
	
	if (vIdMsg == 0)
	{
		for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
		{
			if (vClientes[idCliente].vListaEmails[i].deletado == 0)
			{
				vNumeroTotalEmails++;
				vTotal = vTotal + (int) vClientes[idCliente].vListaEmails[i].tamanho;
			}
		}

		sprintf(vBufTmp,"+OK %d messages (%d octets)",vNumeroTotalEmails,vTotal);
		//printf("%s\n",vBufTmp);

		if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
		{
			perror("send");
			close(vClientes[vID].vSocket);
			exit(0);
		}
		sleep(1);
		
		// Agora envia informacoes individuais
		for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
		{
			if (vClientes[idCliente].vListaEmails[i].deletado == 0)
			{
				sprintf(vBufTmp,"%d %d",(i+1),(int) vClientes[idCliente].vListaEmails[i].tamanho);
				//printf("%s\n",vBufTmp);
				if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
				{
					perror("send");
					close(vClientes[vID].vSocket);
					exit(0);
				}
				sleep(1);
			}
		}
	
		// Envia um "."
		//printf("%s\n",".");
		if (send(vClientes[vID].vSocket, ".", strlen("."), 0) == -1)
		{
			perror("send");
			close(vClientes[vID].vSocket);
			exit(0);
		}
	}
	else
	{
		// Agora envia informacoes individuais
		for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
		{
			if ((vClientes[idCliente].vListaEmails[i].id == vIdMsg) && (vClientes[idCliente].vListaEmails[i].deletado == 0))
			{
				sprintf(vBufTmp,"+OK %d %d",vIdMsg,(int) vClientes[idCliente].vListaEmails[i].tamanho);
				printf("%s\n",vBufTmp);
				if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
				{
					perror("send");
					close(vClientes[vID].vSocket);
					exit(0);
				}
			}
		}
	}
}

void enviaRetrMensagens(int idCliente, int vIdMsg)
{
	if (vClientes[idCliente].vNumEmails == 0)
		return;
	
	// Envia o resumo das mensagens do usuario

	int i;
	int vTotal;
	vTotal = 0;
	char vBufTmp[MAXDATASIZE];
	char *vNomeArquivo;
	char *texto;
	struct stat dadosTexto;
	FILE *fp;

	// Agora envia informacoes individuais
	for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
	{
		if ((vClientes[idCliente].vListaEmails[i].id == vIdMsg) && (vClientes[idCliente].vListaEmails[i].deletado == 0))
		{
			// Envia o tamanho da mensagem
			
			sprintf(vBufTmp,"+OK %d octets",(int) vClientes[idCliente].vListaEmails[i].tamanho);
			printf("%s\n",vBufTmp);
			sleep(1);

			if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
			{
				perror("send");
				close(vClientes[vID].vSocket);
				exit(0);
			}

			// Le a mensagem e envia os dados
			vNomeArquivo = (char *) malloc(strlen(vClientes[idCliente].vListaEmails[i].nome)*sizeof(char));
			strcpy(vNomeArquivo,vClientes[idCliente].vListaEmails[i].nome);
			
			// Abre o arquivo, le o tamanho do mesmo
			fp = fopen(vNomeArquivo,"r");
			if (!fp)
			{
				printf ("Erro na abertura do arquivo. Fim de programa.\n");
				exit (1);
			}
			
			if (stat(vNomeArquivo, &dadosTexto)<0)
			{
				printf ("Erro ao ler informacoes do arquivo. Fim de programa.\n");
				exit (1);
			}

			// Aloca espaco para o texto inteiro
			texto = (char *)malloc(dadosTexto.st_size*sizeof(char));
			
			if (!texto)
			{
				fclose(fp);
				return;
			}

			// Le todo o texto
			texto[0] = "\n";
			fread(texto, sizeof(char), dadosTexto.st_size, fp);

			// Envia o texto
			sprintf(vBufTmp,"<%s>",texto);
			printf("%s\n",vBufTmp);

			if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
			{
				perror("send");
				close(vClientes[vID].vSocket);
				exit(0);
			}
			
			fclose(fp);			
		}
	}

	free(vNomeArquivo);
	free(texto);
}

void enviaDeleMensagens(int idCliente, int vIdMsg)
{
	if (vClientes[idCliente].vNumEmails == 0)
		return;
	
	int i;
	char vBufTmp[MAXDATASIZE];
	
	// Agora envia informacoes individuais
	for (i = 0; i < vClientes[idCliente].vNumEmails; i++)
	{
		if ((vClientes[idCliente].vListaEmails[i].id == vIdMsg) && (vClientes[idCliente].vListaEmails[i].deletado == 0))
		{
			vClientes[idCliente].vListaEmails[i].deletado = 1;
			
			sprintf(vBufTmp,"+OK message %d deleted",vIdMsg);
			//printf("%s\n",vBufTmp);

			if (send(vClientes[vID].vSocket, vBufTmp, strlen(vBufTmp), 0) == -1)
			{
				perror("send");
				close(vClientes[vID].vSocket);
				exit(0);
			}
		}
	}
}

void enviaQuitMensagens(int idCliente)
{
	// TODO
}
