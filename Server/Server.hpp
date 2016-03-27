#ifndef SERVER_HPP
#define SERVER_HPP

//STL containers
#include <queue>
#include <map>
#include <vector>
#include <string>
//Boost Asio
#include <boost/asio.hpp>
//Local libraries
#include <Stanza.hpp>

class Server
{
    private:
        enum Status
        {
            AVAILABLE = 0,
            UNAVAILABLE,

            MAX_SERVER_STATUS
        }
        Status status;
        std::map<std::string, Connection> connections;
        std::map<std::string, Room> rooms;
    public:
        Server();
        Server( const Server& obj );
        ~Server();
        Status getStatus();
        void setStatus( Status newStatus );
        int addConnection( std::string userName );
        void removeConnection( std::string userName );
        int addRoom( std::string name, Connection& owner );
        void removeRoom( std::string name );
};

class Connection
{
    private:
        std::string userName;
        boost::asio::ip::tcp::endpoint ep;
        std::queue<Stanza> msgQueue;
    public:
        Connection();
        Connection( const Connection& obj );
        ~Connection();
        std::string getUserName();
        void setUserName( std::string userName );
        boost::asio::ip::tcp::endpoint getEndPoint();
        void setEndPoint(  boost::asio::ip::tcp::endpoint ep );
        void addMsg( Stanza msg );
        void sendMsg( Stanza msg );
};

class Room
{
    private:
        std::string name;
        std::vector<Connection&> users;
    public:
        Room();
        Room( const Room& obj );
        ~Room();
        std::string getName();
        void setName( std::string name );
        std::vector<Connection&>& getUsers();
        void addUser( Connection& user );
};

#endif // SERVER_HPP
