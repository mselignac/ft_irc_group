#include "command.hpp"

void	NICK(Server *server, std::string nick, int id) {

	std::map<int, User*> clients = server->getUserMap();

	if (clients[id]->getMode('r') == true)
		return (send_message(server, id, ERR_RESTRICTED, ""));

	if (!nick.size())
		return (send_message(server, id, ERR_NONICKNAMEGIVEN, ""));
	else if (nick.length() > 9 || (!isalpha(nick[0]) && !isspecial(nick[0])))
		return (send_message(server, id, ERR_ERRONEUSNICKNAME, nick));

	for (size_t i = 0; i < nick.size(); i++)
		if (!isalnum(nick[i]) && !isspecial(nick[i]) && nick[i] != '-')
			return (send_message(server, id, ERR_ERRONEUSNICKNAME, nick));

	for (std::map<int, User*>::iterator it = clients.begin(); it != clients.end(); ++it)
		if ((*it->second).getNick() == nick)
			return (send_message(server, id, ERR_NICKNAMEINUSE, nick));

	if (nick == "BOT" && (server->getUserMap()[id]->getBot() != true))
		return (send_message(server, id, ERR_NICKNAMEINUSE, nick));

	std::string old_nick = server->getUserMap()[id]->getNick();
	if (old_nick == "")
		old_nick = "*";
	server->getUserMap()[id]->setNick(nick);
	std::string new_nick = server->getUserMap()[id]->getNick();
	std::string name = server->getUserMap()[id]->getUsername();
	std::string message = ":" + old_nick + "!" + name + "@" + clients[id]->getHost() + " NICK " + new_nick + "\r\n";
	// std::cout << "REPLY --- " << message;
	send(id, message.c_str(), message.length(), MSG_DONTWAIT);

	for (mChannel::iterator itC = server->getChannelMap().begin();
		 itC != server->getChannelMap().end(); ++itC)
		for (std::vector<User*>::iterator itU = itC->second->getUsers().begin();
			 itU != itC->second->getUsers().end(); ++itU)
		{
			if (nick != (*itU)->getNick())
				send((*itU)->getId(), message.c_str(), message.size(), MSG_DONTWAIT);
		}
	if (server->getUserMap()[id]->getRegistered() == true && server->getUserMap()[id]->getComplete() == false) {
    	send_message(server, id, RPL_WELCOME, "");
    	send_message(server, id, RPL_YOURHOST, "");
    	send_message(server, id, RPL_CREATED, "");
    	send_message(server, id, RPL_MYINFO, "");
		server->getUserMap()[id]->setComplete();
    }
}