#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "network.h"


    
//https://www.raspberrypi.org/forums/viewtopic.php?p=479174
int main (int argc, char *argv[ ]){
	
	printf("master\n");
	printf("master\n");
	printf("master\n");

	int rett = connection_master();

	master_receive_slave_ip();
	printf( "master has slaves ip: '%s'\n", SLAVE_IP_ADDR );

	return 0;
}

