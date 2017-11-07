
/* 
 * File:   main.c
 * Author: Osric Wilkinson (osric@fluffypeople.com)
 *
 * Created on 07 November 2017, 06:50
 */
#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define AUTH_FAIL 1
#define TEMP_ERROR 111

#define PORT 15408
#define LOCALHOST "127.0.0.1"

int create_socket() {
    int sock;
    struct sockaddr_in dest;

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        exit(TEMP_ERROR);
    }

    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(dest.sin_zero, '\0', sizeof dest.sin_zero);

    if (connect(sock, (struct sockaddr*) &dest, sizeof (dest)) != 0) {
        perror("Connect");
        exit(TEMP_ERROR);
    }

    return sock;
}

void trimNewline(char *str) {
    str[strcspn(str, "\n")] = 0;  
}

void readEnvironment(FILE *socket) {
    char *env;
    size_t env_len = 0;
    while (getline(&env, &env_len, socket) != -1) {
        char *name, *value;
        
        trimNewline(env);
        
        // Split env on equals.        
        name = env;        
        size_t eq = strcspn(env, "=");
        name[eq] = 0;        
        value = env + (eq + 1);        
        
        if (setenv(name, value, 1) == -1) {
            perror("Can't set environment");
            exit(TEMP_ERROR);
        }
        
        free(env);
        env_len = 0;
    }
}

/*
 * 
 */
int main(int argc, char** argv) {    
    char *user, *passwd, *remoteIP;

    FILE *input = fdopen(3, "r");

    size_t len = 0;
    if (getdelim(&user, &len, '\0', input) < 0) {
        perror("Can't read username");
        exit(TEMP_ERROR);
    }

    len = 0;
    if (getdelim(&passwd, &len, '\0', input) < 0) {
        perror("Can't read password");
        exit(TEMP_ERROR);
    }

    remoteIP = getenv("TCPREMOTEIP");

    int socket_fp = create_socket();
    
    FILE *socket = fdopen(socket_fp, "r+");

    // Write to socket
    fprintf(socket, "%s\n%s\n%s\n", user, passwd, remoteIP == NULL ? "unknown" : remoteIP);
    fflush(socket);
    
    // Check for success    
    char *result;
    size_t result_len = 0;
    if (getline(&result, &result_len, socket) == -1) {
        perror("getline");
        exit(TEMP_ERROR);
    }
    
    trimNewline(result);
    
    if (strcmp("success", result) == 0) {
        clearenv();    
        readEnvironment(socket);
            
        if (execl(argv[1], argv[1], NULL) == -1) {
            perror("execl");
            exit(TEMP_ERROR);
        }
        // end of process
    } else {
        return (AUTH_FAIL);
    }
}

