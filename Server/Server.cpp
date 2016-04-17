#include <Server.hpp>
#include <boost/bind.hpp>

Server::Server( int port ) :
    _service(),
    _status( UNAVAILABLE ),
    _acceptor( _service, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) ),
    _connections()
{
}

void Server::handleAccept( Connection::ptr connect, const boost::system::error_code& err )
{
    std::cout << "Handling accept..." << std::endl;
    connect->start();
    this->addConnection( connect );

	Connection::ptr new_connect = Connection::new_( *this );
	_acceptor.async_accept( new_connect->socket(),
    boost::bind( &Server::handleAccept, this, new_connect, _1 ) );
}

void Server::start()
{
    std::cout << "Starting server..." << std::endl;
    _status = AVAILABLE;

    Connection::ptr new_connect = Connection::new_( *this );
    _acceptor.async_accept( new_connect->socket(),
    boost::bind( &Server::handleAccept, this, new_connect, _1 ) );
}

void Server::addConnection( Connection::ptr newConnection )
{
    std::cout << newConnection->userName() <<" added to the list..." << std::endl;

    _connections.push_back( newConnection );
    _clientsChanged = true;
}

void Server::deleteConnection( Connection::ptr closedConnection )
{
    std::cout << "Deleting connection " << closedConnection->userName() << std::endl;

    std::vector<Connection::ptr>::iterator it = std::find( _connections.begin(),
                                                           _connections.end(),
                                                           closedConnection );
    _connections.erase( it );
    _clientsChanged = true;
}

void Server::addAccount( std::pair<std::string, std::string> newAcc )
{
    std::cout << "Account added..." << std::endl;
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

bool Server::checkLoginAndPassword( std::pair<std::string, std::string> acc )
{
    std::map<std::string, std::string>::iterator it = _accounts.find( std::get<0>(acc) );

    if ( it == _accounts.end() )
        return false;

    if ( std::get<1>(*it) != std::get<1>(acc) )
        return false;

    return true;
}

void Server::stop()
{
    std::vector<Connection::ptr>::iterator it = _connections.begin();

    while ( it != _connections.end() )
    {
        (*it)->stop();
        it = _connections.begin();
    }
}


