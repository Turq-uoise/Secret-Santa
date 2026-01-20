#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdbool.h>



struct participant{
    char name[50];
    char giftee[50];
};



int main() {
    int server_sd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    bind(server_sd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sd, 10);

    while (1) {
        struct participant p;
        int client_sd = accept(server_sd, (struct sockaddr*)&client_addr, &client_len);
        recv(client_sd, &p, sizeof(p), 0);
        int send_p_giftee = send(client_sd, p.giftee, strlen(p.giftee) +1, 0);
    };



    struct participant p;
    struct participant p2;
    struct participant participants[5];
    bool draw_done = false;
    int count = 0;

    printf("Received name: %s\n", p.name);
    printf("Received giftee: %s\n", p.giftee);

    for (int i = 0; i < sizeof(p.name); i++){
        for (int i = 0; i < sizeof(p2); i++){

            if (strlen(p.giftee) > 0){
                continue;
                }

                else{
                    strcpy(p.giftee, p2.name);
                }
            
            if (strcmp(p.name, p2.name) == 0){
                continue;
            }
        }

    }

    //int send_p_giftee = send(client_sd, p.giftee, strlen(p.giftee) +1, 0);


}


    close(client_sd);
    close(server_sd);
