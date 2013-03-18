#define PORTA_TP 1234
#define MAXDATASIZE 2048
#define MAX_CLIENTES 10

// Outras variaveis auxiliares
int 	vTamanho;
// TODO
char 	vBuffer[MAXDATASIZE];
int 	vNumBytes;
int 	vID;

typedef struct Email {
	//char *id;
	int deletado;
	int id;
	char nome[256];
	off_t tamanho;
} Email;

typedef struct Cliente {
	char *vUsuario;
	char *vSenha;
	// Lista de e-mails lidos do arquivo durante o login
	Email *vListaEmails;
	// Variavel de maquina de estados [0] StandBy; [1] User OK; [2] Password OK	
	int  vEstado;
	// Socket
	int vSocket;
	// Configuracao do socket
	struct sockaddr_in vRemoto;
	// Maquina de estados
	int vFluxo;
	// Numero de emails
	int vNumEmails;
} Cliente;

// Aceita ate no maximo MAX_CLIENTES clientes simultaneos

// Para saber qual posicao esta livre mantemos um vetor de inteiros
int     vPosicaoClientes[MAX_CLIENTES];
// O vetor abaixo guarda cada cliente conectado
Cliente vClientes[MAX_CLIENTES];