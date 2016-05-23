CC = g++
STANDART = --std=c++11
COMPILER_FLGS = -Werror 
LINKER_FLGS = -lboost_system -lssl -lcrypto -lsqlite3

ROOT_DIR = ../../

vpath %.hpp $(ROOT_DIR)/DataClasses/
vpath %.hpp $(ROOT_DIR)/Server/

vpath %.cpp $(ROOT_DIR)/DataClasses/
vpath %.cpp $(ROOT_DIR)/Server/

INCL = -I $(ROOT_DIR)/DataClasses/ -I $(ROOT_DIR)/Server/

all: server

server: server_main.o Server.o Connection.o Stanza.o Database.o error_codes.o
	$(CC) $(STANDART) -o $@ $^ $(LINKER_FLGS)

Server.o: Server.cpp Server.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

Connection.o: Connection.cpp Server.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

server_main.o: server_main.cpp Server.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

Stanza.o: Stanza.cpp Stanza.hpp JID.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

Database.o: Database.cpp Database.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

error_codes.o: error_codes.cpp error_codes.hpp
	$(CC) $(STANDART) $(COMPILER_FLGS) -c $< $(INCL)

clean:
	rm -f *.o
	rm -f server

