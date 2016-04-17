#include <Server.hpp>
#include <boost/bind.hpp>

Connection::Connection( Server& server ) :
    _myServer( server ),
    _socket( server.service() )
{
}

void Connection::start()
{
    std::cout << "Starting new unauthorized connection" << std::endl;

    _started = true;
    this->doRead();
}

void Connection::doRead()
{
    std::cout << "Reading..." << std::endl;

    boost::asio::async_read( _socket, boost::asio::buffer( _readBuffer ),
    boost::bind( &Connection::readComplete, shared_from_this(), _1, _2 ),
    boost::bind( &Connection::onRead, shared_from_this(), _1, _2 ) );
}

size_t Connection::readComplete( const boost::system::error_code& err, size_t bytes )
{
    std::cout << std::string( _readBuffer ) << std::endl;

    if ( err )
        return 0 ;

    bool found = std::find( _readBuffer, _readBuffer + bytes, '\n') < _readBuffer + bytes;

    if ( found == true )
        return 0;
    else
        return 1;
}

void Connection::onRead( const boost::system::error_code& err, size_t bytes )
{
    std::cout << "On read..." << std::endl;

	if ( err )
        this->stop();

	if ( !_started )
        return;

	std::string msg( _readBuffer, bytes );
	std::cout << " Message from client: " << msg << std::endl;

	if ( msg.find( "login:" ) == 0 )
        this->onLogin( msg );
	else
        if ( msg.find( "clients" ) == 0 )
            this->onClients();
        else
            doWrite( msg );
}

void Connection::onWrite( const boost::system::error_code& err, size_t bytes )
{
    std::cout << "On write..." << std::endl;
    doRead();
}

void Connection::onLogin( const std::string msg )
{
    std::cout << "On login..." << std::endl;
	_userName = msg.substr( 6, std::string::npos );

	std::pair<std::string,std::string> newAccount( _userName, std::string( "null" ) );
	_myServer.addAccount( newAccount );

	std::string answer( "login OK---Your name: " );
	answer += _userName;
	doWrite( answer );
	_myServer.setClientsChanged( true );
}

void Connection::doWrite( const std::string msg )
{
    std::cout << "Do write..." << std::endl;
	if ( !_started )
        return;

	std::copy( msg.begin(), msg.end(), _writeBuffer );
	std::cout << " Message to client: " << std::string(_writeBuffer, msg.size()) << std::endl;
	_socket.async_write_some( boost::asio::buffer( _writeBuffer, msg.size() ),
	boost::bind( &Connection::onWrite, shared_from_this(), _1, _2 ) );
}

void Connection::onClients()
{
    std::cout << "Sending the list of users..." << std::endl;
	std::string msg;
    std::vector<Connection::ptr>::iterator e = _myServer.connections().end();

	for( std::vector<Connection::ptr>::iterator b = _myServer.connections().begin(); b != e; ++b )
    {
        msg += (*b)->userName() + " ";
    }

	doWrite( "Clients: " + msg + "\n" );
}

void Connection::stop()
{
    std::cout << "Closing connection..." << std::endl;
    if ( !_started )
        return;

    _started = false;
    _socket.close();
    _myServer.deleteConnection( shared_from_this() );
}

