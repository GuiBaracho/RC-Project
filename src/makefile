all: player

player: commands process_input connections
	g++ -o player player.cpp commands.o process_input.o connections.o

GS:
	g++ -o GS GS.cpp

commands:
	g++ -o commands.o -c commands.cpp

process_input:
	g++ -o process_input.o -c process_input.cpp

connections:
	g++ -o connections.o -c connections.cpp