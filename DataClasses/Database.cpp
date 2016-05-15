#include "Database.hpp"

bool Database::OpenDatabase(std::string Name)
{
	char *zErrMsg = 0;
	int  rc;
	std::string sql;
	rc = sqlite3_open("test.db", &db);
   	if( rc ){
		return false;
	}
	
	sql = "CREATE TABLE IF NOT EXISTS USERS( Login TEXT PRIMARY KEY, Password TEXT);"
	      "CREATE TABLE IF NOT EXISTS LOGS(Id INT PRIMARY KEY,Status TINYINT, From TEXT, To TEXT,Body TEXT);";    

	rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
	if( rc != SQLITE_OK ){
		sqlite3_free(zErrMsg);
		return false;
	}
	return true;
};

bool Database::AddUser(std::string login, std::string password)
{
	char *zErrMsg = 0;
	int rc;
	std::string sql;
	
	sql = "INSERT INTO USERS (Login, Password) VALUES ('" + login + "', '" + password + "');";
	
	rc = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
	if (rc != SQLITE_OK){
		return false;
	}
	return true;	
};

bool Database::CheckUser(std::string login, std::string password)
{
	int rc;
	std::string sql;
	std::string posPass;
	sqlite3_stmt *res; 
	
	sql = "SELECT * FROM USERS WHERE Login = '" + login + "';";

	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &res, 0);
	if (rc == SQLITE_OK)
	{
		if(sqlite3_step(res) == SQLITE_ROW){
		 	const char* str = reinterpret_cast<const char*>(sqlite3_column_text(res, 1));
			
			if (str == NULL) return false;
			
			std::string posPass(str);
			return (posPass.compare(password) == 0);	
		}
	}
	return false;
};

bool Database::StoreMSG(Stanza msg)
{
	int rc;
	std::string sql;
	
	sql = "INSERT INTO LOGS (Status, From, To, Body) VALUES(1, '" 
	      + msg.getFrom().getNode() + "', '" 
	      + msg.getTo().getNode() + "', '" 
	      + msg.getMSG() + "');";
	
	rc = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
	if (rc != SQLITE_OK){
		return false;
	}
	return true;
};

std::queue<Stanza> Database::getNew(std::string login)
{
	std::queue<Stanza> msges;
	Stanza msg;
	JID usr;
	int rc;
	std::string sql;
	sqlite3_stmt *res;

	sql = "UPDATE LOGS SET Status = 2 WHERE Id = "
	      "(SELECT * FROM Friends WHERE Status = 1 AND To = '" + login + "' LIMIT 50);";

	rc = sqlite3_exec(db, sql.c_str(), 0, 0, 0);
        if (rc != SQLITE_OK){
                return msges;
        }
	sql = "SELECT * FROM LOGS WHERE To = '" + login + "' AND Status = 2;"
	      "UPDATE LOGS SET Status = 0 WHERE Status = 2;";
	rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &res, 0);
        if (rc == SQLITE_OK) {
                while(sqlite3_step(res) == SQLITE_ROW){
                        msg.setMSG(Stanza::CHAT, std::string(reinterpret_cast<const char*>(sqlite3_column_text(res, 4))));
			
			msg.setStanzaType(Stanza::MESSAGE);
			
			usr.setNode(login);
			msg.setTo(usr);
			
			usr.setNode(reinterpret_cast<const char*>(sqlite3_column_text(res, 2)));
			msg.setFrom(usr);

			msges.push(msg);
                }
        }
	return msges;
};


std::vector<Stanza> Database::getOld(std::string to, std::string from)
{
        std::vector<Stanza> msges;
        Stanza msg;
        int rc;
        std::string sql;
        sqlite3_stmt *res;
	JID usr;

        sql = "SELECT * FROM LOGS WHERE (To = '" + to + "' OR To = '" + from + "') AND (From = '" + from + "' OR From = '" + to + "')AND Status = 0 ORDER BY Id DESC LIMIT 100;";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &res, 0);
        if (rc == SQLITE_OK) {
                while(sqlite3_step(res) == SQLITE_ROW){
                        msg.setMSG(Stanza::CHAT, std::string(reinterpret_cast<const char*>(sqlite3_column_text(res, 4))));

                        msg.setStanzaType(Stanza::MESSAGE);

                        usr.setNode(reinterpret_cast<const char*>(sqlite3_column_text(res, 3)));
                        msg.setTo(usr);

                        usr.setNode(reinterpret_cast<const char*>(sqlite3_column_text(res, 2)));
                        msg.setFrom(usr);

                        msges.push_back(msg);
                }
        }
        return msges;
};

void Database::CloseDataBase()
{
	sqlite3_exec(db, "END", 0, 0, 0);
	sqlite3_close(db);
};
