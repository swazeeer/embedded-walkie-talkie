#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "network.h"

pthread_t avconv_thrd;
void* run_avconv();

pthread_t avplay_thrd;
void* run_avplay();


// slave should be started first
int main(int argc, char *argv[]){
    
    printf("slave\n");
    printf("slave\n");
    printf("slave\n");
    int rett = connection_slave();

    slave_send_ip();
    printf( "slave has masters ip: '%s'\n", PARTNER_ADDR );

   
    //strncpy(        PARTNER_ADDR , "192.75.241.139",20);

    pthread_create(&avplay_thrd, 0, &run_avplay,0);
    struct timespec sleep = {1, 0}; //1 s,,,
    nanosleep(&sleep, (struct timespec *) 0);  
    pthread_create(&avconv_thrd, 0, &run_avconv,0);
    
   // struct timespec sleep = {50, 0}; 
   // nanosleep(&sleep, (struct timespec *) 0);  

    return 0;
}   
void* run_avplay(){
    char avplay[80];
    strcpy(avplay, "nice -n -20 avplay -nodisp -i rtp://");
    strcpy(avplay, LOCAL_IP_ADDR);
    strcpy(avplay, ":12346  >/dev/null 2>&1");
    
    system(avplay);

    return 0;
}

void* run_avconv(){
    char avconv[80];
    strcpy(avconv, " nice -n -20 avconv  -f alsa -ac 1 -i hw:Talk -acodec mp2 -b 64k  -f rtp rtp://");
    strcpy(avconv, PARTNER_ADDR);
    strcpy(avconv, ":12346  >/dev/null 2>&1"); 
    system(avconv);

    return 0;
}
