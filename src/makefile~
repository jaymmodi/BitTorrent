bt :	bt_lib.o bt_setup.o ParseTorrent.o BitTorrent.cpp	
		g++ -g -std=c++11 bt_lib.o bt_setup.o ParseTorrent.o BitTorrent.cpp -lssl -lcrypto -o bt

ParseTorrent.o:	ParseTorrent.cpp
		g++ -g -std=c++11 -c ParseTorrent.cpp -o ParseTorrent.o

bt_lib.o: bt_lib.cpp
		g++ -g -std=c++11 -c bt_lib.cpp -o bt_lib.o

bt_setup.o: bt_setup.cpp
		g++ -g -std=c++11 -c bt_setup.cpp -o bt_setup.o

clean:
	rm *.o bt















