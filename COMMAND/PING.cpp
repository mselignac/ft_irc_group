#include "command.hpp"

void	PING(Server *server, std::string params, int id) {
    std::vector<std::string> parts;
    std::stringstream ss(params);
    std::string s;

    if (server->getUserMap()[id]->getMode('r') == true)
		return (send_message(server, id, ERR_RESTRICTED, ""));
    if (params.size() < 1)
        return (send_message(server, id, ERR_NEEDMOREPARAMS, ""));
    
    while (std::getline(ss, s, ' '))
        parts.push_back(s);

    std::string ret = ":" + server->getUserMap()[id]->getNick() + "!" + server->getUserMap()[id]->getUsername()  + "@" + server->getUserMap()[id]->getHost() + " PONG " + ":"  + parts[0] + " \r\n";
    std::cout << "REPLY --- " << ret << std::endl;
    send(id, ret.c_str(), ret.length(), MSG_DONTWAIT);
}