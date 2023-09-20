#include "../command.hpp"

void INVITE(Server *server, std::string params, int id)
{
	std::string target;
	std::string channel;

	size_t pos = params.find_first_of(" ");
	if (pos != std::string::npos) {
		target = params.substr(0, pos);
		channel = params.substr(pos + 1);
	} else {
		return (send_message(server, id, ERR_NORECIPIENT, ""));
	}

	User *user = server->getUserMapName()[target];
	if (!user) {
		return (send_message(server, id, ERR_NOSUCHNICK, target));
	}

	Channel *chan = server->getChannelMap()[channel];
	if (!chan) {
		return (send_message(server, id, ERR_NOSUCHCHANNEL, channel));
	}

	if (!chan->getMapUser()[id]) {
		return (send_message(server, id, ERR_NOTONCHANNEL, channel));
	}
	
	if (!chan->getOp(id)&& !server->getUserMap()[id]->getMode('o')) {
		return (send_message(server, id, ERR_CHANOPRIVSNEEDED, channel));
	}

	if (!chan->getMapUser()[user->getId()]) {
		std::string reply = ":" + server->getUserMap()[id]->getNick() + " INVITE " + target + " " + channel + "\r\n";
		send(user->getId(), reply.c_str(), reply.size(), MSG_DONTWAIT);
		JOIN(server, channel, user->getId());
	}
}