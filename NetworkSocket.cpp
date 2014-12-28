/*
 * NetworkSocket.cpp
 *
 *  Created on: Oct 6, 2014
 *      Author: vivek
 */

#include "NetworkSocket.h"

NetworkSocket::NetworkSocket() {
	// TODO Auto-generated constructor stub

}

NetworkSocket::~NetworkSocket() {
	// TODO Auto-generated destructor stub
}

int NetworkSocket::send_to_peer(peer_t* peer, bt_msg_t* msg) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Socket initialization failed");
		exit(1);
	}
	peer->sockaddr.sin_addr.s_addr = INADDR_ANY;
	int connect_check = connect(sockfd, (struct sockaddr *) &peer->sockaddr,
			sizeof(struct sockaddr_in));
	if (connect_check < 0) {
		fprintf(stderr, "Failed to connect server Check Port/IP\n");
		exit(1);
	}

	char * sendDataForMessage;
	//DO SOMETHING HERE TO SEND REQUIRED DATA
	send(sockfd, sendDataForMessage, 1024, 0);
	close(sockfd);
	return 0;
}

int NetworkSocket::read_from_peer(peer_t* peer, bt_msg_t* msg) {
}
// leecher
int NetworkSocket::send_peer_handshake(peer_t* peer, bt_args_t* bt_args,
		char* msg) {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Socket initialization failed");
		return -1;
	}
	cout << peer->sockaddr.sin_addr.s_addr << endl;
	peer->sockaddr.sin_addr.s_addr = INADDR_ANY;
	cout << peer->port << endl;
	cout << peer->sockaddr.sin_addr.s_addr << endl;
	int connect_check = connect(sockfd, (struct sockaddr *) &peer->sockaddr,
			sizeof(struct sockaddr_in));
	cout << connect_check << endl;
	cout << strerror(errno) << endl;
	if (connect_check < 0) {
		fprintf(stderr, "Failed to connect server Check Port/IP\n");
		return -1;
	}

	//TO SEND REQUIRED DATA(MSG SEND BY CALLIE)
	int sendLeecherCheck = send(sockfd, msg, strlen(msg), 0);
	if (sendLeecherCheck < 0) {
		cout << "send from server error..!!!!";
	}
	char fromSeerder[20];
	int leecherRecvCheck = recv(sockfd, fromSeerder, 20, 0);
	cout << strerror(errno) << endl;
	cout << fromSeerder << endl;
	if (leecherRecvCheck < 0) {
		cout << "message not received from seeder" << endl;
	}
	close(sockfd);
}
//seeder
int NetworkSocket::recv_peer_handshake(peer_t* peer, bt_args_t* bt_args,
		char* msg) {
	struct sockaddr_in client;

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Socket initialization failed");
		return -1;
	}
	cout << "before bind" << endl;
	int bind_check = bind(sockfd, (struct sockaddr *) &bt_args->bindToThis,
			sizeof(struct sockaddr_in));
	if (bind_check == -1) {
		fprintf(stderr, "Bind Process failed..!!");
		return -1;
	}

	int listen_check = listen(sockfd, 5);
	if (listen_check < 0) {
		fprintf(stderr, "Cannot listen on port..Port already in use..!!");
		return -1;
	}
	socklen_t size_of_struct = sizeof(client);
	int client_accept = accept(sockfd, (struct sockaddr *) &client,
			&size_of_struct);
	cout << sizeof(size_of_struct) << endl;
	cout << ntohs(client.sin_port) << endl;
	cout << client.sin_addr.s_addr << endl;
	if (client_accept < 0) {
		fprintf(stderr, "Accept function failed %d\n", client_accept);
		return -1;

	}
	char buf[68];
	char hashToCompare[20];
	buf[68] = '\0';
	int bytes_receieved = 0;

	memset(buf, '\0', sizeof(char) * 68);
	cout << "waiting..!!" << endl;
	if ((bytes_receieved = recv(client_accept, buf, 68, 0)) < 0) {
		fprintf(stderr, "recv() function failed\n");
		return -1;
	}
	buf[bytes_receieved] = '\0';
	cout << buf << endl;
	cout << "no of bytes received  " << bytes_receieved << endl;

	// to check if info hash is the same.

	//compareInfoHash(buf);
	stringstream ss;
	char * infoHash;
	ss << buf;
	cout << ss.str() << endl;
		cout << "In compare" <<endl;
		cout << ss.tellg() << endl;
		ss.seekg(28,ss.beg);
		cout << ss.tellg() << endl;
		ss.read(infoHash,20);

	cout << "in seeder again" << endl;

	char messagefromSeeder[] = "Message from seeder" ;
	int sendFromSeederCheck = send(client_accept, messagefromSeeder,
			strlen(messagefromSeeder), 0);
	cout << strerror(errno) << endl;
	if (sendFromSeederCheck < 0) {
		cout << "error sending message from seeder";
	}
	close(client_accept); // Close the socket connection
	return 1;
}

void NetworkSocket::compareInfoHash(char* handshakeMessage) {
stringstream ss;
char * infoHash;
ss << handshakeMessage;
cout << ss.str() << endl;
	cout << "In compare" <<endl;
	cout << ss.tellg() << endl;
	ss.seekg(28,ss.beg);
	cout << ss.tellg() << endl;
	ss.read(infoHash,20);


return ;
}
