#include "../command.hpp"

void    MODE_CHAN(Server *server, std::vector<std::string> parts, int id, int nbUser) {
    std::string message;
    User *targetPtr;    

    Channel *chan = server->getChannelMap()[parts[0]];
    bool op = false;
    for (std::vector<User *>::iterator it = chan->getChanops().begin(); it != chan->getChanops().end(); it++) {
		if ((*it)->getNick() == server->getUserMap()[id]->getNick() || server->getUserMap()[id]->getMode('o')) {
			op = true;
            break;
        }
	}

    if (!op && !server->getUserMap()[id]->getMode('o'))
        return send_message(server, id, ERR_CHANOPRIVSNEEDED, chan->getName());

    if ((nbUser == 1 && parts.size() < 3) || (nbUser == 2 && parts.size() < 4) || (nbUser == 3 && parts.size() < 5))
        return send_message(server, id, ERR_NEEDMOREPARAMS, "");
    std::map<int, User *> usermap = chan->getMapUser();

    for (int i = 0; i < nbUser; i++) {
        int target = 0;
        for (std::map<int, User *>::iterator iter = usermap.begin(); iter != usermap.end(); iter++) {
            if ((*iter).second->getNick() == parts[2 + i]) {
                target = (*iter).second->getId();
                targetPtr = (*iter).second;
            }
        }
        if (target == 0)
            return send_chan_message(server, id, ERR_USERNOTINCHANNEL, parts[2 + i], chan->getName());

        if (parts[1][2 * i] == '+') {
            chan->getChanops().push_back(server->getUserMap()[target]);
            message = ":" + server->getUserMap()[id]->getNick() + "!" + server->getUserMap()[id]->getUsername() +
                "@" + server->getUserMap()[id]->getHost() + " MODE " + parts[0] + " +o " + targetPtr->getNick() + "\r\n";
            std::vector<User *> vec_users = server->getChannelMap()[chan->getName()]->getUsers();
            for (std::vector<User *>::iterator it2 = vec_users.begin(); it2 != vec_users.end(); it2++) 
                send((*it2)->getId(), message.c_str(), message.length(), MSG_DONTWAIT);
            // std::cout << "REPLY CHAN --- " << message << std::endl;
            continue ;
        }
        else {
            // std::cout << "YOOOOO" << std::endl;
            for (std::vector<User *>::iterator iter = chan->getChanops().begin(); iter != chan->getChanops().end(); iter++) {
                if ((*iter)->getNick() == parts[2 + i]) {
                    chan->getChanops().erase(iter);
                    message = ":" + server->getUserMap()[id]->getNick() + "!" + server->getUserMap()[id]->getUsername()  +
                        "@" + server->getUserMap()[id]->getHost() + " MODE " + parts[0] + " -o " + targetPtr->getNick() + "\r\n";
                    std::vector<User *> vec_users = server->getChannelMap()[chan->getName()]->getUsers();
                    for (std::vector<User *>::iterator it2 = vec_users.begin(); it2 != vec_users.end(); it2++)
                        send((*it2)->getId(), message.c_str(), message.length(), MSG_DONTWAIT);
                    // std::cout << "REPLY CHAN --- " << message << std::endl;
                    break ;
                }
            }
        }
    }
}

void    MODE_USER(Server *server, std::vector<std::string> parts, int id) {
    std::string message;
    bool positive;
    std::string flags;

    flags = parts[1];
    if (flags.at(0) == '+')
        positive = true;
    else if (flags.at(0) == '-')
        positive = false;
    else {
        send_message(server, id, ERR_UMODEUNKNOWNFLAG, "");
        return ;
    }

    if (*parts.begin() == "*")
        *parts.begin() = server->getUserMap()[id]->getNick();

    if (*parts.begin() != server->getUserMap()[id]->getNick()) {
        send_message(server, id, ERR_USERSDONTMATCH, "");
        return ;
    }

    for (std::string::iterator it = flags.begin(); it != flags.end(); it++) {
        if (*it == 'o' && positive)
            (void)it;
        else if (*it == 'o' && !positive) {
            server->getUserMap()[id]->setMode('o', false);
        }
        else if (*it == 'r' && positive)
            (void)it;
        else if (*it == 'r' && !positive)
            server->getUserMap()[id]->setMode('r', true);
        else if (*it == 'i' && positive)
            server->getUserMap()[id]->setMode('i', true);
        else if (*it == 'i' && !positive)
            server->getUserMap()[id]->setMode('i', false);
    }
    send_message(server, id, RPL_UMODEIS, "");
}

void    MODE(Server *server, std::string params, int id) {
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

    if (parts.size() == 1 && parts[0][0] != '#') {
        send_message(server, id, RPL_UMODEIS, "");
        return ;
    }
    else if (parts.size() == 1 && parts[0][0] == '#') {
        return ;
    }

    if (parts[0][0] == '#'){
        if (server->getChannelMap().find(parts[0]) == server->getChannelMap().end())
            return send_chan_message(server, id, ERR_NOSUCHCHANNEL, "", parts[0]);

        Channel *chan = server->getChannelMap()[parts[0]];
        switch(parts[1].size()) {
            case 2: {
                if ((parts[1][0] != '+' && parts[1][0] != '-') || parts[1][1] != 'o') {
                    // std::cout << parts[1] << std::endl;
                    return send_chan_message(server, id, ERR_UNKNOWNMODE, parts[1], chan->getName());
                }
                MODE_CHAN(server, parts, id, 1); break;
            }
            case 4:{
                if (((parts[1][0] != '+' && parts[1][0] != '-') || parts[1][1] != 'o') || ((parts[1][2] != '+' && parts[1][2] != '-') ||
                    parts[1][3] != 'o'))
                    return send_chan_message(server, id, ERR_UNKNOWNMODE, parts[1], chan->getName());
                MODE_CHAN(server, parts, id, 2); break;
            }
            case 6: {
                if (((parts[1][0] != '+' && parts[1][0] != '-') || parts[1][1] != 'o') || ((parts[1][2] != '+' && parts[1][2] != '-') ||
                    parts[1][3] != 'o') || ((parts[1][4] != '+' && parts[1][4] != '-') || parts[1][5] != 'o'))
                    return send_chan_message(server, id, ERR_UNKNOWNMODE, parts[1], chan->getName());
                MODE_CHAN(server, parts, id, 3); break;
            }
            default: {
                return send_chan_message(server, id, ERR_UNKNOWNMODE, parts[1], chan->getName()); break;
            }
        }
    }

    else {
        MODE_USER(server, parts, id);
    }
}