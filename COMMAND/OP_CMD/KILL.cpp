# include "../command.hpp"

static void	leaveChan(Server *serv, std::string message, int id, User *user)
{
	message += " (killed by " + user->getNick() + ")";
	std::string reply = ":" + serv->getUserMap()[id]->getNick() + "!" + serv->getUserMap()[id]->getUsername() + "@"
						+ serv->getUserMap()[id]->getHost() + " QUIT " + message + "\r\n";
	for (mChannel::iterator itC = serv->getChannelMap().begin();
		 itC != serv->getChannelMap().end(); ++itC)
	{
		itC->second->leaveChan(serv->getUserMap()[id]->getNick());
		for (std::vector<User*>::iterator itU = itC->second->getUsers().begin();
			 itU != itC->second->getUsers().end(); ++itU)
			send((*itU)->getId(), reply.c_str(), reply.size(), MSG_DONTWAIT);
	}
}

bool	isUser (Server *serv, std::string user, std::string why, int id_exec) {
	for (std::map<int, User*>::iterator it = serv->getUserMap().begin(); it!= serv->getUserMap().end(); it++) {
		if (user.compare(it->second->getNick()) == 0) {
			User *exec = serv->getUserMap()[id_exec];
			std::string message = ":" + exec->getNick() + "!" + exec->getUsername() + "@" +
			 serv->getUserMap()[id_exec]->getHost() + "!Unknown@" + serv->getHostname() +  " KILL " + user + " " + why + "\r\n";
			leaveChan(serv, why, it->first, exec);
			std::cout << "REPLY --- " << message;
			send(it->first, message.c_str(), message.length(), MSG_DONTWAIT);
			send_message(serv, exec->getId(), RPL_KILLDONE, user);
			serv->setKilled(it->first);
			return TRUE;
		}
	}
	return FALSE;
}

std::vector<std::string> splitArgsKill(std::string params, size_t end_pos) {
	std::size_t pos;
	std::vector<std::string> args;

	while ((pos = params.find(' ')) < end_pos) {
			args.push_back(params.substr(0, pos));
			params = params.substr(pos + 1);
			end_pos -= (pos + 1);
		}
		args.push_back(params);
	return args;
}

void KILL(Server *serv, std::string params, int id) {
	User *oper;
	if (serv->getUserMap().find(id) != serv->getUserMap().end())
		oper = serv->getUserMap()[id];
	else
		return; //error
	std::vector<std::string> args;
	size_t end_pos;
	if ((end_pos = params.find(':')) != std::string::npos)
		args = splitArgsKill(params, end_pos);
	else
		args = splitArgsKill(params, std::string::npos);
	if (args.size() != 2)
		return (send_message(serv, id, ERR_NEEDMOREPARAMS, ""));
	if (oper->getMode('r') == true)
		return (send_message(serv, id, ERR_RESTRICTED, ""));
	if (oper->getMode('o') == false)
	 	return (send_message(serv, id, ERR_NOPRIVILEGES, ""));
	if (isUser(serv, args[0], args[1], id) != TRUE) {
		return (send_message(serv, id, ERR_NOSUCHNICK, ""));
	}
	return;
}