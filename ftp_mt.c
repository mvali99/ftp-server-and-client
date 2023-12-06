#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PORT 8080

typedef struct {
    char username[50];
    char password[50];
    int download_volume;
} User;

User users[3] = {
    {"alice", "P@ssw0rd1", 1000},
    {"bob", "Secret123", 2000},
    {"Charlie", "Pa$$w0rd!", 3000}
};

FILE *log_file;

void write_log(char* message) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(log_file, "%d-%02d-%02d %02d:%02d:%02d %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, message);
}

int authenticate(char* username, char* password) {
    for(int i = 0; i < 3; i++) {
        if(strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            char log_message[100];
            sprintf(log_message, "User %s logged in.", username);
            write_log(log_message);
            return i;
        }
    }
    return -1;
}

void list_files() {
    struct dirent *de; 

    DIR *dr = opendir("."); 

    if (dr == NULL)  
    {
        printf("Could not open current directory" ); 
        return; 
    } 

    while ((de = readdir(dr)) != NULL) 
            printf("%s\n", de->d_name); 

    closedir(dr);     
}

void download_file(int user_index, char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }

    fseek(file, 0L, SEEK_END);
    int file_size = ftell(file);
    fclose(file);

    if (file_size > users[user_index].download_volume) {
        printf("425 Cannot open data connection.\n");
        return;
    }

    users[user_index].download_volume -= file_size;

    char log_message[100];
    write_log(log_message);
}

void* handle_client(void* new_socket_ptr) {
    int new_socket = *((int*)new_socket_ptr);
    char buffer[1024] = {0};

    while(1) {
        read(new_socket, buffer, 1024);

        char* command = strtok(buffer, " ");
        char* username = strtok(NULL, " ");
        char* password = strtok(NULL, " ");

        int user_index = authenticate(username, password);

        if(user_index != -1) {
            if(strcmp(command, "PWD") == 0) {
                char response[1024];
                sprintf(response, "257 \"/home/%s\" is the current directory\n", username);
                send(new_socket, response, strlen(response), 0);
            } else if(strcmp(command, "LS") == 0) {
                list_files();
            } else if(strcmp(command, "QUIT") == 0) {
                printf("221 Goodbye!\n");
                break;
            } else if(strcmp(command, "PORT") == 0) {
                printf("500 Illegal PORT command.\n");
            } else if(strcmp(command, "DOWNLOAD") == 0) {
                char* filename = strtok(NULL, " ");
                download_file(user_index, filename);
            } else {
                send(new_socket , "501 Syntax error in parameters or arguments.\n" , strlen("501 Syntax error in parameters or arguments.\n") , 0 );
            }
        } else {
            send(new_socket , "530 Login incorrect.\n" , strlen("530 Login incorrect.\n") , 0 );
        }

        memset(buffer, 0, sizeof(buffer));
    }

    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Bind the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Put the server socket in a passive mode
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    log_file = fopen("server_log.txt", "a");
    if (log_file == NULL) {
        printf("Could not open log file.\n");
        return -1;
    }

    while(1) {
        int new_socket;

        // Extract the first connection request on the queue of pending connections and create a new connected socket
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, &new_socket);

        write_log("New client connected.");
    }

    fclose(log_file);

    return 0;
}
