CC = g++
STANDART = --std=c++11
COMPILER_FLGS = -Werror 
LINKER_FLGS = -lboost_system

ROOT_DIR = ../../

vpath %.hpp $(ROOT_DIR)/DataClasses/
vpath %.hpp $(ROOT_DIR)/Server/

vpath %.cpp $(ROOT_DIR)/DataClasses/
vpath %.cpp $(ROOT_DIR)/Server/

INCL = -I $(ROOT_DIR)/DataClasses/ -I $(ROOT_DIR)/Server/

all: server

server: server_main.o Server.o Connection.o Stanza.o
	$(CC) $(STANDART) -o $@ $^ $(LINKER_FLGS)

Server.o: Server.cpp Server.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

Connection.o: Connection.cpp Server.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

server_main.o: server_main.cpp Server.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

Stanza.o: Stanza.cpp Stanza.hpp JID.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

clean:
	rm -f *.o
	rm -f server

