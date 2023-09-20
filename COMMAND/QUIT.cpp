#include "command.hpp"

void	QUIT(Server *server, std::string params, int id)
{
	std::string message = "";

	size_t pos = params.find_first_of(':');
	if (pos != std::string::npos) {
		message = params.substr(pos);
	}

	User *user = server->getUserMap()[id];
	std::string reply = ":" + user->getNick() + "!" + user->getUsername() + "@"
						+ user->getHost() + " QUIT :" + message + "\r\n";


	for (mChannel::iterator itC = server->getChannelMap().begin();
		 itC != server->getChannelMap().end(); ++itC)
	{
		itC->second->leaveChan(user->getNick());
		for (std::vector<User*>::iterator itU = itC->second->getUsers().begin();
			 itU != itC->second->getUsers().end(); ++itU)
			send((*itU)->getId(), reply.c_str(), reply.size(), MSG_DONTWAIT);
	}
	server->setKilled(id);
}