#include <iostream>
#include "Stanza.hpp"

int main(void)
{
	JID from;
	JID to;
	Stanza msg;

	from.setNode("FROM_NODE");
	from.setDomain("FROM_DOMAIN");
	from.setResource("FROM_RESOURCE");

	to.setNode("TO_NODE");
	to.setDomain("TO_DOMAIN");
	to.setResource("TO_RESOURCE");
#if 0
	msg.setFrom( from );
	msg.setTo( to );
	msg.setMSG( Stanza::CHAT, "TEST EXAMPLE");
#endif
	msg.setStanzaType( Stanza::EVENT);
	msg.setSubType( Stanza::BEGIN);
	
	std::string str;

	msg.save(str);

	std::cout << msg.getFrom().getDomain() << std::endl;

	std::cout << str << std::endl;

	return 0;
}
