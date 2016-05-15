#ifndef DATABASE_HPP
#define DATABASE_HPP
#include "Stanza.hpp"
#include <sqlite3.h> 
#include <vector>
#include <queue>

class Database
{
	sqlite3 *db = NULL;
public:
	bool OpenDatabase(std::string Name);
	bool AddUser(std::string login, std::string password);
	bool CheckUser(std::string login, std::string password);
	bool StoreMSG(Stanza msg);
	std::queue<Stanza> getNew(std::string login);
	std::vector<Stanza> getOld(std::string to, std::string from);
	void CloseDataBase();
};

#endif
