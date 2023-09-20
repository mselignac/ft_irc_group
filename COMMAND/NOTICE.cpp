#include "command.hpp"

void	NOTICE(Server *server, std::string params, int id) {
	if (server->getUserMap()[id]->getMode('r') == true)
		return ;
	else if (!params.size())
		return ;

	std::string targets;
	std::string message;

	size_t pos = params.find_first_of(':');
	if (pos != std::string::npos)
	{
		targets = params.substr(0, pos - 1);
		message = params.substr(pos);
	}
	else
		return ;

	if (targets.size() == 0)
		return ;
	else if (message.size() == 1)
		return ;

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
							+ server->getUserMap()[id]->getHost() + " NOTICE " + *itT
							+ " " + message + "\r\n";
							send((*it)->getId(), reply.c_str(), reply.size(), MSG_DONTWAIT);
						}
					}
				}
			}
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
							+ server->getUserMap()[id]->getHost() + " NOTICE " + *itT + " "
							+ message + "\r\n";
							send(it->first, reply.c_str(), reply.size(), MSG_DONTWAIT);
						}
					}
				}
		}
	}
}