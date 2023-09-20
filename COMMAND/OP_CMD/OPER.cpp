#include "../command.hpp"

void    OPER(Server *server, std::string params, int id) {
    std::vector<std::string> parts;
    std::stringstream ss(params);
    std::string s;

    if (server->getUserMap()[id]->getMode('r') == true)
		return (send_message(server, id, ERR_RESTRICTED, ""));

    while (std::getline(ss, s, ' '))
        parts.push_back(s);

    if (parts.size() < 2) {
        send_message(server, id, ERR_NEEDMOREPARAMS, "");
        return ;
    }

    std::string pwd = *(parts.begin() + 1);
    if (pwd != server->getOpPwd()) {
        send_message(server, id, ERR_PASSWDMISMATCH, "");
        return ;
    }
    std::string dest;
    if (parts[0][0] == '*')
        dest = server->getUserMap()[id]->getNick();
    else
        dest = *parts.begin();
    int idDest = searchNick(server, dest);
    if (idDest < 0)
        return ;
    server->getUserMap()[idDest]->setMode('o', true);
    if (server->getUserMap()[idDest]->getMode('o'))
        std::cout << "MODE : true" << std::endl;
    else
        std::cout << "MODE : false" << std::endl;
    send_message(server, idDest, RPL_YOUREOPER, "");
    send_message(server, idDest, RPL_UMODEIS, "");
}
