/*
 * NetworkSocket.h
 *
 *  Created on: Oct 6, 2014
 *      Author: vivek
 */


#ifndef NETWORKSOCKET_H_
#define NETWORKSOCKET_H_

#include "bt_lib.h"
#include <string.h>
#include <iostream>
#include <errno.h>
#include <sstream>
#include <stdint.h>

class NetworkSocket {
public:
	NetworkSocket();
	virtual ~NetworkSocket();

	/*send a msg to a peer*/
	int send_to_peer(peer_t * peer, bt_msg_t * msg);

	/*read a msg from a peer and store it in msg*/
	int read_from_peer(peer_t * peer, bt_msg_t *msg);

	int send_peer_handshake(peer_t * peer, bt_args_t* bt_args,char * msg);

	int recv_peer_handshake(peer_t* peer, bt_args_t* bt_args,char * msg);
	void compareInfoHash(char * infohash);
};


#endif /* NETWORKSOCKET_H_ */
