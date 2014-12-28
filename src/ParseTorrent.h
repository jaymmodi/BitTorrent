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
#include <arpa/inet.h>
#include <iomanip>
#include "bt_lib.h"
using namespace std;

class ParseTorrent {
public:
	unsigned char hashOP[20];
	static char handshakeMessage[28];
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
	void getHandshakeMessage(unsigned char * hash, bt_args_t * btargs,
			char* hashval,char * ip,int port);

};

#endif /* PARSETORRENT_H_ */
