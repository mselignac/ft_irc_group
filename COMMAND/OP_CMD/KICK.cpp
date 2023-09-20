# include "../command.hpp"

int		isUser (Channel chan, std::string user) {
	std::vector<User*> vec_users = chan.getUsers();
	int i = 0;
	for (std::vector<User*>::iterator it = vec_users.begin(); it!= vec_users.end(); it++) {
		if (user.compare((*it)->getNick()) == 0) {
			return i;
			}
			i++;
		}
	return -1;
}

User	*isChanop(User oper, Channel chan) {
	std::vector<User*> vec = chan.getChanops();
	for (std::vector<User*>::iterator it = vec.begin(); it!= vec.end(); it++) {
		if (oper.getNick().compare((*it)->getNick()) == 0)
			return *it;
	}
	return NULL;
}

std::vector<std::string> splitArgs(std::string params, size_t end_pos) {
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


void	sendToChan(std::string message, Channel chan) {
	std::vector<User*> vec_users = chan.getUsers();
	std::vector<User*> vec_chanops = chan.getChanops();
	for (std::vector<User*>::iterator it = vec_users.begin(); it!= vec_users.end(); it++) {
		if ((*it) != NULL)
			send((*it)->getId(), message.c_str(), message.length(), MSG_DONTWAIT);
	}
}

void KICK(Server *serv, std::string params, int id) {
	User *exec;
	if (serv->getUserMap().find(id) != serv->getUserMap().end())
		exec = serv->getUserMap()[id];
	else
		return; //error
	if (exec->getMode('r') == true)
		return (send_message(serv, id, ERR_RESTRICTED, ""));

	std::vector<std::string> args;
	size_t end_pos;
	if ((end_pos = params.find(':')) != std::string::npos)
		args = splitArgs(params, end_pos);
	else
		args = splitArgs(params, std::string::npos);
	if (args.size() < 2)
		return (send_message(serv, id, ERR_NEEDMOREPARAMS, ""));


	Channel	*chan = NULL;
	User *chanoper = NULL;
	int kicked = -1;

	if ((serv->getChannelMap().find(args[0])) == serv->getChannelMap().end())
		return (send_message(serv, id, ERR_NOSUCHCHANNEL, ""));
	else
		chan = serv->getChannelMap()[args[0]];
	if ((chanoper = isChanop(*exec, *chan)) == NULL && !serv->getUserMap()[id]->getMode('o')) {
		return (send_message(serv, id, ERR_CHANOPRIVSNEEDED, chan->getName()));
	}
	else if ((kicked = isUser(*chan, args[1])) == -1) {
		return (send_message(serv, id, ERR_NOTONCHANNEL, ""));
	}

	std::string why = "";
	if (args.size() == 3)
		why = args[2];
	else
		why = "";

	std::string message = ":" + exec->getNick() + "!" +  exec->getUsername() + "@" +
	exec->getHost() + " KICK " + args[0] + " " + args[1] + " " + why + "\r\n";
	std::cout << "REPLY --- " << message;

	sendToChan(message, *chan);
	chan->leaveChan(args[1]);
}