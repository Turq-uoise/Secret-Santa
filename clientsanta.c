#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define ARR_SIZE(arr) ( sizeof((arr)) / sizeof((arr[0])) )

int main(void){
    // Get random name
    char giftee[24];
    char reg[20] = "REGISTER ";
    char draw[5] = "DRAW";
    char getter[5] = "GET ";
    char names[5][10] = {"Rudolph", "Nick", "Jesus", "Jack", "Will"};
    char buffer[1024];
    char name[20];
    char firstname[10];
    char lastname[10];
    srand(time(NULL));
    int rand_name_index = rand() % ARR_SIZE(names);
    printf("rand_name_index is %d.\n", rand_name_index);
    sprintf(firstname, "%s_%d", names[rand_name_index], rand_name_index);
    printf("Firstname is %s.\n", firstname);
    strcpy(lastname, names[rand() % ARR_SIZE(names)]);
    sprintf(name, "%s %s", firstname, lastname);
    printf("My name is %s.\n", name);
    
    // Set up socket and connect
    int ret, sd;
    struct sockaddr_in my_addr;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(4242);
    inet_pton(AF_INET, "127.0.0.1", &my_addr.sin_addr);
    ret = connect(sd, (struct sockaddr *)&my_addr, sizeof(my_addr));
    
    // Check connection
    if(ret == 0) {
        // Send name to server
        char registry[100];

        sprintf(registry, "%s%s", reg, name);
 
        printf("%s\n", registry);
        send(sd, registry, sizeof(registry), 0);


        //start draw
        // send(sd, "DRAW", 4, 0);
        
        //Get giftee from server (tentative) 

        // Receive giftee info from server
        while((ret = recv(sd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[ret] = '\0';
            if (strncmp(buffer, "DRAWN", strlen("DRAWN")) == 0){
                printf("Draw complete.\n");
                sprintf(giftee, "GET %s", name);
                send(sd, giftee, sizeof(giftee), 0);
                // GET {OWN NAME}
            }
            else if (strncmp(buffer, "REGISTERED", strlen("REGISTERED")) == 0){
                printf("%s has registered with the server.\n", name);
                continue;
            }
            else if (strncmp(buffer, "UNKNOWN COMMAND", strlen("UNKNOWN COMMAND")) == 0){
                printf("Unknown Command sent to server.\n");
                continue;
            }
            else {
                printf("My (%s) giftee is %s\n", name, buffer);
            }
        }
    }
    close(sd);
    return 0;
}