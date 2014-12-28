/*
 * ParseTorrent.h
 *
 *  Created on: Sep 28, 2014
 *      Author: jay
 */

#ifndef PARSETORRENT_H_
#define PARSETORRENT_H_
#include <iostream>
#include <istream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <fstream>
#include <openssl/sha.h> //hashing pieces
#include <map>
#include <cmath>
#include <iomanip>
#include "bt_lib.h"
using namespace std;

class ParseTorrent {
public:
	unsigned char hashOP[20];
    char handshakeMessage[28] ={0x13,0x42,0x69,0x74,0x54,0x6f,0x72,0x72,0x65,0x6E,0x74,0x20,0x50,0x72,0x6F,0x74,0x6F,0x63,0x6F,0x6c,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30};
	string array[2]; // output of each decode function
	ParseTorrent();
	virtual ~ParseTorrent();
	string * decodeDict(char * str1, int index);
	string * decodeString(char * str1, int index);
	string * decodeInt(char * str1, int index);
	string * decodeList(char * str1, int index);
	map<string, string> parseFile(string fileName);
	stringstream getTorrentFileStream(char * str);
	bt_info_t getInfoStructure(map<string, string> m);
	unsigned char * calInfoHash(char * filename);
	 char * getHandshakeMessage(unsigned char * hash,peer_t * peer);

};

#endif /* PARSETORRENT_H_ */
