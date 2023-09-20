#include "command.hpp"

void	PRIVMSG(Server *server, std::string params, int id) {
	if (server->getUserMap()[id]->getMode('r') == true || server->getUserMap()[id]->getComplete() == false)
		return (send_message(server, id, ERR_RESTRICTED, ""));
	else if (!params.size())
		return (send_message(server, id, ERR_NEEDMOREPARAMS, params));

	std::string targets;
	std::string message;

	size_t pos = params.find_first_of(':');
	if (pos != std::string::npos)
	{
		targets = params.substr(0, pos - 1);
		message = params.substr(pos);
	}
	else
		return (send_message(server, id, ERR_NORECIPIENT, "PRIVMSG"));

	if (targets.size() == 0)
		return (send_message(server, id, ERR_NORECIPIENT, "PRIVMSG"));
	else if (message.size() == 1)
		return (send_message(server, id, ERR_NOTEXTTOSEND, ""));

	std::vector<std::string> msgTargets;
	std::stringstream ss(targets);
	std::string s;

	if (targets.find(',') != std::string::npos)
		while (std::getline(ss, s, ','))
			msgTargets.push_back(s);
	else
		msgTargets.push_back(targets);

	for (std::vector<std::string>::iterator itT = msgTargets.begin(); itT != msgTargets.end(); itT++)
	{
		if (itT.base()->find('#', 0) != std::string::npos)
		{
			mChannel chanCp = server->getChannelMap();
			if (server->getChannelMap().find(*itT) != server->getChannelMap().end())
			{
				Channel *chan = server->getChannelMap()[*itT];
				std::vector<User*> users = chan->getUsers();
				if (chan->getMapUser()[id])
				{
					for (std::vector<User*>::iterator it = users.begin(); it != users.end(); it++)
					{
						if ((*it)->getNick() == server->getUserMap()[id]->getNick())
							;
						else
						{
							std::string reply =  ":" + server->getUserMap()[id]->getNick() + "!"
							+ server->getUserMap()[id]->getUsername() + "@"
							+ server->getUserMap()[id]->getHost() + " PRIVMSG " + *itT
							+ " " + message + "\r\n";
							send((*it)->getId(), reply.c_str(), reply.size(), MSG_DONTWAIT);
						}
					}
				}
				else
					return send_message(server, id, ERR_CANNOTSENDTOCHAN, server->getUserMap()[id]->getNick());
			}
			else
				return send_message(server, id, ERR_NOSUCHCHANNEL, *itT);
		}
		else
		{
			if (server->getUserMapName()[*itT])
				for (std::map<int, User*>::iterator it = server->getUserMap().begin(); it != server->getUserMap().end(); it++)
				{
					if (it->second->getNick() == *itT)
					{
						if (message.size() > 1)
						{
							std::string reply = ":" + server->getUserMap()[id]->getNick() + "!"
							+ server->getUserMap()[id]->getUsername() + "@"
							+ server->getUserMap()[id]->getHost() + " PRIVMSG " + *itT + " "
							+ message + "\r\n";
							send(it->first, reply.c_str(), reply.size(), MSG_DONTWAIT);
						}
						else
							send_message(server, id, ERR_NORECIPIENT, "PRIVMSG");
					}
				}
			else
				send_message(server, id, ERR_NOSUCHNICK, *itT);
		}
	}
}