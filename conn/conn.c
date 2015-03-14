#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

#include "config.h"

// GLOBAL 
int PROT; 

void init();
void destroy();
int bindAddress();

void *httpHandler(void *);
void *httpsHandler(void *);

int isPicture(char *b);
int isGet(char *b);

int main(int argc, char **argv){
    
    int status;
    pid_t http_proc, https_proc, ret;

    init();

    http_proc = https_proc = -1;

reset:
    /* Daemonize the HTTP process */
    if(http_proc == -1){
        PROT = PROT_HTTP; 
        http_proc = fork();
        if(http_proc == (pid_t)-1){
            DEBUG("Could not create another process");
            return 1;
        }
        if(!http_proc){ // daemonize child
            printf("Starting HTTP Daemon %d.\n", getpid());
            if(setsid()==-1){
                DEBUG("Could not daemonize");
                return 1;
            }else{
                printf("[%d] inside HTTP Daemon.\n", getpid());
                if(bindAddress())
                    exit(1);
                exit(0);
            }
        }
    }
    
    /* Daemonize the HTTPS process */
    if(https_proc == -1){
        PROT = PROT_HTTPS; 
        https_proc = fork();
        if(https_proc == (pid_t)-1){
            DEBUG("Could not create another process");
            kill(http_proc, SIGKILL); // get rid of other process
            if( (waitpid(http_proc, NULL, 0)) < 0 ){
                DEBUG("Could not wait for child");
                return 2;
            }
            return 1;
        }
        if(!https_proc){
            printf("Starting HTTPS Daemon %d.\n", getpid());
            if(setsid()==-1){
                DEBUG("Could not daemonize");
                return 1;
            }else{
                printf("[%d] inside HTTPS Daemon.\n", getpid());
                if(bindAddress())
                    exit(1);
                exit(0);
            }
        }
    }

    // If it breaks, be sad and try again
    while((ret = waitpid(-1, &status, 0)) < 0){
        sleep(1);
    }

    printf("[%d] Retiring %d\n", getpid(), ret);

    if(ret == http_proc)
        http_proc = -1;
    else
        https_proc = -1;

    sleep(1);
    goto reset;

    destroy();

    return 0;
}

void init(){
     curl_global_init(CURL_GLOBAL_ALL);
}

void destroy(){
    curl_global_cleanup();
}

int bindAddress() {

    int res, bindfd;
    int *fd;

    struct sockaddr_in s;

    void *(*handler)(void *arg) = (PROT == PROT_HTTP) ? httpHandler : httpsHandler;

    if((bindfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        DEBUG("Could not open a socket");
        return -1;
    }
 
    s.sin_family = AF_INET;
    s.sin_port = htons((PROT == PROT_HTTP ? HTTP_BIND_PORT : HTTPS_BIND_PORT));
    inet_aton("127.0.0.1",&s.sin_addr);


    if((bind(bindfd, (const struct sockaddr *)&s, sizeof(const struct sockaddr_in)))){
        DEBUG("Could not bind to port");
        return -1;
    }

    if(listen(bindfd,50) == -1){
        DEBUG("Could not listen..");
        return -1;
    }

    while(1){
        pthread_t pt;
        
        printf("Accepting...\n");
        res = accept(bindfd, NULL, NULL);
        if(res == -1){
            perror("Could not accept");
            continue;
        }

        printf("Accepted!\n");
        fd = (int *)malloc(sizeof(int));
        *fd = res;

        pthread_create(&pt, NULL, handler, (void *) fd);
        pthread_detach(pt);
    }

    printf("Exiting.. \n");

    return 0;
}

void* httpsHandler(void *arg) {
    int sockfd;
    
    sockfd = *((int *)arg);
    
    
    free(arg);
    return 0;
}

void* httpHandler(void *arg) {
    int sockfd;
    char buf[4096];
    char *host;
    file_type ft;

    sockfd = *((int *)arg);

    read(sockfd, buf, 4096*sizeof(char));

    if(!isGet(buf))
        goto done;

    if(!isPicture(buf))
        goto done;

done:
    free(arg);
    return 0;
}


// make sure it is a get request
int isGet(char *b){

    if(b[0] != 'G' || b[1] != 'E' || b[2] != 'T' || b[3] != ' '){
        return 0;
    }

    return 1;
}

int isPicture(char *b){

    return 0;
}
