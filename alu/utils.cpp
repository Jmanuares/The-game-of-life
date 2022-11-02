#include "header.h"

// Un regalito, puede que quieran modificarla
// Dado un arreglo de char str y un socket descriptor s, hace una lectura
// bloqueante sobre s y llena el buffer str con los bytes leídos.
int read_sock(char str[], int s) {
    int n;
    n = recv(s, str, 2*256, 0);

    if (n == 0) return -1;
    if (n < 0){ 
        perror("receiving");
        exit(1);
    }
    str[n] = '\0';
    printf("%d\n",n);
    printf("get: %s\n",str);
    return 0;
}


// Dado un puntero a un request req y un socket s, recibe una request en s y la 
// almacena en req. La funcion es bloqueante
void get_request(struct request* req, int s){
    char request[256 + 10];
    int aux = recv(s, request, 256 + 10, 0);
    if (aux < 0){ 
    	perror("receiving");
    }
    strncpy(req->type,((struct request*)request)->type, 10);
    strncpy(req->msg, ((struct request*)request)->msg, 256);
}

// Dado un vector de enteros que representan socket descriptors y un request,
// envía a traves de todos los sockets la request.
void broadcast(vector<vector<int>> &sockets, struct request* req){
    for (size_t i = 0; i < sockets.size(); i++){
        for (size_t j = 0; j < sockets.size(); j++){
            send_request(sockets[i][j], req);
        }
        
    }
}

// Por siempre, acepta conexiones sobre un socket s en estado listen y 
// agrega los sockets asociados al vector v.
void accept_conns(int s, vector<int>& v, sem_t& clientSem){
	struct sockaddr_in remote;
	int t = sizeof(remote);
	int socket;
	for (size_t i = 0; i < 9;i++){
		if ((socket = accept(s, (struct sockaddr *)&remote, (socklen_t *)&t)) == -1){
			perror("validating incoming connection request");
			exit(1);
		}
		v.push_back(socket);
		sem_post(&clientSem);
	}  
}

// Dado un puerto lsn_port devuelve un socket en estado listen asociado
// a todas las interfaces de red local y a ese puerto (ej 127.0.0.1:lsn_port)
int set_acc_socket(int lsn_port){
	int s;
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		perror("socket");
		exit(1);
	}
	struct sockaddr_in local;

	local.sin_family = AF_INET;
	local.sin_port = htons(lsn_port);
	local.sin_addr.s_addr = INADDR_ANY;

	if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0){
		perror("bind");
		exit(1);
	}

	if (listen(s, 10) == -1){
		perror("listen");
		exit(1);
	}

	return s;
}


// Setea un socket al modo nobloqueante
static int nonblockingsocket(int s){
    int flags;

    if ((flags = fcntl(s, F_GETFL)) == -1) return -1;

    return fcntl(s, F_SETFL, flags | O_NONBLOCK);
}

void send_request(int socket, struct request* req){
    int aux = send(socket, (char *) req , 256 + 10, 0);
    if (aux < 0) perror("sending");

}

int read_socket(int s) {
	int n;
	char str[256];
    n = recv(s, str, 256, 0);
    if (n == 0) return -1;
    if (n < 0) { 
    	perror("receiving");
    	return 0;
    }
    str[n] = '\0'; 
    printf("%s", str);
    return 0;
}

void connection_handler(int dSocket){
    while(1){
        if(read_socket(dSocket) == -1) break;
    }      
}