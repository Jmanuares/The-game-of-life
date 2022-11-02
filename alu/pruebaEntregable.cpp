#include "header.h"

void client(int aux, int serverPort){
    std::string bash = "./cell ";
    bash += std::to_string(aux);
    bash += " ";
    bash += std::to_string(serverPort);
    system(bash.c_str());
}

void clients(vector<thread> &threads, int clients, int serverPort){
    for (size_t i = 0; i < clients; i++)
        threads.push_back(thread(client, (rand()%(serverPort*2)) + serverPort, serverPort));
}

void server(int port){
    std::string bash = "./board ";
    bash += std::to_string(port);
    system(bash.c_str());
}

int main(int argc, char const *argv[]){
    srand(time(0));
    vector <thread> threads;
    threads.push_back(thread(server, atoi(argv[1])));
    
    threads.push_back(thread(clients, ref(threads), 9, atoi(argv[1])));
    
    for (unsigned int i = 0; i < threads.size(); i++)
		threads[i].join();

    return 0;
}
