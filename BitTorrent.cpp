#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "bt_lib.h"
#include "ParseTorrent.h"
#include "bt_setup.h"
#include "NetworkSocket.h"

using namespace std;

int main(int argc, char* argv[]) {
	bt_info_t btStruct;
	bt_args_t argStruct;
	bt_setup bt;
	peer_t * peer;
	unsigned char * infoHash;
	NetworkSocket netsock;
	bt.parse_args(&argStruct, argc, argv);

	//to store information in bt_info_t
	ParseTorrent parseTorrent;
	map<string, string> m = parseTorrent.parseFile(argStruct.torrent_file);
	btStruct = parseTorrent.getInfoStructure(m);
	// to calculate info hash
	peer = argStruct.peers[1];
	infoHash = parseTorrent.calInfoHash(argStruct.torrent_file);
	cout << "'info hash here" << endl;
	char *h = parseTorrent.getHandshakeMessage(infoHash, peer);
	if (argStruct.bindCheck == 1) {
		netsock.recv_peer_handshake(peer, &argStruct, h);
	} else {
		netsock.send_peer_handshake(peer, &argStruct, h);
	}
	return 0;

}
