#include "command.hpp"

void    user_cmd(Server *server, std::string params, int id) {
    std::vector<std::string> parts;
    std::stringstream ss(params);
    std::string s;
    
    if (server->getUserMap()[id]->getRegistered() == true)
		return (send_message(server, id, ERR_ALREADYREGISTRED, ""));

    while (std::getline(ss, s, ' '))
        parts.push_back(s);

    if (parts.size() < 4 || parts[3][0] != ':') {
        send_message(server, id, ERR_NEEDMOREPARAMS, "");
        return ;
    }
    std::string name;
    std::vector<std::string>::iterator it = parts.begin() + 3;
    for (int i = 0; it != parts.end(); i++) {
        name += *it;
        if (i == 0) {
            name += " ";
            name = name.substr(1);
        }
        it++;
    }
    std::string username = parts[0];
    std::string host = parts[2];
    std::string realname = name;

    server->getUserMap()[id]->setUsername(username);
    server->getUserMap()[id]->setRealname(realname);

    server->getUserMap()[id]->setRegistered();
    if (server->getUserMap()[id]->getNick() != "" && server->getUserMap()[id]->getComplete() == false) {
        send_message(server, id, RPL_WELCOME, "");
        send_message(server, id, RPL_YOURHOST, "");
        send_message(server, id, RPL_CREATED, "");
        send_message(server, id, RPL_MYINFO, "");
        server->getUserMap()[id]->setComplete();
    }
    // std::string message = ":0.0.0.0 433 * plouf :Nickname is already in use\r\n";
    // send(id, message.c_str(), message.length(), MSG_DONTWAIT);
}