#include "header.h"

void createClients(int numero){
    std::string comando = "./client ";
    comando += std::to_string(numero);
    system(comando.c_str());
}

int main(int argc, char const *argv[])
{
    srand(time(0));
    vector <thread> threads;
    for(size_t i = 0; i < 9; i++){
        threads.push_back(thread(createClients, (rand()%5000) + 1025));
    }

    for(unsigned int i = 0; i < 9; i++){
		threads[i].join();
	}

    return 0;
}
