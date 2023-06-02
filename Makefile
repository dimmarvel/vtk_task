all:
	g++ -g -Wall main.cpp tlv.cpp app.cpp -o out

build-tests:
	g++ -g -Wall main.cpp tlv.cpp app.cpp -o tests