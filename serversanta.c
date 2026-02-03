#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX 100
#define NAME_LEN 50

typedef struct Participant {
    char santa_name[NAME_LEN];
    int santa_sd;
    char giftee[NAME_LEN];
} Participant;

Participant participants[MAX];

// char participants[MAX][NAME_LEN];
// char giftees[MAX][NAME_LEN];
int count = 0;
int draw_done = 0;


void swap(int *a, int *b) { //swaps two ints
    int t = *a;
    *a = *b;
    *b = t;
}

// find participant index
int find_participant(char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(participants[i].santa_name, name) == 0) {
            return i;
        }
    }
    return -1;
}


void do_draw() {
    int order[MAX];
    for (int i = 0; i < count; i++) {
        order[i] = i;
    }

    int valid = 0;
    while (!valid) {
        valid = 1;

        for (int i = 0; i < count; i++) {
            int j = rand() % count;
            swap(&order[i], &order[j]);
        }

        for (int i = 0; i < count; i++) {
            if (order[i] == i) {
                valid = 0;
                break;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        strcpy(participants[i].giftee, participants[order[i]].santa_name);
        printf("%s got %s\n", participants[i].santa_name, participants[i].giftee);
    }

    draw_done = 1;
    for (int i = 0; i < count; i++){
        send(participants[i].santa_sd, "DRAWN", 6, 0);
    }
}

int main() {
    srand(getpid());

    int ret;
    int yes = 1;

    int server_sd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4242);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(server_sd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret != 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    ret = listen(server_sd, 10);
    if (ret != 0) {
        perror("listen");
        return EXIT_FAILURE;
    }

    printf("Secret Santa Server running...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        char buffer[256] = {0};
        memset(buffer, 0, sizeof(buffer));
        
        if (!draw_done){
            int client_sd = accept(server_sd, (struct sockaddr*)&client_addr, &client_len);
            printf("Client connected to socket %d.\n", client_sd);
            recv(client_sd, buffer, sizeof(buffer), 0);
            printf("Received: %s\n", buffer);
            
            if (strncmp(buffer, "REGISTER", strlen("REGISTER")) == 0) {
                printf("Participant registered.\n");
                if (draw_done) {
                    
                    send(client_sd, "DRAW_ALREADY_DONE\n", 18, 0);
                } else {
                    char *name = buffer + strlen("REGISTER ");
                    
                    // If participant name doesnt exist, find_participant returns -1
                    if (find_participant(name) != -1) {
                        send(client_sd, "NAME_EXISTS\n", 12, 0);
                    } else {
                        strcpy(participants[count].santa_name, name);
                        participants[count].santa_sd = client_sd; 
                        count++;
                        
                        // send(client_sd, giftees, 50, 0);
                        send(client_sd, "REGISTERED\n", 11, 0);
                        
                        // START DRAW
                        if (count >= 3){
                            printf("Draw started on server.\n");
                            do_draw();
                        }
                        
                    }
                }
            }
        }

        else {
            // for (int i = 0; i < sizeof(participants); i++){ //sizeofparticipants is TOO BIG (10800) and gets looped to infinity.
            for (size_t i = 0; participants[i].santa_name[0] != '\0'; i++) {
                recv(participants[i].santa_sd, buffer, sizeof(buffer), 0);
                if (strncmp(buffer, "GET ", strlen("GET ")) == 0) {
                    if (!draw_done) {
                        send(participants[i].santa_sd, "DRAW_NOT_DONE\n", 14, 0);
                    } else {
                        char *name = buffer + strlen("GET ");
                        printf("%s asked for giftee\n", name);
                        int idx = find_participant(name);
        
                        if (idx == -1) {
                            send(participants[idx].santa_sd, "UNKNOWN_NAME\n", 13, 0);
                        } else {
                            char reply[256];
                            sprintf(reply, "%s\n", participants[idx].giftee);
                            send(participants[idx].santa_sd, reply, strlen(reply), 0);
                        }
                    }
                }
            }
            close(server_sd);
            break;
        }

        
        //pid_t pid = fork();
        
        //if (pid == 0) {
            //close(server_sd);
            


        // REGISTER <name>

        // DRAW (unused)
        // else if (strncmp(buffer, "DRAW", strlen("DRAW")) == 0) {
        //     if (draw_done) {
        //         send(client_sd, "DRAW_ALREADY_DONE\n", 18, 0);
        //     } else if (count < 2) {
        //         send(client_sd, "NOT_ENOUGH_PARTICIPANTS\n", 24, 0);
        //     } else {
        //         do_draw();
        //         send(client_sd, "DRAW_COMPLETE\n", 14, 0);
        //     }
        // }

        // GET <name>

        // else {
        //     send(client_sd, "UNKNOWN_COMMAND\n", 16, 0);
        // }
        
        //}
    }
    
    close(server_sd);
    return 0;
}