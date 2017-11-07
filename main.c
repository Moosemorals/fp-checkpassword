
/* 
 * File:   main.c
 * Author: Osric Wilkinson (osric@fluffypeople.com)
 *
 * Created on 07 November 2017, 06:50
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {
    printf("checkpoint 0\n");
    
    char *user, *passwd;
    
    printf("checkpoint 1\n");
    FILE *input = fdopen(3, "r");
    printf("checkpoint 2\n");
            
    size_t len = 0;        
    getdelim(&user, &len, '\0', input );
    printf("checkpoint 3\n");
    len = 0;
    getdelim(&passwd, &len, '\0', input );
    printf("checkpoint 4\n");
    
    printf("%s\n%s\n", user, passwd);
    
    free(user);
    free(passwd);
    
    return (EXIT_SUCCESS);
}

