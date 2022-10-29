#include "header.h"

// Un regalito, puede que quieran modificarla
// Dado un arreglo de char str y un socket descriptor s, hace una lectura
// bloqueante sobre s y llena el buffer str con los bytes leídos.
int read_sock(char str[], int s) 
{
    int n;
    n = recv(s, str, 2*256, 0);

    if (n == 0) 
        return -1;
    if (n < 0) { 
        perror("recibiendo");
        exit(1);
    }
    str[n] = '\0';
    printf("%d\n",n);
    printf("recibi: %s\n",str);
    return 0;
}


// Dado un puntero a un request req y un socket s, recibe una request en s y la 
// almacena en req. La funcion es bloqueante
void get_request(struct request* req, int s)
{
    char request[256 + 10];
    int aux = recv(s, request, 256 + 10, 0);
    if (aux < 0) { 
    	perror("recibiendo");
    }
    strncpy(req->type,((struct request*)request)->type, 10);
    strncpy(req->msg, ((struct request*)request)->msg, 256);
}

// Dado un vector de enteros que representan socket descriptors y un request,
// envía a traves de todos los sockets la request.
void broadcast(vector<vector<int>> &sockets, struct request* req)
{
    for (size_t i = 0; i < sockets.size(); i++)
    {
        for (size_t j = 0; j < sockets.size(); j++)
        {
            send_request(sockets[i][j], req);
        }
        
    }
}

// Por siempre, acepta conexiones sobre un socket s en estado listen y 
// agrega los sockets asociados al vector v.
void accept_conns(int s, vector<int> &v)
{
     // TO DO  
}
// Dado un puerto lsn_port devuelve un socket en estado listen asociado
// a todas las interfaces de red local y a ese puerto (ej 127.0.0.1:lsn_port)
int set_acc_socket(int lsn_port)
{
    return 0; 
}

// Setea un socket al modo nobloqueante
static int nonblockingsocket(int s)
{
    int flags;

    if ((flags = fcntl(s, F_GETFL)) == -1)
        return -1;

    return fcntl(s, F_SETFL, flags | O_NONBLOCK);
}

void send_request(int socket, struct request* req)
{
    int aux = send(socket, (char *) req , 256 + 10, 0);
    if (aux < 0) { 
    	perror("enviando");
    }
    
}

int read_socket(int s) {
	int n;
	char str[256];
    n = recv(s, str, 256, 0);
    if (n == 0) 
        return -1;
    if (n < 0) { 
    	perror("recibiendo");
    	return 0;
    }
    str[n] = '\0'; /* Agregar caracter de fin de cadena a lo recibido. */
    printf("%s", str);
    return 0;
}

void connection_handler(int dSocket){
    while(1){
        if(read_socket(dSocket) == -1)
            break;
    }      
}