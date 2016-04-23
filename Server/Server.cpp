#include <Server.hpp>
#include <boost/bind.hpp>

Server::Server( int port ) :
    _service(),
    _status( UNAVAILABLE ),
    _acceptor( _service, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) )
{
}

void Server::handleAccept( Connection::ptr connect, const boost::system::error_code& err )
{
    std::cout << "Handling accept..." << std::endl;
    connect->start();
    this->addConnection( connect );

	Connection::ptr newConnect = Connection::new_( *this );
	_acceptor.async_accept( newConnect->socket(),
    boost::bind( &Server::handleAccept, this, newConnect, _1 ) );
}

void Server::start()
{
    std::cout << "Starting server..." << std::endl;
    _status = AVAILABLE;

    Connection::ptr newConnect = Connection::new_( *this );
    _acceptor.async_accept( newConnect->socket(),
    boost::bind( &Server::handleAccept, this, newConnect, _1 ) );
}

void Server::addConnection( Connection::ptr newConnection )
{
    std::cout << "Connection added to the list..." << std::endl;

    _connections.insert( std::pair<std::string, Connection::ptr> ( "unknown", newConnection ) );
    _clientsChanged = true;
}

void Server::deleteConnection( std::string login )
{
    std::cout << "Deleting connection " << login << std::endl;

    _connections.erase( _connections.find( login ) );
    _clientsChanged = true;
}

void Server::addAccount( std::string login, std::string pass )
{
    std::cout << "Account added..." << std::endl;

    std::pair<std::string, Data> newAcc;
    newAcc.first = login;
    newAcc.second.password = pass;

    _accounts.insert( newAcc );
}

void Server::deleteAccount( const std::string login )
{
    std::cout << "Account deleted..." << std::endl;
    _accounts.erase( login );
}

bool Server::checkAccount( const std::string login )
{
    if ( _accounts.find( login ) == _accounts.end() )
        return false;

    return true;
}

bool Server::checkLoginAndPassword( std::string login, std::string pass )
{
    std::map<std::string, Data>::iterator it = _accounts.find( login );

    if ( it == _accounts.end() )
        return false;

    return ( std::get<1>(*it).password == pass );
}

void Server::goOnline( std::string login )
{
    std::cout << login << " online..." << std::endl;

    _online.insert( login );
}

void Server::goOffline( std::string login )
{
    std::cout << login << " offline..." << std::endl;

    _online.erase( login );
}

void Server::stop()
{
}


