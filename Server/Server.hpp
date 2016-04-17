#ifndef SERVER_HPP
#define SERVER_HPP

//STL containers
#include <map>
#include <queue>
#include <vector>
#include <string>
#include <iostream>
//Boost libraries
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
//Local libraries
#include <Stanza.hpp>

typedef std::pair<std::string,std::string> lp;
class Server;

class Connection : public boost::enable_shared_from_this<Connection>, boost::noncopyable
{
    private:

        boost::asio::ip::tcp::socket _socket;
        Server& _myServer;
        bool _started;

        std::string _userName;

        std::queue<Stanza> _received;
        std::queue<Stanza> _toSend;

        enum { max_len = 4096 };
        std::string _strWriteBuffer;
        std::string _strReadBuffer;
        char _writeBuffer[max_len];
        char _readBuffer[max_len];

        Connection( Server& myServer );

        void onRead( const boost::system::error_code& , size_t);
        void doRead();
        void onClients();
        void onLogin( const std::string msg );
        void doWrite( const std::string msg );
        void onWrite( const boost::system::error_code& , size_t );
        size_t readComplete( const boost::system::error_code&, size_t );

    public:
        typedef boost::shared_ptr<Connection> ptr;

        ~Connection() = default;
        void start();
        void stop();


        Server& server()
        {
            return _myServer;
        }

        std::string userName()
        {
            return _userName;
        }

        static ptr new_( Server& server )
        {
            ptr new_( new Connection( server ) );
            return new_;
        }
        boost::asio::ip::tcp::socket& socket()
        {
            return _socket;
        }
};

class Server
{
    private:

        enum Status
        {
            AVAILABLE = 0,
            UNAVAILABLE,
            MAX_SERVER_STATUS
        };

        boost::asio::io_service _service;
        boost::asio::ip::tcp::acceptor _acceptor;
        std::vector<Connection::ptr> _connections;
        std::map<std::string, std::string> _accounts;

        Status _status;
        bool _clientsChanged;

        void handleAccept( Connection::ptr, const boost::system::error_code& );
    public:
        Server( int );
        Server( const Server& );
        ~Server() = default;

        void addConnection( Connection::ptr );
        void deleteConnection( Connection::ptr );
        void addAccount( std::pair<std::string, std::string> );
        void deleteAccount( std::string );
        bool checkAccount( std::string );
        bool checkLoginAndPassword( std::pair<std::string, std::string> );

        void start();
        void stop();


        void setClientsChanged( bool value )
        {
            _clientsChanged = value;
        }
        Status getStatus()
        {
            return _status;
        }
        boost::asio::io_service& service()
        {
            return _service;
        }

        std::vector<Connection::ptr>& connections()
        {
            return _connections;
        }
//        void addRoom( std::string name, Connection& owner );
//        void removeRoom( std::string name );
};

//class Room
//{
//    private:
//        std::string _name;
//        std::vector<Connection> _users;
//
//    public:
//        Room();
//        Room( const Room& obj );
//        ~Room();
//        std::string getName();
//        void setName( std::string name );
//        std::vector<Connection&>& getUsers();
//        void addUser( Connection& user );
//};

#endif // SERVER_HPP
