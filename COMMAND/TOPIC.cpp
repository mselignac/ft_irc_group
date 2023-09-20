#include "command.hpp"

void    TOPIC(Server *server, std::string params, int id) {
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

    bool exist = false;
    if (parts[0][0] != '#')
        return send_message(server, id, ERR_NOSUCHCHANNEL, "");

    for (mChannel::iterator iter = server->getChannelMap().begin(); iter != server->getChannelMap().end(); iter++)
        if (iter->first == parts[0])
            exist = true;

    if (!exist)
        return send_message(server, id, ERR_NOSUCHCHANNEL, "");

    else {
        exist = false;
        for (std::vector<User*>::iterator it = server->getChannelMap()[parts[0]]->getUsers().begin(); it != server->getChannelMap()[parts[0]]->getUsers().end(); ++it)
		    if (*it == server->getUserMap()[id])
			    exist = true;
        if (!exist)
            return send_chan_message(server, id, ERR_NOTONCHANNEL, "", parts[0]);
        if (parts.size() == 1) {
            if (server->getChannelMap()[parts[0]]->getTopic() == "")
                return send_chan_message(server, id, RPL_NOTOPIC, "", parts[0]);
            send_chan_message(server, id, RPL_TOPIC, server->getChannelMap()[parts[0]]->getTopic(), parts[0]);
            return send_chan_message(server, id, RPL_TOPICWHOTIME, server->getChannelMap()[parts[0]]->getLastTopicChanger(), parts[0]);
        }

        else {
            std::string topic = parts[1];
            topic = topic.substr(1);
            server->getChannelMap()[parts[0]]->setTopic(topic);
            server->getChannelMap()[parts[0]]->setLastTopicChanger(server->getUserMap()[id]->getNick());
            std::string message;
            for (std::vector<User*>::iterator it = server->getChannelMap()[parts[0]]->getUsers().begin(); it != server->getChannelMap()[parts[0]]->getUsers().end(); ++it) {
                message = ":" + server->getUserMap()[id]->getNick() + "!" + server->getUserMap()[id]->getUsername()  + "@" + server->getUserMap()[id]->getHost() + " TOPIC " + parts[0] + " :" + topic + "\r\n";
                std::cout << "DEBUG ----- " << message << std::endl;
                send((*it)->getId(), message.c_str(), message.length(), MSG_DONTWAIT);
            }
        }
    }
}