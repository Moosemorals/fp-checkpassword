
/* 
 * File:   main.c
 * Author: Osric Wilkinson (osric@fluffypeople.com)
 *
 * Created on 07 November 2017, 06:50
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

int create_socket() {

    int sock;
    struct sockaddr_in name;
    struct hostent *hostinfo;
    const char *hostname = "127.0.0.1";

    /* Create the socket. */
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    name.sin_family = AF_INET;
    name.sin_port = htons(15408);
    hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL) {
        fprintf(stderr, "Unknown host %s.\n", hostname);
        exit(EXIT_FAILURE);
    }
    name.sin_addr = *(struct in_addr *) hostinfo->h_addr;

    if (bind(sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return sock;

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
        exit(EXIT_FAILURE);
    }
    
    len = 0;
    if (getdelim(&passwd, &len, '\0', input) < 0) {
        perror("Can't read password");
        exit(EXIT_FAILURE);
    }
    
    remoteIP = getenv("TCPREMOTEIP");
    
    int socket_fp = create_socket();
    
    FILE *socket = fdopen(socket_fp, "r+");
    
    fprintf(socket, "%s\n%s\n%s", user, passwd, remoteIP == NULL ? "unknown" : remoteIP);
    
    clearenv();
    
    char *env;
    size_t env_len;
    while (getdelim(&env, &env_len, '\n', socket) != -1) {
        putenv(env);
        env_len = 0;
    }

    return (EXIT_SUCCESS);
}

