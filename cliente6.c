/* Headers */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#define PORTA 1234    /* Porta para conectar */
#define MAXDATASIZE 2048 /* máximo número de bytes que poderemos enviar
                           por vez */

int main(int argc, char *argv[]) {
	int Meusocket, numbytes, n = 0;
	char buf[MAXDATASIZE], bufAux[MAXDATASIZE], vAux[MAXDATASIZE], * username, * password;
	char opcao;
	struct hostent * hostname;
	struct sockaddr_in6 seu_endereco;

	if (argc != 4) {
		fprintf(stderr,"Parametros esperados: ./cliente username password hostname\n");
		exit(1);
	}

	// Pega o username
	if ((username = argv[1]) == NULL)  {
		herror("gethostbyname");
		exit(1);
	}

	// Pega o password
	if ((password = argv[2]) == NULL)  {
		herror("gethostbyname");
		exit(1);
	}

	// Pega o hostname
	if ((hostname = gethostbyname(argv[3])) == NULL)  {
		herror("gethostbyname");
		exit(1);
	}

	if ((Meusocket = socket(AF_INET6, SOCK_STREAM, 0)) == -1)  {
		perror("socket");
		exit(1);
	}

	seu_endereco.sin6_family = AF_INET6;
	seu_endereco.sin6_port = htons(PORTA);
	seu_endereco.sin6_addr = *((struct in6_addr *) hostname->h_addr);
	//bzero(&(seu_endereco.sin_zero), 8);

	if (connect(Meusocket,(struct sockaddr *)&seu_endereco, sizeof(struct sockaddr)) ==-1)  {
		perror("connect");
		exit(1);
	}

	if ((numbytes=recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	printf("%s\n", buf);

	n = sprintf(bufAux, "USER %s", username);
	send(Meusocket, bufAux, n, 0);
	sleep(1);
	
	n = sprintf(bufAux, "PASS %s", password);
	send(Meusocket, bufAux, n, 0);
	sleep(1);

	if ((numbytes=recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	printf("%s\n", buf);

	n = sprintf(bufAux, "LIST");
	send(Meusocket, bufAux, n, 0);

	// Recebe primeiro o cumprimento
	if ((numbytes=recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
		perror("recv");
		exit(1);
	}
	buf[numbytes] = '\0';
	
	//printf("%s como buffer aqui 0 com tamanho %d\n",buf,strlen(buf));
	
	// Agora deve esperar ate receber um "."
	int status;
	status = 0;
	char *ptr, *emails[256];
	int i, numEmails;
	numEmails = 0;
	int vListaEmails[256];
	int emailAtual;
	emailAtual = -1;
	
	while (status == 0)
	{
		if ((numbytes=recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
			perror("recv");
			exit(1);
		}
		buf[numbytes] = '\0';

		if (numbytes > 2)
		{
			strcpy(vAux, buf);
			ptr = strtok(vAux," ");
			vListaEmails[i] = atoi(ptr);
			i++;
			numEmails++;

			//printf("%s como buffer aqui 1 com tamanho %d\n",buf,strlen(buf));
		}
		else
		{
			status = 1;

			//printf("%s como buffer aqui2 com tamanho %d\n",buf,strlen(buf));
		}
	}

	/*
	printf("Achou %d emails...\n",numEmails);
	for (i = 0; i < numEmails; i++)
	{
		printf("Número do e-mail %d eh %d\n",i,vListaEmails[i]);
	}
	*/

	printf("\n\n==============================================================\n");
	printf("Escolha uma das opcoes:\n");
	printf("(p) ler proxima | (a) ler anterior | (e) excluir atual | (s) sair");
	printf("\n==============================================================\n\n");

	while(1) {
		scanf("%c", &opcao);

		system("clear");

		printf("\n\n==============================================================\n");
		printf("Escolha uma das opcoes:\n");
		printf("(p) ler proxima | (a) ler anterior | (e) excluir atual | (s) sair");
		printf("\n==============================================================\n\n");

		if (opcao == 'p') {
			if (emailAtual < (numEmails-1))
			{
				emailAtual++;
				n = sprintf(bufAux, "RETR %d", vListaEmails[emailAtual]);
				send(Meusocket, bufAux, 7, 0);

				// Le o cumprimento
				if ((numbytes = recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
					perror("recv");
					exit(1);
				}
				buf[0] = '\0';
				//printf("%s\n\n", buf);
				
				// Le o e-mail
				if ((numbytes = recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
					perror("recv");
					exit(1);
				}
				buf[numbytes] = '\0';
				printf("%s\n\n", buf);
			}
			else
			{
				printf("Nao existe proximo email\n");
			}
		}
		else if (opcao == 'a')
		{
			if (emailAtual > 0)
			{
				emailAtual--;
				n = sprintf(bufAux, "RETR %d", vListaEmails[emailAtual]);
				send(Meusocket, bufAux, 7, 0);

				// Le o cumprimento
				if ((numbytes = recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)
				{
					perror("recv");
					exit(1);
				}
				buf[0] = '\0';
				//printf("%s\n\n", buf);
				
				// Le o e-mail
				if ((numbytes = recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
					perror("recv");
					exit(1);
				}
				buf[numbytes] = '\0';
				printf("%s\n\n", buf);
			}
			else
			{
				printf("Nao existe email anterior\n");
			}
			
		} else if (opcao == 'e')
		{
			if (emailAtual >= 0 && emailAtual < numEmails)
			{
				n = sprintf(bufAux, "DELE %d", vListaEmails[emailAtual]);
				send(Meusocket, bufAux, 7, 0);
				if ((numbytes = recv(Meusocket, buf, MAXDATASIZE, 0)) == -1)  {
					perror("recv");
					exit(1);
				}
				buf[numbytes] = '\0';
				printf("%s\n\n", buf);
			}
			else
			{
				printf("Abra um email para exclui-lo\n");
			}
		}
		else if (opcao == 's')
		{
			n = sprintf(bufAux, "QUIT");
			send(Meusocket, bufAux, n, 0);
			printf("Saindo...\n");
			exit(1);
		}
		else
		{
			printf("opcao invalida\n");
		}

		// para pegar o enter que vem junto com o caracter escolhido
		scanf("%c", &opcao);
	}

	close(Meusocket);
	return 0;
}