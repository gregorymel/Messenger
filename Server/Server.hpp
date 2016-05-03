#ifndef SERVER_HPP
#define SERVER_HPP

//STL containers
#include <map>
#include <set>
#include <deque>
#include <queue>
#include <vector>
#include <chrono>
#include <string>
#include <iostream>
//Boost libraries
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
//Local libraries
#include <Stanza.hpp>
class Server;

class Connection : public boost::enable_shared_from_this<Connection>, boost::noncopyable
{
    private:

        boost::asio::ip::tcp::socket _socket;
        std::chrono::system_clock::time_point _wasOnline;
        Server& _myServer;
        bool _started;      // Has connection been started?
        bool _loggedIn;     // Has the user logged in?
        bool _isWaiting;    // Is the user waiting for answer? (for long-polling)
        bool _isWriting;


        std::string _login;             // user 1
        std::deque<Stanza> _received;
        std::vector<JID> _friendList;

        std::vector<uint8_t> _strWriteBuffer;
        std::vector<char> _strReadBuffer;

        enum { size_max_len = 8,
               buff_max_len = 2048 };

        uint8_t _writeBuffer[size_max_len];
        uint8_t _readBuffer[size_max_len];

        char _bigWriteBuffer[buff_max_len];
        char _bigReadBuffer[buff_max_len];

        Connection( Server& myServer );

        void onRead( const boost::system::error_code&, size_t );
        void doRead( const boost::system::error_code&, size_t );
        void updateRecentOnlineTime()
        {
            _wasOnline = std::chrono::system_clock::now();
        }
        void doReadSize();
        void onPresence();
        void onRoaster();
        void onRequest( Stanza );
        void onMessage( Stanza );
        void onLogin( Stanza );
        void onRegister( Stanza );
        void startWrite();
        void doWrite();
        void doWriteQuick( std::string msg );
        void store();
        void retrieve();
        void resend( Stanza st );
        void onWrite( const boost::system::error_code&, size_t );
        void onWriteQuick( const boost::system::error_code&, size_t );
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

        std::string getLogin()
        {
            return _login;
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
    public:

        typedef struct Data
        {
            std::string password;
            std::queue<Stanza> newMessages;
            std::set<std::string> friends;
            std::set<std::string> activeRequests;
        } Data;

    private:

        enum Status
        {
            AVAILABLE = 0,
            UNAVAILABLE,
            MAX_SERVER_STATUS
        };

        boost::asio::io_service _service;
        boost::asio::ip::tcp::acceptor _acceptor;

        std::map<std::string, Connection::ptr> _connections;
        std::map<std::string, Data> _accounts;

        Status _status;
        bool _clientsChanged;

        void handleAccept( Connection::ptr, const boost::system::error_code& );
    public:

        Server( int );
        Server( const Server& );
        ~Server() = default;

        void addConnection( Connection::ptr );
        void deleteConnection( std::string login );
        void addAccount( std::string login, std::string pass );
        void deleteAccount( std::string login);
        bool checkAccount( std::string login );
        bool checkLoginAndPassword( std::string login, std::string pass );
        void addFriend( std::string owner, std::string newFriend );
        void deleteFriend( std::string owner, std::string newFriend );

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

        std::map<std::string, Connection::ptr>& connections()
        {
            return _connections;
        }

        std::map<std::string, Data>& accounts()
        {
            return _accounts;
        }
};

#endif // SERVER_HPP
