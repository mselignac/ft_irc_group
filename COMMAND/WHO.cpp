#include "command.hpp"

void    WHO(Server *server, std::string params, int id) {
    std::vector<std::string> parts;
    std::stringstream ss(params);
    std::string s;

    if (server->getUserMap()[id]->getMode('r') == true)
		return (send_message(server, id, ERR_RESTRICTED, ""));

    while (std::getline(ss, s, ' '))
        parts.push_back(s);

    if (parts.size() < 1) {
        send_message(server, id, ERR_NEEDMOREPARAMS, "");
        return ;
    }

    std::map<int, User*> clients = server->getChannelMap()[parts[0]]->getMapUser();
    bool exist = false;
    Channel *chan;

    if (parts[0][0] == '#') {
        for (mChannel::iterator iter = server->getChannelMap().begin(); iter != server->getChannelMap().end(); iter++) {
            if (iter->first == parts[0]) {
                exist = true;
                chan = iter->second;
            }
        }
        if (!exist)
            return send_message(server, id, ERR_NOSUCHCHANNEL, parts[0]);

        std::map<int, User*> clients = server->getUserMap();

        std::map<int, User*> chanUsers = chan->getMapUser();
    
        std::string message;
        for (std::map<int, User*>::iterator it = chanUsers.begin(); it != chanUsers.end(); it++) {
            // message = ":" + clients[id]->getNick() + "!" + clients[id]->getNick()  + "@" + clients[id]->getHost()
            message = ":0.0.0.0 "
                        + parts[0] + " " + (*it).second->getUsername() + " " + (*it).second->getHost() + " "
                        + server->getHostname() + " " + (*it).second->getNick() + " ";

            for (std::vector<User *>::iterator iter = chan->getChanops().begin(); iter != chan->getChanops().end(); iter++) {
			    if ((*iter)->getNick() == (*it).second->getNick() || (*it).second->getMode('o'))
                    message += "@ ";

                message += ":0 " + (*it).second->getRealname() + "\r\n";
                send(id, message.c_str(), message.length(), MSG_DONTWAIT);
		    }
        }
        message = "0.0.0.0 " + parts[0] + " :End of WHO list\r\n";
        send(id, message.c_str(), message.length(), MSG_DONTWAIT);
        //<client> <channel> <username> <host> <server> <nick> <flags> :<hopcount> <realname>
    }
}