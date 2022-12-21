all: player GS

player: commands process_input connections
	g++ -g -o player player.cpp commands.o process_input.o connections.o

GS: udp_GS tcp_GS
	g++ -g -o GS GS.cpp udp_GS.o tcp_GS.o connections.o

udp_GS:
	g++ -o udp_GS.o -c udp_GS.cpp

tcp_GS:
	g++ -o tcp_GS.o -c tcp_GS.cpp

commands:
	g++ -o commands.o -c commands.cpp

process_input:
	g++ -o process_input.o -c process_input.cpp

connections:
	g++ -o connections.o -c connections.cpp