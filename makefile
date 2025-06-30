make:
	g++ -o mygit main.cpp init.cpp hash-object.cpp cat-file.cpp utils.cpp -lssl -lcrypto -lz
	clear