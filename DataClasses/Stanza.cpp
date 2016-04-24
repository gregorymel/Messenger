#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "Stanza.hpp"
#include <sstream>


void Stanza::load(const std::string &str)
{
	using boost::property_tree::ptree;
	ptree pt;
	std::stringstream ss;

	ss.str(str);

	read_xml(ss, pt);

	from.setNode(pt.get<std::string>("Stanza.from.Node"));
	from.setDomain(pt.get<std::string>("Stanza.from.Domain"));
	from.setResource(pt.get<std::string>("Stanza.from.Resource"));

	to.setNode(pt.get<std::string>("Stanza.to.Node"));
	to.setDomain(pt.get<std::string>("Stanza.to.Domain"));
	to.setResource(pt.get<std::string>("Stanza.to.Resource"));

	type = static_cast<StanzaType>(pt.get<int>("Stanza.Type"));

	switch ( type )
	{
		case MESSAGE :
		{
			body = pt.get<std::string>("Stanza.Msg.Body");
			subType = static_cast<SubType>(pt.get<int>("Stanza.Msg.type"));
			break;
		}
		case PRESENCE :
		{
			subType = static_cast<SubType>(pt.get<int>("Stanza.Presense.Status"));
			break;
		}
		case IQ :
		{
			subType = static_cast<SubType>(pt.get<int>("Stanza.IQ.type"));

			if ( subType == Stanza::SIGNIN || subType == Stanza::SIGNUP )
                 body = pt.get<std::string>("Stanza.IQ.Body");

			break;
		}
		case ROASTER:
		{
			BOOST_FOREACH(ptree::value_type &v,
            			      pt.get_child("Stanza.Roster.users")) {
				availabilityList.push_back(v.second.data());
			}
			break;
		}
		case EVENT:
		{
			subType = static_cast<SubType>(pt.get<int>("Stanza.Event.type"));
			break;
		}
		default :
		{
			return;
		}
	}
}

void Stanza::save( std::string &str)
{
	using boost::property_tree::ptree;
	ptree pt, node;
	std::string tmp;
	std::stringstream ss;

	pt.put("Stanza.from.Node", from.getNode());
	pt.put("Stanza.from.Domain", from.getDomain());
	pt.put("Stanza.from.Resource", from.getResource());


	pt.put("Stanza.to.Node", to.getNode());
	pt.put("Stanza.to.Domain", to.getDomain());
	pt.put("Stanza.to.Resource", to.getResource());

	pt.put("Stanza.Type", static_cast<int>(type));

	switch ( type )
    {
        case MESSAGE :
        {
            pt.put("Stanza.Msg.Body", body);
            pt.put("Stanza.Msg.type", static_cast<int>(subType));
            break;
        }
        case PRESENCE :
        {
            pt.put("Stanza.Presense.Status", static_cast<int>(subType));
            break;
        }
        case IQ :
        {
            pt.put("Stanza.IQ.type", static_cast<int>(subType));

            if ( subType == Stanza::SIGNIN || subType == Stanza::SIGNUP )
                 pt.put("Stanza.IQ.Body", body);
            break;
        }
        case ROASTER:
        {
            BOOST_FOREACH(JID &jid, availabilityList)
            {
                node.put("JID.Node", jid.getNode());
                node.put("JID.Domain", jid.getDomain());
                node.put("JID.Resource", jid.getResource());
                pt.add_child("Stanza.Roster.users", node);
            }
            break;
        }
		case EVENT:
		{
			pt.put("Stanza.EVENT.type", static_cast<int>(subType));
			break;
		}
        default :
        {
            break;
        }
    }
   	write_xml(ss, pt);
	str = ss.str();
}

Stanza::StanzaType Stanza::getStanzaType()
{
	return type;
}

Stanza::SubType Stanza::getSubType()
{
	return subType;
}

JID Stanza::getFrom()
{
	return from;
}

JID Stanza::getTo()
{
	return to;
}

void Stanza::setStanzaType(StanzaType val)
{
	type = val;
}

void Stanza::setSubType(SubType val)
{
	subType = val;
}

void Stanza::setFrom( JID id)
{
	from = id;
}

void Stanza::setTo( JID id)
{
	to = id;
}

void Stanza::setMSG(SubType val, std::string msg)
{
	subType = val;
	body = msg;
}

std::string Stanza::getMSG()
{
	return body;
}

void Stanza::addAvailable( JID newJID )
{
	availabilityList.insert(newJID);
}
