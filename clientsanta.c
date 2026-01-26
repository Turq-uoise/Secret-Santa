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
    char names[5][10] = {"Rudolph", "Nick", "Jesus", "Jack", "Will"};
    char buffer[1024];
    char name[20];
    char firstname[10];
    char lastname[10];
    srand(time(NULL));
    strcpy(firstname, names[rand() % ARR_SIZE(names)]);
    strcpy(lastname, names[rand() % ARR_SIZE(names)]);
    strcpy(name, strcat(firstname, lastname));
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
        send(sd, name, sizeof(name), 0);

        // Get giftee from server (tentative)
        send(sd, "Get giftee", 20, 0);

        // Receive giftee info from server
        while((ret = recv(sd, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[ret] = '\0';
            printf("My giftee is %s\n", buffer);
        }
    }
    close(sd);
    return 0;
}