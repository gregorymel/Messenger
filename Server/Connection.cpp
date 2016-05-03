#include <Server.hpp>
#include <boost/bind.hpp>
#include <string.h>

Connection::Connection( Server& server ) :
    _login( "unknown_user" ),
    _myServer( server ),
    _socket( server.service() ),
    _wasOnline( std::chrono::system_clock::now() ),
    _loggedIn( false ),
    _started( false ),
    _isWaiting( false )
{
}

void Connection::start()
{
    std::cout << "Starting new connection with " << _login << std::endl;

    _started = true;
    doReadSize();
}

void Connection::doReadSize()
{
    std::cout << "Reading size from " << _login  << std::endl;

    memset( _readBuffer, 0, size_max_len * sizeof( uint8_t ) );
    boost::asio::async_read( _socket, boost::asio::buffer( _readBuffer ),
    //boost::bind( &Connection::readComplete, shared_from_this(), _1, _2 ),
    boost::bind( &Connection::doRead, shared_from_this(), _1, _2 ) );
}

void Connection::doRead( const boost::system::error_code& err, size_t bytes )
{
    //std::cout << bytes << " bytes read" << std::endl;

    if ( err || ( bytes == 0  && !_isWaiting ) )
	{
        std::cout << err.message() << " in doRead" << std::endl;
        stop();
        return;
    }

    uint64_t size = 0;
    std::copy( _readBuffer, _readBuffer + size_max_len, &size );
    std::cout << "Next stanza size from " << _login << ": " << size << std::endl;

    if ( size <= 0 || size > buff_max_len )
    {
        stop();
        return;
    }

    //_strReadBuffer.clear();
    //_strReadBuffer.resize( size );
    boost::asio::async_read( _socket, boost::asio::buffer( _bigReadBuffer, size ),
    //boost::bind( &Connection::readComplete, shared_from_this(), _1, _2 ),
    boost::bind( &Connection::onRead, shared_from_this(), _1, _2 ) );
}

size_t Connection::readComplete( const boost::system::error_code& err, size_t bytes ) // not used
{

    if ( err )
        return 0;

    if ( bytes == 0 && _isWaiting && !_received.empty() )
    {
        std::cout << "Interrupt reading to write to " << _login << std::endl;
        return 0;
    } else
    {
        std::cout << "Not completed" << std::endl;
        return 1;
    }
}

void Connection::onRead( const boost::system::error_code& err, size_t bytes )
{
    std::cout << "On read..." << bytes << std::endl;

    if ( err )
	{
        std::cout << err.message() << " in onRead" << std::endl;
        stop();
        return;
    }

    std::string msg( _bigReadBuffer, bytes );
    //std::cout << "From " << _login << " client: " << msg << std::endl;

    Stanza st;
    try
    {
        st.load( msg );
    }
    catch ( ... )
    {
        std::cout << "Corrupted XML stanza" << std::endl;
        stop();
        return;
    }
//    if ( ( _loggedIn == false ) && ( st.getStanzaType() != Stanza::IQ ||
//         ( st.getSubType() != Stanza::SIGNIN && st.getSubType() != Stanza::SIGNUP ) ) )
//    {
//        stop();
//        return;
//    }

    switch ( st.getStanzaType() )
    {
        case Stanza::MESSAGE:
            onMessage( st );
            break;

        case Stanza::PRESENCE:
            onPresence();
            break;

        case Stanza::IQ:
            onRequest( st );
            break;

        case Stanza::ROASTER:
            onRoaster();
            break;

        default:
            break;
    }
}

void Connection::onMessage( Stanza st )
{
    std::cout << "OnMessage" << std::endl;
    std::string target = st.getTo().getNode();

    if ( _myServer.checkAccount( target ) == false )
    {
        Stanza ans;
        ans.setStanzaType( Stanza::IQ );
        ans.setSubType( Stanza::ERROR );
        //std::string strAns;
        //ans.save( strAns );
        _received.push_back( ans );
        //doWriteQuick( strAns );

        doReadSize();
        return;
    }

    resend( st );
    doReadSize();
}

void Connection::store()
{
    std::cout << "Store" << std::endl;

    if ( _received.empty() )
        return;

    Server::Data& storage = std::get<1>(*(_myServer.accounts().find( _login ))) ;

    while ( !_received.empty() )
    {
        storage.newMessages.push( _received.front() );
        _received.pop_front();
    }
}

void Connection::retrieve()
{
    std::cout << "Retrieve" << std::endl;
    Server::Data& storage = std::get<1>(*(_myServer.accounts().find( _login ))) ;

    while ( !storage.newMessages.empty() )
    {
        _received.push_back( storage.newMessages.front() );
        storage.newMessages.pop();
    }
}

void Connection::resend( Stanza st )
{
    std::cout << "Resend to " << st.getTo().getNode() << std::endl;

    //To do offline write to Data structure.

    Connection::ptr target = std::get<1>(*(_myServer.connections().find( st.getTo().getNode() ) ) ); // online
    target->_received.push_back( st );

    if ( target->_isWaiting && !target->_isWriting  )
        target->doWrite();
}

void Connection::onPresence()
{
    std::cout << "Presence" << std::endl;
    updateRecentOnlineTime();
    doReadSize();
}

void Connection::onRequest( Stanza st )
{
    std::cout << "onRequest" << std::endl;
    updateRecentOnlineTime();

    switch ( st.getSubType() )
    {
        case Stanza::SIGNIN:
        {
            onLogin( st );
            break;
        }

        case Stanza::SIGNUP:
        {
            onRegister( st );
            break;
        }

        case Stanza::GET:
        {
            std::cout << "GET " << _login << std::endl;
            _isWaiting = true;
            doWrite();
            doReadSize();
            break;
        }

        case Stanza::ADD_FRIEND:
        {
            std::cout << "ADD_FRIEND" << st.getMSG() << "->" << _login << std::endl;
            _myServer.addFriend( _login, st.getMSG() );
            doReadSize();
            break;
        }

        case Stanza::DELETE_FRIEND:
        {
            std::cout << "ADD_FRIEND" << st.getMSG() << " delete<-" << _login << std::endl;
            _myServer.deleteFriend( _login, st.getMSG() );
            doReadSize();
            break;
        }

        default:
        {
            break;
        }
    }
}

void Connection::onRoaster()
{
    std::cout << "onRoaster" << std::endl;
    updateRecentOnlineTime();

    Stanza ans;
    std::map<std::string, Connection::ptr>::iterator it;
    std::string strAns;

    for ( it = _myServer.connections().begin();
          it != _myServer.connections().end(); ++it )
    {
        JID newJID;
        newJID.setNode( std::get<1>(*it)->getLogin() );
        ans.addAvailable( newJID );
    }

    ans.setStanzaType( Stanza::ROASTER );
    _received.push_back( ans );

    if ( _isWaiting && !_isWriting  )
        doWrite();

    doReadSize();
    //ans.save( strAns );
    //doWriteQuick( strAns );
}

void Connection::onLogin( Stanza st )
{
    std::string body  = st.getMSG();
    std::string login = body.substr( 0, body.find("\n") );
    std::string psswd = body.substr( body.find("\n") );

    if ( _myServer.checkLoginAndPassword( login, psswd ) == true )
    {
        _login = login;
        _loggedIn = true;
        _myServer.addConnection( shared_from_this() );
        retrieve();
        Stanza ans;
        ans.setStanzaType( Stanza::IQ );
        ans.setSubType( Stanza::AVAILABLE );
        //std::string strAns;
        //ans.save( strAns );
        _received.push_back( ans );
        //doWriteQuick( strAns );
    } else
    {
        Stanza ans;
        ans.setStanzaType( Stanza::IQ );
        ans.setSubType( Stanza::ERROR );
        //std::string strAns;
        //ans.save( strAns );
        _received.push_back( ans );
        //doWriteQuick( strAns );
    }

    if ( _isWaiting && !_isWriting  )
        doWrite();

    doReadSize();
}

void Connection::onRegister( Stanza st )
{
    std::string body  = st.getMSG();
    std::string login = body.substr( 0, body.find("\n") );
    std::string psswd = body.substr( body.find("\n") );

    if ( _myServer.checkAccount( login ) == true )
    {
        Stanza ans;
        ans.setStanzaType( Stanza::IQ );
        ans.setSubType( Stanza::ERROR );
        //std::string strAns;
        //ans.save( strAns );
        _received.push_back( ans );
        //doWriteQuick( strAns );
    } else
    {
        _myServer.addAccount( login, psswd );

        Stanza ans;
        ans.setStanzaType( Stanza::IQ );
        ans.setSubType( Stanza::AVAILABLE );
        //std::string strAns;
        //ans.save( strAns );
        _received.push_back( ans );
        //doWriteQuick( strAns );
    }

    if ( _isWaiting && !_isWriting )
        doWrite();

    doReadSize();
}

//void Connection::doWriteQuick( std::string msg )
//{
//    std::cout << "Do write quick..." << std::endl;
//
//	if ( !_started )
//        return;
//
//    _strWriteBuffer.resize( msg.size() );
//    std::copy( msg.begin(), msg.end(), _strWriteBuffer.begin() );
//
//	_socket.async_write_some( boost::asio::buffer( _strWriteBuffer, _strWriteBuffer.size() ),
//	boost::bind( &Connection::onWriteQuick, shared_from_this(), _1, _2 ) );
//}
//
//void Connection::onWriteQuick( const boost::system::error_code& err, size_t bytes )
//{
////    if ( err )
////    {
////        stop();
////        return;
////    }
//    std::cout << "On quick write..." << std::endl;
//    doReadSize();
//}

void Connection::doWrite()
{
    std::cout << "Do write..." << std::endl;

	if ( !_started )
        return;

    if ( _received.empty() )
    {
        std::cout << "EMPTY FOR NOW" << std::endl;
        //doReadSize();
        return;
    }

    _isWriting = true;
    std::string msg;
    _received.front().save( msg );
    _received.pop_front();

    uint64_t msgSize = msg.size();

    //_strWriteBuffer.clear();
    //_strWriteBuffer.resize( msgSize + size_max_len );
    std::copy( &msgSize, &msgSize + 1, _bigWriteBuffer );
    std::copy( msg.begin(), msg.end(), _bigWriteBuffer + size_max_len );

    std::cout << "size: " << msgSize  << std::endl;
    std::cout << " To " << _login <<  " client: " << msg << std::endl;

	_socket.async_write_some( boost::asio::buffer( _bigWriteBuffer, size_max_len + msgSize ),
	boost::bind( &Connection::onWrite, shared_from_this(), _1, _2 ) );
}

void Connection::onWrite( const boost::system::error_code& err, size_t bytes )
{
    if ( err )
    {
        std::cout << "OnWrite error" << bytes << std::endl;
        stop();
        return;
    }

    std::cout << "On write..." << bytes << std::endl;

    if ( _isWaiting )
    {
        Stanza ev;
        ev.setStanzaType( Stanza::EVENT );
        ev.setSubType( Stanza::END );
        _received.push_back( ev );
        std::cout << " _isWaiting = 0" << std::endl;
        _isWaiting = false;
    }

    if ( !_received.empty() )
    {
        doWrite();
        return;
    }

    _isWriting = false;
    //doReadSize();
}

void Connection::stop()
{
    std::cout << "Closing connection..." << std::endl;

    if ( !_started )
        return;

    _socket.close();
    _started = false;
    _loggedIn = false;
    store();
    _myServer.deleteConnection( _login );
}

