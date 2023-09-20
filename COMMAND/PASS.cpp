#include "command.hpp"

void	PASS(Server *server, std::string pass, int id) {
	std::map<int, User*> clients = server->getUserMap();

	if (clients[id]->getMode('r') == false)
		return (send_message(server,id, ERR_ALREADYREGISTRED, ""));
	else if (!pass.size())
		return (send_message(server, id, ERR_NEEDMOREPARAMS, pass));
	else if (pass == server->getPwd())
		server->getUserMap()[id]->setMode('r', false);
	else if (pass == "BOT") {
		server->getUserMap()[id]->setBot();
		server->getUserMap()[id]->setMode('r', false);
	}
	else {
		send_message(server, id, ERR_PASSWDMISMATCH, "");
		return ;
	}
}