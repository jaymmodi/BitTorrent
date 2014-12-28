#include "bt_setup.h"
#include <iostream>

void bt_setup::usage(FILE* file) {
}

void bt_setup::parse_args(bt_args_t* bt_args, int argc, char** argv) {

	int ch; //ch for each flag
	int n_peers = 0;
	int i;
	struct hostent * hostinfo;

	/* set the default args */
	bt_args->verbose = 0; //no verbosity

	//null save_file, log_file and torrent_file
	memset(bt_args->save_file, 0x00, FILE_NAME_MAX);
	memset(bt_args->torrent_file, 0x00, FILE_NAME_MAX);
	memset(bt_args->log_file, 0x00, FILE_NAME_MAX);

	//null out file pointers
	bt_args->f_save = NULL;

	//null bt_info pointer, should be set once torrent file is read
	bt_args->bt_info = NULL;

	//default lag file
	strncpy(bt_args->log_file, "bt-client.log", FILE_NAME_MAX);

	for (i = 0; i < MAX_CONNECTIONS; i++) {
		bt_args->peers[i] = NULL; //initially NULL
	}

	bt_args->id = 0;
	bt_args->pCheck =0;

	while ((ch = getopt(argc, argv, "hp:s:b:l:vI:")) != -1) {
		switch (ch) {
		case 'h':{ //help
			usage(stdout);
			exit(0);
			break;}
		case 'v':{ //verbose
			bt_args->verbose = 1;
			break;}
		case 's':{ //save file
			strncpy(bt_args->save_file, optarg, FILE_NAME_MAX);
			break;}
		case 'l':{ //log file
			strncpy(bt_args->log_file, optarg, FILE_NAME_MAX);
			break;}
		case 'p':{ //peer
			bt_args->pCheck = 1;
			n_peers++;
			//check if we are going to overflow
			if (n_peers > MAX_CONNECTIONS) {
				fprintf(stderr, "ERROR: Can only support %d initial peers",
				MAX_CONNECTIONS);
				usage(stderr);
				exit(1);
			}

			bt_args->peers[n_peers] = new peer_t;

			//parse peers
			__parse_peer(bt_args->peers[n_peers], optarg);
			break;}
		case 'I':{
			bt_args->id = atoi(optarg);
			break;}
		case 'b':{
			char * parse_str1;
			char * word1;
			unsigned short port1;
			char * ip1;
			char sep1[] = ":";
			int i1=0;
						//need to copy becaus strtok mangels things
			parse_str1 = new char[strlen(optarg) + 1];
			strncpy(parse_str1, optarg, strlen(optarg) + 1);

							//only can have 2 tokens max, but may have less
			for (word1 = strtok(parse_str1, sep1), i1 = 0; (word1 && i1 < 3); word1 = strtok(NULL, sep1), i1++) {

							//	printf("Binding On: %d:%s\n", i1, word1);
								switch (i1) {
								case 0: //id
									ip1 = word1;
									break;
								case 1: //ip
									port1 = atoi(word1);
									break;
								default:
									break;
								}
							}

			if (!(hostinfo = gethostbyname(ip1))){
				fprintf(stderr, "ERROR: Invalid host name\n");
				usage(stderr);
				exit(1);
			}

			bt_args->bindToThis.sin_family = hostinfo->h_addrtype;
			bcopy((char *) hostinfo->h_addr,(char *) &(bt_args->bindToThis.sin_addr.s_addr),hostinfo->h_length);
			bt_args->bindToThis.sin_port = htons(port1);
			delete[] parse_str1;
			break;}
		default:{
			fprintf(stderr, "ERROR: Unknown option '-%c'\n",ch);
			usage(stdout);
			exit(1);
		}
		}
	}

	argc -= optind;
	argv += optind;

	if (argc == 0) {
		fprintf(stderr, "ERROR: Require torrent file\n");
		usage(stderr);
		exit(1);
	}

	//copy torrent file over
	strncpy(bt_args->torrent_file, argv[0], FILE_NAME_MAX);

	return;

}

void bt_setup::__parse_peer(peer_t* peer, char* peer_st) {
	char * parse_str;
	bt_lib bt;
	char * word;
	unsigned short port;
	char * ip;
	char id[20];
	char sep[] = ":";
	int i;

	//need to copy becaus strtok mangels things
	parse_str = new char[strlen(peer_st) + 1];
	strncpy(parse_str, peer_st, strlen(peer_st) + 1);

	//only can have 2 tokens max, but may have less
	for (word = strtok(parse_str, sep), i = 0; (word && i < 3);
			word = strtok(NULL, sep), i++) {

		//printf("%d:%s\n", i, word);
		switch (i) {
		case 0: //id
			ip = word;
			break;
		case 1: //ip
			port = atoi(word);
			break;
		default:
			break;
		}

	}

	if (i < 2) {
		fprintf(stderr, "ERROR: Parsing Peer: Not enough values in '%s'\n",
				peer_st);
		usage(stderr);
		exit(1);
	}

	if (word) {
		fprintf(stderr, "ERROR: Parsing Peer: Too many values in '%s'\n",
				peer_st);
		usage(stderr);
		exit(1);
	}

	//calculate the id, value placed in id
	bt.calc_id(ip, port, id);
	//memcpy(&peer->id,&id,20);
	//build the object we need
	bt.init_peer(peer, id, ip, port);
	//free extra memory
	delete[] parse_str;

	return;

}
