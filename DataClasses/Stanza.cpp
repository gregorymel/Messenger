#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "Stanza.hpp"

void Stanza::load(const std::string &str)
{
	using boost::property_tree::ptree;
	ptree pt;

	read_xml(str, pt);

	_from = pt.get<JID>("Stanza.from");

	_to = pt.get<JID>("Stanza.to");

	type = pt.get<StanzaType>("Stanza.Type");

	switch ( type )
	{
		case MESSAGE :
		{
			body = pt.get<std::string>("Stanza.Msg.Body");
			subType = pt.get<subType>("Stanza.Msg.type");
			break;
		}
		case PRESENCE :
		{
			subType = pt.get<subType>("Stanza.Presense.Status");
			break;
		}
		case IQ :
		{
			subType = pt.get<subType>("Stanza.IQ.type");
			break;
		}
		case ROSTER:
		{
			BOOST_FOREACH(ptree::value_type &v,
            			      pt.get_child("Stanza.Roster.users"))
			availabilityList.insert(v.second.data());
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
	ptree pt;

	pt.put("Stanza.from", _from);
	pt.put("Stanza.to", _to);
	pt.put("Stanza.Type",type);

	switch ( type )
        {
                case MESSAGE :
                {
                        pt.put("Stanza.Msg.Body", body);
                        pt.put("Stanza.Msg.type", subType);
                        break;
                }
                case PRESENCE :
                {
                        pt.put("Stanza.Presense.Status", subType);
                        break;
                }
                case IQ :
                {
                        pt.put("Stanza.IQ.type", subType);
                        break;
                }
                case ROSTER:
                {
                        BOOST_FOREACH(const std::string &name, availabilityList)
                        pt.put("Stanza.Roster.users.user", name, true);
                        break;
                }
                default :
                {
                        break;
                }
        }
   	write_xml(str, pt);
}
