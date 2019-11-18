#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#include "network.h"
//https://www.raspberrypi.org/forums/viewtopic.php?p=479174
// slave should be started first
int main(int argc, char *argv[]){
    printf("slave\n");
    printf("slave\n");
    printf("slave\n");
    int rett = connection_slave();
    slave_send_ip();
    printf( "slave has masters ip: '%s'\n", MASTER_IP_ADDR );
    return 0;
}   