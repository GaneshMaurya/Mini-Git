make:
	g++ -o mygit main.cpp init.cpp hash-object.cpp cat-file.cpp write-tree.cpp ls-tree.cpp add.cpp commit.cpp log.cpp checkout.cpp utils.cpp -lssl -lcrypto -lz
	clear