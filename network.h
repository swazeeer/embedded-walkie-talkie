#ifndef NETWORK_H_
#define _NETWORK_H_



int connection_master();
int connection_slave();

int slave_send_ip();
int master_receive_slave_ip();

void get_local_ipaddress(char* interface_name, char* IP_address);


#endif