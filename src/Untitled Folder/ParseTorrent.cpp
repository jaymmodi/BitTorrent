/*
 * ParseTorrent.cpp
 *
 *  Created on: Sep 28, 2014
 *      Author: jay
 */
#include "ParseTorrent.h"

ParseTorrent::ParseTorrent() {
// TODO Auto-generated constructor stub

}

ParseTorrent::~ParseTorrent() {
// TODO Auto-generated destructor stub
}

string* ParseTorrent::decodeDict(char* str1, int index) {
	index++;

	if (str1[index] == 'e') {

		array[0] = index;
		array[1] = "\0";
		return array;
	}
	if (str1[index] == 'l') {
		return decodeList(str1, index++);
	} else if (str1[index] == 'd') {
		return decodeDict(str1, index++);

	} else if (str1[index] == 'i') {
		return decodeInt(str1, index++);

	} else {
		return decodeString(str1, index);

	}

}

string* ParseTorrent::decodeString(char* str1, int index) {
	int startIndex = index; //first position of string
	stringstream ss;

	int colon;
// to find first index of colon
	for (int i = startIndex; i < strlen(str1); i++) {
		if (str1[i] == ':') {
			colon = i;
			break;
		}

	}

	for (int i = startIndex; i < colon; i++) // to get the number
			{
		ss << str1[i];

	}

	int number;
	ss >> number;  //stringstream to int
	colon++;

//get each string
	stringstream value;
	for (int j = (colon); j < (colon + number); j++) {
		value << str1[j];

	}

	ostringstream convert;
	convert << colon + number;
	array[0] = convert.str();
	array[1] = value.str();
	ss.str("");     //clear string stream
	value.str("");
	return array;
}

string* ParseTorrent::decodeInt(char* str1, int index) {
	int Eindex;
	int startIndex = index; // to find first index of 'e'
	for (int i = startIndex; i < strlen(str1); i++) {
		if (str1[i] == 'e') {
			Eindex = i;
			break;
		}

	}

	index++;
	stringstream number;

// to store number
	for (int i = index; i < Eindex; i++) {
		number << str1[i];
	}

	ostringstream convert;
	convert << Eindex + 1;
	array[0] = convert.str();
	array[1] = number.str();
	number.str(""); // clear stringstream

	return array;

}

string* ParseTorrent::decodeList(char* str1, int index) {
	index++;

	if (str1[index] == 'e') {

		array[0] = index;
		array[1] = "\0";
		return array;
	}
	if (str1[index] == 'l') {
		return decodeList(str1, index++);
	} else if (str1[index] == 'd') {
		return decodeDict(str1, index++);

	} else if (str1[index] == 'i') {
		return decodeInt(str1, index++);

	} else {
		return decodeString(str1, index++);
	}
}

map<string, string> ParseTorrent::parseFile(string fileName) {

	map<string, string> infoValues;
	int a = 1; // iterator to store values in map.
	string mapKey, mapValue;
	ifstream infile(fileName.c_str(), ifstream::binary);

// get size of file
	infile.seekg(0, infile.end);
	long fileLength = infile.tellg();
	infile.seekg(0, infile.beg);

// allocate memory for file content
	char ch[fileLength];
	ch[fileLength + 1] = '\0';
// read content of infile
	infile.read(ch, fileLength);
	infile.close();

//cout.write(ch, fileLength) << endl;

	int index = 0;
	string * output;
	do {
		switch (ch[index]) {
		case 'l':
			output = decodeList(ch, index);
			break;
		case 'd':
			output = decodeDict(ch, index);
			break;
		case 'i':
			output = decodeInt(ch, index);
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			output = decodeString(ch, index);
			break;
		default:
			cout << "Invalid BenCoded Meta Info. Please Check File" << endl;
			exit(1);
		}
		index = atoi(output[0].c_str());

		if (a % 2 == 1 && output[1] != "info") {
			mapKey = output[1];
			cout<<mapKey<<" : ";
		}

		if (a % 2 == 0) {
			mapValue = output[1];
			infoValues[mapKey] = mapValue;
			cout<<mapValue<<endl;

		}
		if (output[1] == "info") {
			a--;
		}
		a++;
	} while (ch[index] != 'e' && index < fileLength);

	return infoValues;

}

/*stringstream ParseTorrent::getTorrentFileStream(char* str) {

 return stringstream;
 }*/

bt_info_t ParseTorrent::getInfoStructure(map<string, string> m) {

//char **a;
	bt_info_t btStructure;
	map<int, string> p_hashes;
	map<string, string>::iterator it;
	for (it = m.begin(); it != m.end(); it++) {
		btStructure.name = ((m.find("name"))->second);
		btStructure.piece_length = std::strtol(
				((m.find("piece length"))->second).c_str(), NULL, 10);
		btStructure.length = std::strtol(((m.find("length"))->second).c_str(),
		NULL, 10);
		btStructure.num_pieces = ceil(
				(double) btStructure.length
						/ (double) btStructure.piece_length);
		//char* p = new char[20];
		char** intermediate = new char*[btStructure.num_pieces];

		stringstream value;
		value << ((m.find("pieces"))->second);
		//cout << value.str() << endl;
		char* p = new char[btStructure.num_pieces*20];
		for (int i = 0; i < btStructure.num_pieces; i++) {

			memset(p, '\0', 20);
			value.read(p, 20);
			intermediate[i] = &p[i];
		}
		btStructure.piece_hashes = (char**) intermediate;
		delete[] p;
		delete[] intermediate;
		return btStructure;

	}

}

unsigned char* ParseTorrent::calInfoHash(char * filename) {

	ifstream infile(filename, ifstream::binary);

	//char * sp;

	// get size of file
	infile.seekg(0, infile.end);
	long fileLength = infile.tellg();
	infile.seekg(0, infile.beg);

	// allocate memory for file content
	char ch[fileLength];
	ch[fileLength + 1] = '\0';
	// read content of infile
	infile.read(ch, fileLength);
	infile.close();

	stringstream s;
	for (int j = 0; j < fileLength; j++) {
		s << ch[j];

	}

	size_t index = s.str().find("info", 0);
	index = index + 4;

	size_t lastIndexOfE = s.str().find_last_of('e', fileLength);
	lastIndexOfE = lastIndexOfE - 1;

	string info_part = s.str().substr(index, lastIndexOfE - index + 1);
	int len = info_part.length();
	//cout << "Before" << len << endl;
	char * infoToChar = new char[len+1];
	infoToChar[len] = '\0';
	int len1;
	len1 = info_part.copy(infoToChar, len, 0);

	SHA1((unsigned char *) infoToChar, strlen(infoToChar),
			(unsigned char*) hashOP);

//return 'i';
	delete[] infoToChar;
	//cout << "hash value returned." << endl;

	return hashOP;

}

void ParseTorrent::getHandshakeMessage(unsigned char * hash, bt_args_t * btargs,char* hashval) {


	bt_lib bt;
	stringstream sample;
	//char * finalHash = new char[68];
	char peer_id_hash[20];
	for (int i = 0; i < 28; i++) {
		sample << handshakeMessage[i];
	}
	//cout << "Handshake msh here" << endl;
	//for (int i = 0; i < 28; i++) {
		//printf("%02x\n", handshakeMessage[i]);

	//}
	sample.seekg(0, sample.end);
	for (int i = 0; i < 20; i++) {
		sample << hash[i];
	}

	//cout << sample.str() << endl;
	/*cout << "info hash plus 19" << endl;
	for (int i = 0; i < 48; i++) {
		printf("%02x\n", sample.str().c_str()[i]);

	}*/

	 int portNo = ntohs(btargs->bindToThis.sin_port);
	 char myIP[16];					// Ip address
	 inet_ntop(AF_INET, &btargs->bindToThis.sin_addr.s_addr, myIP,sizeof(myIP));
	 char id[20];
	 cout << portNo<< endl;
	 bt_lib blib;
	 blib.calc_id(myIP,(unsigned short)portNo,id);

/*	memcpy(&peer_id_hash, &peer->id, 20);
	cout << " last hasj" << endl;
	for (int i = 0; i < 20; i++) {
		printf("%02x\n", peer_id_hash[i]);

	}*/

	sample.seekg(0, sample.end);
	//cout << sample.tellg() << endl;
	//cout << sample.str() << endl;
	for (int j = 0; j < 20; j++) {
		sample << id[j];
	}
	sample.seekg(0, sample.end);
	//cout << sample.tellg() << endl;
	sample.seekg(0, sample.beg);
	sample.read(hashval, 68);
//cout << hashval << endl;
	//cout << "final msg is here..!!!!!!!!!!";
	//cout << finalHash<< endl;

	return;

}
