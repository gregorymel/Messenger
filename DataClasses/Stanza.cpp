#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include "Stanza.hpp"

void Stanza::load(const std::string &str)
{
	using boost::property_tree::ptree;
	ptree pt;

	read_xml(str, pt);
		
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
		default :
		{
			return;
		}
	}
}



void Stanza::save(const std::string &str)
{
	using boost::property_tree::ptree;
	ptree pt, node;
	std::string tmp;
	
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
                        break;
                }
                case ROASTER:
                {
                        BOOST_FOREACH(JID &jid, availabilityList){
                        	node.put("JID.Node", jid.getNode());
				node.put("JID.Domain", jid.getDomain());
				node.put("JID.Resource", jid.getResource());
				pt.add_child("Stanza.Roster.users", node);
			}
                        break;
                }
                default :
                {
                        break;
                }
        }
   	write_xml(str, pt);
}
