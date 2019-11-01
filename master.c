#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include "network.h"


    

int main (int argc, char *argv[ ]){
	
	printf("master\n");
	printf("master\n");
	printf("master\n");
	int rett = connection_master();
	return 0;
}