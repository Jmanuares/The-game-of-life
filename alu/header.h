#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <iostream>
#include <sys/sendfile.h>
#include <vector>
#include <semaphore.h>
#include <mutex>
#include <sys/wait.h>
#include <cassert>
#include <time.h>    
#include <sstream>
#define PORT 5031

using namespace std;

// Ejemplo sencillo. 
// Recordar que es posible definir clases
// También se pueden definir funciones dentro del struct

struct request{
	char type[10];
	char msg[256];
};

// OPCIONAL
struct client_request{
	// TO DO
};

// Funciones utiles sugeridas
int read_sock(char str[], int s);
void broadcast(vector<vector<int>> &sockets, struct request *req);
void get_request(struct request* req, int s);
void send_request(int s, struct request *req);
int set_acc_socket(int lsn_port);
void accept_conns(int s, vector<int> &v);
int read_socket(int s);
void connection_handler(int dSocket);


