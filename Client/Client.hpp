#ifndef CLIENT_HPP
#define CLIENT_HPP
//STL
#include <queue>
//Boost.Asio
#include <boost/asio.hpp>
//Local
#include <Stanza.hpp>

class Client
{
    private:
        enum State
        {
        }
        State state;
        std::queue<Stanza> writePort;
        std::queue<Stanza> readPort;
        boost::asio::ip::tcp::endpoint ep;
    public:
        Client();
        Client( const Client& obj );
        ~Client();
        int connect( JID jid, std::string password );
        void disconnect();
        bool sendPackage();
        bool receivePackage();
        void pushStanza( Stanza stanza );
        Stanza popStanza();
};

#endif // CLIENT_HPP
