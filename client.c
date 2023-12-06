#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    struct sockaddr_in address;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    while(1) {
        char command[50];
        printf("Enter command:\n");
        fgets(command, 50, stdin);
        command[strcspn(command, "\n")] = 0;

        send(sock , command , strlen(command) , 0 );
        printf("Command sent\n");

        memset(buffer, 0, sizeof(buffer));
        read(sock , buffer, 1024);
        
        // Print server response
        printf("%s\n",buffer);

        // If the server response is "425 Cannot open data connection.", then break the loop
        if(strcmp(buffer, "425 Cannot open data connection.\n") == 0) {
            break;
        }
    }

    return 0;
}
