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
#include <iostream>
#include <string>
#include <ostream>
#include <errno.h>
#include <openssl/sha.h> //hashing pieces
#include <algorithm>
#include <sys/select.h>
#include <sys/fcntl.h>

#include "bt_lib.h"
#include "ParseTorrent.h"

int bt_lib::init_peer(peer_t* peer, char* id, char* ip, unsigned short port) {

	struct hostent * hostinfo;
	//set the host id and port for referece
	memcpy(peer->id, id, ID_SIZE);
	peer->port = port;

	//get the host by name
	if ((hostinfo = gethostbyname(ip)) == NULL) {
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
//cout << "inside cal ID";
	//format print
	len = snprintf(data, 256, "%s%u", ip, port);

	//id is just the SHA1 of the ip and port string
	SHA1((unsigned char *) data, len, (unsigned char*) id);

	return;
}

void bt_lib::print_peer(peer_t* peer) {
	int i;

	if (peer) {
		printf("peer: %s:%u ", inet_ntoa(peer->sockaddr.sin_addr), peer->port);
		printf("id: ");
		for (i = 0; i < ID_SIZE; i++) {
			printf("%02x", peer->id[i]);
		}
		printf("\n");
	}
}

void bt_lib::leecher(peer_t* peer, bt_args_t* bt_args, char* msg) {

	bt_msg_t btmsg;
	cout << "Inside leecher" << endl;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Socket initialization failed");
		exit(1);
	}
	if (bind(sockfd, (struct sockaddr *) &bt_args->bindToThis,
			sizeof(struct sockaddr_in))) {
		fprintf(stderr, "Bind initialization failed");
		exit(1);
	}

	int connect_check = connect(sockfd, (struct sockaddr *) &peer->sockaddr,
			sizeof(struct sockaddr_in));
	if (connect_check < 0) {
		fprintf(stderr, "Failed to connect server Check Port/IP\n");
		exit(1);
	}
	// to find id and ip of client.
	int port = peer->port;
	// Ip address
	char myIP[16];
	inet_ntop(AF_INET, &peer->sockaddr.sin_addr.s_addr, myIP, sizeof(myIP));

	cout << "Connection Done On IP: " << myIP << " : " << peer->port << endl;
	//TO SEND REQUIRED DATA(MSG SEND BY CALLIE)
	int sendLeecherCheck = send(sockfd, msg, 68, 0);  // send handshake
	if (sendLeecherCheck < 0) {
		cout << "send from server error..!!!!";
		exit(1);
	}
	// receive handshake message from seeder.
	cout << "Handshake Message Sent" << endl;

	char handshakeReceivedFromSeeder[68];
	int leecherRecvCheck = recv(sockfd, handshakeReceivedFromSeeder, 68, 0);
	if (leecherRecvCheck < 0) {
		cout << "message not received from seeder" << endl;
		exit(1);
	}
	cout << "Handshake Message Received" << endl;
	cout << "Handshake Msg Received From Seeder: "
			<< handshakeReceivedFromSeeder << endl;
	cout << "Size Of Handshake Msg Received From Seeder (Bytes): "
			<< leecherRecvCheck << endl;
//compare info hash
	if (!compareInfoHash(handshakeReceivedFromSeeder, bt_args)) {
		cout << "Info Hash In Handshake message was NOT authentic..!!" << endl;
		close(sockfd);
	}

	// compare id hash
	if (!compareIdHash(handshakeReceivedFromSeeder, port, myIP)) {
		cout << "ID Hash in Handshake message was NOT authentic..!!" << endl;
		close(sockfd);
	}

	cout << "     Protocol started  " << endl;

	// prepare request message and have message here in while loop
	char msgFromSeeder[sizeof(btmsg)];
	while (recv(sockfd, msgFromSeeder, 1032, 0) > 0) {
		int length = (int) (msgFromSeeder[0]);
		int messageType = (int) (msgFromSeeder[1]);

		//cout << "In Leecher Receive" << endl;

		switch (messageType) {

		case 5: {
			cout << "Received BITFIELD Preping Interested Message" << endl;
			// send intersted msg :

			char *msgToSeeder = new char[2];
			// prepare interested message
			prepareInterestedMessage(&btmsg, msgToSeeder);
			int sendInterestedMsg = send(sockfd, msgToSeeder,
					sizeof(msgToSeeder), 0);
			if (sendInterestedMsg < 0) {
				cout << "Fault in sending Interested Message" << endl;
				exit(1);
			}
			cout << "Interested Message Sent " << endl;

			char req[2 + 3 * sizeof(int)];

			int length = 1024; // block size
			int begin = 0;
			char *dataFromSeeder = new char[1032];
			char dataToFile[1024];
			FILE * f;
			int loop = 0;
			int numberOfRequests = ceil(
					(float) bt_args->bt_info->piece_length / length);

			int lastBytesInPiece = bt_args->bt_info->length
					% bt_args->bt_info->piece_length;
			int lastBytesInBlock = lastBytesInPiece % length;
			int a = 1;
			if (lastBytesInPiece != 0) {
				a = 0;

			}
			int randomIndex[bt_args->bt_info->num_pieces - 1];
			for (int i = 1; i <= bt_args->bt_info->num_pieces; i++) {

				randomIndex[i - 1] = i;

			}

			random_shuffle(&randomIndex[0],
					&randomIndex[bt_args->bt_info->num_pieces - 1]);
			cout << "Requesting Random Pieces" << endl;
			for (int i = 0; i < bt_args->bt_info->num_pieces; i++) {

				cout << i + 1 << "th Request Piece Index: " << randomIndex[i]
						<< endl;

			}
			fstream out;
			out.open(bt_args->save_file,
					ios::out | ios::in | ios::binary | ios::trunc);
			if (out.fail()) {
				cout << " File with name " << bt_args->save_file
						<< "not present in directory" << endl;
				exit(1);

			}
			string ab = string(bt_args->bt_info->length, '0');
			out << ab;
			int i = 0;
			for (i = 0; i < bt_args->bt_info->num_pieces; i++) {

				int index = randomIndex[i];
				//cout << "Piece No: " << i + 1 << endl;
				if (a == 0 && i == bt_args->bt_info->num_pieces - 1) {
					numberOfRequests = ceil((float) lastBytesInPiece / length);

				}
				for (loop = 0; loop < numberOfRequests; loop++) {

					begin = 1024 * loop;
					// multiples of 1024
					if (bt_args->bt_info->num_pieces == index
							&& loop == (numberOfRequests - 1)) { // logic to send last bytes which are less than block size.

						prepareRequestMsg(msgFromSeeder, req, index, begin,
								lastBytesInBlock);
					} else {
						prepareRequestMsg(msgFromSeeder, req, index, begin,
								length);
					}
					send(sockfd, req, 1032, 0);
					memset(dataFromSeeder, 0, 1032);
					int bytes = recv(sockfd, dataFromSeeder, 1032, 0);
					int indexInFile;
					memcpy(&indexInFile, dataFromSeeder, sizeof(int));
					int offsetInFile;
					memcpy(&offsetInFile, dataFromSeeder + 4, sizeof(int));
					memset(dataToFile, 0, 1024);
					memcpy(dataToFile, (dataFromSeeder + 8), 1024);

					long seek = (indexInFile - 1)
							* bt_args->bt_info->piece_length + offsetInFile;
					out.seekg(seek);
					dataToFile[1024] = '\0';
					if (bt_args->bt_info->num_pieces == index
							&& loop == (numberOfRequests - 1)) {

						length = lastBytesInBlock;

					}
					out.write(dataToFile, length);

					//	storeInFile(dataToFile, indexInFile, offsetInFile, bt_args,f);

					//cout << errno << endl;

				}
				cout << "OuterLoop" << endl;
				//	fclose(f);
			}

			if (i == bt_args->bt_info->num_pieces) {
				string finish = "All Pieces Recieved";
				cout << "All Pieces Recieved" << endl;
				bool value = true;
				prepareHaveMessage(value, msgToSeeder);
				send(sockfd, msgToSeeder, sizeof(msgToSeeder), 0);
			}
			out.close();
			/*
			 if (loop == numberOfRequests) {

			 comparePieceHash();

			 }
			 */

			cout << "perfect" << endl;
			break;

		}
		case 7:
			cout << "Inside piece" << endl;
			char buf[1024];
			while (1) {
				recv(sockfd, buf, 1024, 0);
				FILE * f;
				f = fopen("test.txt", "wb");
				fseek(f, 50, SEEK_SET);
				fputs(buf, f);
				fclose(f);
			}
			break;
		default:
			cout << "some problem" << endl;
			break;

		}

		break;
	}

	close(sockfd);

}

void bt_lib::seeder(peer_t* peer, bt_args_t* bt_args, char* msg) {
	bt_msg_t btm;
	struct sockaddr_in client;
	fd_set master;
	fd_set read_fds;
	int fdmax;
	int client_accept = 0;
	char buf[69];
	buf[68] = '\0';
	int bytes_receieved = 0;
	char myIP[16];

	FD_ZERO(&master);
	FD_ZERO(&read_fds);

	cout << "Inside seeder" << endl;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		fprintf(stderr, "Socket initialization failed");
		exit(1);
	}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	int bind_check = bind(sockfd, (struct sockaddr *) &bt_args->bindToThis,
			sizeof(struct sockaddr_in));
	if (bind_check == -1) {
		fprintf(stderr, "Bind Process failed..!!");
		exit(1);
	}

	int listen_check = listen(sockfd, 5);
	if (listen_check < 0) {
		fprintf(stderr, "Cannot listen on port..Port already in use..!!");
		exit(1);
	}

	FD_SET(sockfd, &master);
	fdmax = sockfd;
	cout << fdmax << endl;

	for (;;) {
		read_fds = master;
		cout << " in select " << endl;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("Select");
			exit(1);
		}

		for (int i = 0; i <= fdmax; i++) {

			if (FD_ISSET(i, &read_fds)) {
				if (i == sockfd) {
					cout << i << endl;
					socklen_t size_of_struct = sizeof(client);
					cout << "before select 1 " << endl;
					client_accept = accept(sockfd, (struct sockaddr *) &client,
							&size_of_struct);
					cout << " in select 1" << endl;
					cout << "client accept done " << client_accept << endl;
					if (client_accept == -1) {

						perror("accept");

					} else {
						FD_SET(client_accept, &master);
						if (client_accept > fdmax) {
							fdmax = client_accept;

							// Ip address

						}
						inet_ntop(AF_INET, &client.sin_addr.s_addr, myIP,
								sizeof(myIP));
						cout << "Connection Accept Port: "
								<< ntohs(client.sin_port) << endl;
						cout << "Connection Accept I/P: " << myIP << endl;
						memset(buf, '\0', sizeof(char) * 69);
						if ((bytes_receieved = recv(client_accept, buf, 68, 0))
								< 0) {
							fprintf(stderr, "recv() function failed\n");
							exit(1);
						}
						/*	if (bytes_receieved == 0) {
						 cout << i << endl;

						 cout << "socket with IP    " << myIP
						 << "and port  "
						 << ntohs(client.sin_port)
						 << " has hung up " << endl;

						 }*/

						//cout << "Handshake Msg Received From Leecher: " << buf << endl;
						//cout << "Size Of Handshake Msg Received From Leecher (Bytes): "	<< bytes_receieved << endl;
						// compare info hash
						compareInfoHash(buf, bt_args);

						// info of server
						int port = ntohs(client.sin_port);
						// Ip address
						char myIPRecv[16];
						inet_ntop(AF_INET, &client.sin_addr.s_addr, myIPRecv,
								sizeof(myIPRecv));

						// compare id hash
						compareIdHash(buf, port, myIPRecv);

						//msg = handshake message
						int sendFromSeederCheck = send(client_accept, msg, 68,
								0);
						if (sendFromSeederCheck < 0) {
							cout << "error sending message from seeder";
							exit(1);
						}

						// to find id and ip of client.
						cout << "Handshake message was authentic" << endl;
						cout << "Handshake Message Sent" << endl;

						cout << "Protocol Starts " << endl;

						//int numberofbytes = recv(client_accept, messageFromLeecher, sizeof(btm), 0);
						char *msgToLeecher = new char[2
								+ bt_args->bt_info->num_pieces];
						memset(msgToLeecher, 0, sizeof(btm));
						prepareBitFieldMessage(&btm, msgToLeecher, bt_args);
						send(client_accept, msgToLeecher, sizeof(msgToLeecher),
								0);
						cout << "Seeder Has Sent BITFIELD Msg" << endl;
						delete[] msgToLeecher;

					}

				} else {

					// Protocol starts

					// message to send protocol messages to leecher
					char messageFromLeecher[1032];

					//sleep(1000);

					int rec_size = recv(i, messageFromLeecher, 1, MSG_PEEK);

					while (rec_size > 0) {
						//cout << errno << endl;
						recv(i, messageFromLeecher, 1032, 0);
						if (messageFromLeecher[0] == 'A') {

							cout << "Data sent Perfectly break" << endl;
							close(i); // bye!
							FD_CLR(i, &master); // remove from master set
							break;

						}

						functionSwitch(messageFromLeecher, bt_args, i);
						break;

					}
					cout << "after while   " << i << endl;
					//	close(i);
				}

			}

		}

	}

	//close(client_accept);
	return;// Close the socket connection
}
int bt_lib::compareInfoHash(char* handshakeMessage, bt_args_t * bt) {
	char msg[69];
	msg[68] = '\0';
	for (int i = 0; i < 68; i++) {
		msg[i] = handshakeMessage[i];
	}
	stringstream ss;
	ParseTorrent parseTorrent;
	char * infoHash = new char[21];
	infoHash[20] = '\0';
	ss << msg;
	ss.seekg(28, ss.beg);
	ss.read(infoHash, 20);
	int a = memcmp(infoHash, parseTorrent.calInfoHash(bt->torrent_file), 20);
	if (a == 0) {
		cout << "Info Hash are equal..!!" << endl;
		return 1;
	} else {
		cout << "Hashes are not equal..!!" << endl;
		return 0;
	}
	delete[] infoHash;

}

int bt_lib::compareIdHash(char * msg, int port, char * ip) {

	// to get id hash from handshake message.
	stringstream ss;
	char * idHash = new char[21];
	idHash[20] = '\0';
	ss << msg;
	ss.seekg(48, ss.beg);
	ss.read(idHash, 20);
	char id[21];
	id[20] = '\0';
	calc_id(ip, (unsigned short) port, id);

	int a = memcmp(idHash, id, 20);
	if (a == 0) {
		cout << "ID Hash are equal..!!" << endl;
		return 1;
	} else {
		cout << "ID Hashes are not equal..!!" << endl;
		return 0;
	}
	delete[] idHash;

}

void bt_lib::prepareInterestedMessage(bt_msg_t* btmsg, char* msg) {

	msg[0] = 1;
	msg[1] = BT_INTERSTED;
	return;
}

void bt_lib::prepareBitFieldMessage(bt_msg_t* bt, char* data,
		bt_args_t* bt_args) {

	// to find no of pieces.
	int numberOfPieces = bt_args->bt_info->num_pieces;
	char c[numberOfPieces];
	for (int i = 0; i < numberOfPieces; i++) {

		c[i] = '1';
	}

	data[0] = (int) (1 + sizeof(c));
	data[1] = BT_BITFILED;
	memcpy(data + 2, c, numberOfPieces);

	return;

}

void prepareRequestMsg(char * data, char*msg, int index, int begin,
		int length) {

	int lengthOfBtField = (int) data[0] - 1; // no. of 1's
	char btfieldStatus[lengthOfBtField];
	memcpy(btfieldStatus, data + 2, lengthOfBtField);

	memset(msg, 0, 14);
	msg[0] = 1 + 3 * (sizeof(int)); // length = msg_type + sizeof(index,begin,length)
	msg[1] = BT_REQUEST;
	memcpy(&msg[2], &index, sizeof(int));
	memcpy(&msg[6], &begin, sizeof(int));
	memcpy(&msg[10], &length, sizeof(int));

}

void sendRequestedData(bt_args_t* bt_args, int index, int begin, int nbytes,
		char *dataTosend) {

	FILE * f;
	stringstream ss;
	ss << bt_args->torrent_file;
	char nameOfFile[FILE_NAME_MAX];
	memset(nameOfFile, '\0', FILE_NAME_MAX);
	ss.read(nameOfFile, (strlen(bt_args->torrent_file) - strlen(".torrent")));
	f = fopen(nameOfFile, "rb");
	if (f == NULL) {
		fprintf(stderr,
				"File Initialization Failed Check File Present in Same Folder as Torrent\n");
		exit(1);
	}
//long int size_of_file;

	if ((begin + nbytes) > (bt_args->bt_info->piece_length)) {
		cout << "Request More Than Piece Resend Request" << endl;
		return;
	} else if (begin > (bt_args->bt_info->piece_length)) {
		cout << "Request More Than Piece Resend Request" << endl;
		return;
	} else if (nbytes > (bt_args->bt_info->piece_length)) {
		cout << "Request More Than Piece Resend Request" << endl;
		return;
	} else {
		long int seep = (((index - 1) * bt_args->bt_info->piece_length))
				+ begin;
		//	fseek(f, seep-1, SEEK_SET);             // point to start , full file mode

		fseek(f, seep, SEEK_SET);
		char abcc[1024];
		memset(abcc, '\0', sizeof(abcc));   //clearing buf
		char ch;
		int i = 0;

		while ((ch = fgetc(f)) != EOF && strlen(abcc) != nbytes) {
			abcc[i] = ch;
			i++;
		}
		memcpy(dataTosend, abcc, 1024);
		//cout << dataTosend << endl;
	}
	fclose(f);

}

void storeInFile(char* dataToFile, int indexInFile, int offsetInFile,
		bt_args_t * bt, FILE* f) {
	if (f != NULL) {
		int seek = (indexInFile - 1) * bt->bt_info->piece_length + offsetInFile;
		fseek(f, seek, SEEK_SET);
		dataToFile[strlen(dataToFile)] = '\0';
		fwrite(dataToFile, sizeof(char), strlen(dataToFile), f);
	} else
		cout << "Please a name of file for the data to be stored." << endl;

}

int compareHashes(bt_args_t* bt, int index, fstream f, char* receivedData) {
	/*int seek = (index - 1) * bt->bt_info->piece_length;
	 //fseek(f, seek, SEEK_SET);
	 char *PieceValue = new char[bt->bt_info->piece_length];
	 memset(PieceValue, '\0', sizeof(PieceValue));   //clearing buf
	 //int bytes = fread(PieceValue,sizeof(char),31690,f);


	 cout << bytes <<  endl;
	 cout<<"PeiceValue: "<<PieceValue[12]<<endl;
	 char Shaval[20];
	 memset(Shaval, '\0', sizeof(Shaval));
	 SHA1((unsigned char *) PieceValue,bt->bt_info->piece_length, (unsigned char*) Shaval);// Pass Correct Value of PieceValue
	 cout<<"allalalalala   "<<Shaval<<endl;*/
	return 0;
}

void functionSwitch(char * messageFromLeecher, bt_args_t*bt, int sockfd) {
	int lengthOfMessage = (int) messageFromLeecher[0];
	//int msgType = (int) messageFromLeecher[1];
	int msgType = (int) (messageFromLeecher[1]);
	cout << "";

	switch (msgType) {

	case 2: {
		cout
				<< "Leecher is INTERESTED in receiving data\nSeeder is willing to Send Piece Msg On Request"
				<< endl;
		//	cout << "Sending BitField Message to Leecher...." << endl;
		break;
	}
	case 6: {
		//cout << " In 6" << endl;
		char * pieceMsg = new char[1032];
		char dataTosend[1024];
		int index;
		int begin;
		int length;
		memcpy(&index, messageFromLeecher + 2, sizeof(int));
		memcpy(&begin, messageFromLeecher + 6, sizeof(int));
		memcpy(&length, messageFromLeecher + 10, sizeof(int));
		sendRequestedData(bt, index, begin, length, dataTosend);
		memset(pieceMsg, 0, 1032);
		memcpy(pieceMsg, &index, sizeof(int));
		memcpy(pieceMsg + 4, &begin, sizeof(int));
		memcpy(pieceMsg + 8, dataTosend, 1024);

		int bytessent = send(sockfd, pieceMsg, 1032, 0);
		delete[] pieceMsg;

		break;
	}
	case 4:
		break;
	default:
		cout << "Wrong Message Type Sent by Leecher." << endl;
		break;

	}

}

void prepareHaveMessage(bool value, char*msg) {

	if (value) {

		msg[0] = 1;
		msg[1] = BT_HAVE;

	} else {

		cout << "Did not receive all pieces" << endl;
	}

}
