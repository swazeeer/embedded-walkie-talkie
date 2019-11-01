#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <ifaddrs.h>

#include "network.h"




int connection_master(){

    struct in_addr localInterface;
    struct sockaddr_in groupSock;
    int sd;

    char databuf[20];
    int datalen = sizeof(databuf);
    get_local_ipaddress( "eth0", databuf);
    printf("MASTER: local address is :   %s\n", databuf);

    /* Create a datagram socket on which to send. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
      perror("Opening datagram socket error");
      return -1;
      //exit(1);
    }
    else
      printf("Opening the datagram socket...OK.\n");

     

    /* Initialize the group sockaddr structure with a */
    /* group address of 225.1.1.1 and port 5555. */

    memset((char *) &groupSock, 0, sizeof(groupSock));
    groupSock.sin_family = AF_INET;
    groupSock.sin_addr.s_addr = inet_addr("239.0.0.0");
    groupSock.sin_port = htons(12345);


    /* Disable loopback so you do not receive your own datagrams.
    {
    char loopch = 0;
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
    {
    perror("Setting IP_MULTICAST_LOOP error");
    close(sd);
    exit(1);
    }
    else
    printf("Disabling the loopback...OK.\n");
    }
    */

     
    /* Set local interface for outbound multicast datagrams. */
    /* The IP address specified must be associated with a local, */
    /* multicast capable interface. */

    localInterface.s_addr = inet_addr(databuf);
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0){
      perror("Setting local interface error");
      return -1;
      //exit(1);
    }
    else
      printf("Setting the local interface...OK\n");

    /* Send a message to the multicast group specified by the*/
    /* groupSock sockaddr structure. */
    /*int datalen = 1024;*/

    if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0){
        perror("Sending datagram message error");
        return -1; // error
        }
    else
      printf("Sending datagram message...OK\n");

     

    /* Try the re-read from the socket if the loopback is not disable
    if(read(sd, databuf, datalen) < 0)
    {
    perror("Reading datagram message error\n");
    close(sd);
    exit(1);
    }
    else
    {
    printf("Reading datagram message from client...OK\n");
    printf("The message is: %s\n", databuf);
    }
    */
   return 1 ; // success

}


int connection_slave(){
    struct sockaddr_in localSock;
    int sd;
    struct ip_mreq group;
    int datalen;
    char databuf[1024];


    /* Create a datagram socket on which to receive. */
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("Opening datagram socket error");
        return -1;
        //exit(1);
    }
    else
        printf("Opening datagram socket....OK.\n");

    /* Enable SO_REUSEADDR to allow multiple instances of this */
    /* application to receive copies of the multicast datagrams. */
    {

    int reuse = 1;

    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0){
        perror("Setting SO_REUSEADDR error");
        close(sd);
        return -1;
        //exit(1);
    }
    else
        printf("Setting SO_REUSEADDR...OK.\n");
    }
     

    /* Bind to the proper port number with the IP address */
    /* specified as INADDR_ANY. */

    memset((char *) &localSock, 0, sizeof(localSock));
    localSock.sin_family = AF_INET;
    localSock.sin_port = htons(12345);
    localSock.sin_addr.s_addr = INADDR_ANY; // SHOULDnt be binding ot all interfaces
    // might cause problems on the beagle bone           //https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming?noredirect=1&lq=1

    if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock))){
        perror("Binding datagram socket error");
        close(sd);
        return -1;
        //exit(1);
    }
    else
        printf("Binding datagram socket...OK.\n");

     
    /* Join the multicast group x,x,x,x on the local */
    /* interface. Note that this IP_ADD_MEMBERSHIP option must be*/
    /* called for each local interface over which the multicast */
    /* datagrams are to be received. */


    //char* ubuntu = "192.168.1.75";
    //char* windows = "192.168.1.68";
    //char* local_addr = "00.00.00.00";
    char local_addr[20];
    get_local_ipaddress("eth0", local_addr);
    
    printf("SLAVE: local address is :   %s\n", local_addr);


    group.imr_multiaddr.s_addr = inet_addr("239.0.0.0");
    group.imr_interface.s_addr = inet_addr(local_addr);


    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
        perror("Adding multicast group error");
        close(sd);
        return -1;
        //exit(1);
    }
    else
        printf("Adding multicast group...OK.\n");

    /* Read from the socket. */
    datalen = sizeof(databuf);

    if(read(sd, databuf, datalen) < 0){
        perror("Reading datagram message error");
        close(sd);
        return -1;
        //exit(1);
    }
    else{
        printf("Reading datagram message...OK.\n");
        printf("The message from multicast server is: \"%s\"\n", databuf);
    }

    return 1; // success
}

/*
create socket
send 
*/
int slave_send_ip(){

}


/*
return 0 meains failure
return 1 means success
*/
void get_local_ipaddress(char* interface_name, char* IP_address){
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    int max_address_lenth = 20;
    char host[max_address_lenth];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }


    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;  

        s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in) ,IP_address, max_address_lenth, NULL, 0, NI_NUMERICHOST);

        if( (strcmp(ifa->ifa_name,"wifi0") == 0 ) && 
            (ifa->ifa_addr->sa_family      == AF_INET) ){

            if (s != 0){

                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            printf("\tInterface : <%s>\n",ifa->ifa_name );
            printf("\t IP Address : <%s>\n", IP_address); 
           // sprintf(IP_address, host); // return address
           // *IP_address = &host;
        }
    }

    freeifaddrs(ifaddr);
}


