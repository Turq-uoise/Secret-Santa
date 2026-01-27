#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX 100
#define NAME_LEN 50

char participants[MAX][NAME_LEN];
char giftees[MAX][NAME_LEN];
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
        if (strcmp(participants[i], name) == 0) {
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
        strcpy(giftees[i], participants[order[i]]);
    }

    draw_done = 1;
}

int main() {
    srand(getpid());

    int server_sd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4242);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_sd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sd, 10);

    printf("Secret Santa Server running...\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_sd = accept(server_sd, (struct sockaddr*)&client_addr, &client_len);
        printf("Client connected.\n");

        pid_t pid = fork();

        if (pid == 0) {
            close(server_sd);

            char buffer[256];
            memset(buffer, 0, sizeof(buffer));
            recv(client_sd, buffer, sizeof(buffer), 0);

            printf("Received: %s\n", buffer);

            // REGISTER <name>
            if (strncmp(buffer, "REGISTER ", 9) == 0) {
                printf("Participant registered.\n");
                if (draw_done) {
                    
                    send(client_sd, "DRAW_ALREADY_DONE\n", 18, 0);
                } else {
                    char *name = buffer + 9;

                    if (find_participant(name) != -1) {
                        send(client_sd, "NAME_EXISTS\n", 12, 0);
                    } else {
                        strcpy(participants[count], name);
                        count++;
                        
                        send(client_sd, giftees, 50, 0);
                        // send(client_sd, "REGISTERED\n", 11, 0);
                        
                    }
                }
            }

            
            else if (strcmp(buffer, "DRAW") == 0) {
                if (draw_done) {
                    send(client_sd, "DRAW_ALREADY_DONE\n", 18, 0);
                } else if (count < 2) {
                    send(client_sd, "NOT_ENOUGH_PARTICIPANTS\n", 24, 0);
                } else {
                    do_draw();
                    send(client_sd, "DRAW_COMPLETE\n", 14, 0);
                }
            }

            // GET <name>
            else if (strncmp(buffer, "GET ", 4) == 0) {
                if (!draw_done) {
                    send(client_sd, "DRAW_NOT_DONE\n", 14, 0);
                } else {
                    char *name = buffer + 4;
                    int idx = find_participant(name);

                    if (idx == -1) {
                        send(client_sd, "UNKNOWN_NAME\n", 13, 0);
                    } else {
                        char reply[256];
                        sprintf(reply, "GIFTEE %s\n", giftees[idx]);
                        send(client_sd, reply, strlen(reply), 0);
                    }
                }
            }

            else {
                send(client_sd, "UNKNOWN_COMMAND\n", 16, 0);
            }

            close(client_sd);
            exit(0);
        }

        close(client_sd);
    }

    close(server_sd);
    return 0;
}
