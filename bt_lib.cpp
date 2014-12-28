#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h> //internet address library
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sstream>
#include <sys/stat.h>
#include <arpa/inet.h>

#include <openssl/sha.h> //hashing pieces

#include "bt_lib.h"


int bt_lib::init_peer(peer_t* peer, char* id, char* ip, unsigned short port) {

	  struct hostent * hostinfo;
	  //set the host id and port for referece
	  memcpy(peer->id, id, ID_SIZE);
	  peer->port = port;

	  //get the host by name
	  if((hostinfo = gethostbyname(ip)) ==  NULL){
	    perror("gethostbyname failure, no such host?");
	    herror("gethostbyname");
	    exit(1);
	  }

	  //zero out the sock address
	  bzero(&(peer->sockaddr), sizeof(peer->sockaddr));

	  //set the family to AF_INET, i.e., Iternet Addressing
	  peer->sockaddr.sin_family = AF_INET;

	  //copy the address to the right place
	  bcopy((char *) (hostinfo->h_addr),
	        (char *) &(peer->sockaddr.sin_addr.s_addr),
	        hostinfo->h_length);

	  //encode the port
	  peer->sockaddr.sin_port = htons(port);

	  return 0;
}

void bt_lib::calc_id(char* ip, unsigned short port, char* id) {
	char data[256];
	  int len;

	  //format print
	  len = snprintf(data,256,"%s%u",ip,port);

	  //id is just the SHA1 of the ip and port string
	  SHA1((unsigned char *) data, len,(unsigned char*)id);
	 /* stringstream ss;
	  for(int i =0;i<20;i++)
	  {
		  ss << id[i];
		  printf("%02x\n",id[i]);
	  }*/



	  return;
}

void bt_lib::print_peer(peer_t* peer) {
	int i;

	  if(peer){
	    printf("peer: %s:%u ",
	           inet_ntoa(peer->sockaddr.sin_addr),
	           peer->port);
	    printf("id: ");
	    for(i=0;i<ID_SIZE;i++){
	      printf("%02x",peer->id[i]);
	    }
	    printf("\n");
	  }
}
