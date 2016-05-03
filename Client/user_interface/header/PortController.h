//  PortController.h
#ifndef _PortController_h_
#define _PortController_h_

#include "port.hpp"
#include "Stanza.hpp"
#include <vector>
#include <thread>
#include <functional>
#include <vector>
#include <QObject>

// Pattern Observer
class PortController;

class InterfaceItem
{
protected:
	PortController* port;
public:
	InterfaceItem(PortController* _port) : port( _port){}
	//InterfaceItem(QString& login, PortController* _port) : _login_(login), port(_port){};
	virtual void update() = 0;
};


class PortController : public QObject
{
	Q_OBJECT
private:
	// copy and assignment not allowed
	void operator=( const PortController&) = delete;
	PortController( const PortController&) = delete;

	Port* port;
	std::vector<InterfaceItem*> items;
	Stanza cur_stanza;

	void recieveItem()
	{
		cur_stanza = port->recieve_from_server();

		std::string xml;
		cur_stanza.save(xml);
		std::cerr << "Reciev stanza " << xml << std::endl;

		std::cerr << std::this_thread::get_id() << std::endl;
		emit itemWasRecieved();
		std::cerr << "emit" << std::endl;
	}

public:
	Stanza getStanza() { return cur_stanza; }

	PortController(Port* _port, QObject* parent = 0) : QObject( parent), port(_port)
	{
		std::cerr << std::this_thread::get_id() << std::endl;
		connect( this, SIGNAL(itemWasRecieved()), this, SLOT(onRecieveItem()));
		scanPort();
	}

	void scanPort()
	{
		//std::cerr << std::this_thread::get_id() << std::endl;
		std::thread th(&PortController::recieveItem, this);
		th.detach();
	}

	void attach(InterfaceItem* item)
	{
		items.push_back(item);
	}

	void detach(InterfaceItem* item)
	{
		std::vector<InterfaceItem*>::iterator it;
		for ( it = items.begin(); it != items.end(); it++)
			if ( *it == item)
			{
				items.erase(it);
				break;
			}	
	}

	void notify()
	{
		for ( int i = 0; i < items.size(); i++)
			items[i]->update();
	}

	void send(Stanza stanza) {
		std:: cerr << "SEND STANZA" << std::endl; 
		port->send_to_server(stanza); } 

signals:
	void itemWasRecieved();
public slots:
	void onRecieveItem()
	{
		std::cerr << "onRecieveItem\n";
		notify();
		scanPort();				
	}
};


#endif //_PortController_h_