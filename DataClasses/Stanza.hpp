#ifndef STANZA_HPP
#define STANZA_HPP

//Standart C++ libraries
#include <string>
#include <list>
//Local headers
#include "JID.hpp"

class Stanza
{
    public:
	enum SubType
	{
	    CHAT = 0,
            PRIVATE,
            AVAILABLE,
            UNAVAILABLE,
            GET,
            SET,
            RESULT,
            ERROR,
            MAX_SUBTYPE
	};
	enum StanzaType
        {
            MESSAGE = 0,
            PRESENCE,
            IQ,
	    ROASTER,
            MAX_STANZA_TYPE
        };
    protected:
        JID from;
        JID to;
        StanzaType type;
	SubType subType;
	std::string body;
	std::list<JID> availabilityList;     
    public:	
	Stanza(){};        
	Stanza( JID from, JID to ){};
        Stanza( const Stanza &obj ){};
        ~Stanza(){};
        StanzaType getStanzaType();
	SubType getSubType();
        JID getFrom();
        JID getTo();
	void load(const std::string &filename);
	void save( std::string &filename);
	std::list<JID>& getAvailable();
        void addAvailable( JID newJID );
	void setStanzaType(StanzaType val);
	void setSubType(SubType val);
	void setFrom(JID id);
	void setTo(JID id);
	void setMSG(SubType val, std::string msg);
	std::string getMSG();
};
#endif //STANZA_HPP
