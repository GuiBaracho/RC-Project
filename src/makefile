all: player

player: commands process_input
	g++ -o player player.cpp commands.o process_input.o

GS:
	g++ -o GS GS.cpp

commands:
	g++ -o commands.o -c commands.cpp

process_input:
	g++ -o process_input.o -c process_input.cpp