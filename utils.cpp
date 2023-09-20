#include "utils.hpp"

bool    isspecial(char c)
{
    if (c != '[' && c != ']' && c != '\\' && c != '`' && c != '_' && c != '^' && c != '{' && c != '|' && c != '}')
        return (false);
    return (true);
}

bool    isforbidenuser(char c)
{
    if (c == '\0' || c == ' ' || c == '\r' || c == '\n' || c == '@')
        return (true);
    return (false);
}

int     searchNick(Server *server, std::string name) {
    for (std::map<int, User*>::iterator it = server->getUserMap().begin(); it != server->getUserMap().end(); it ++) {
        if (name == (*it).second->getNick())
            return (*it).first;
    }
    return -1;
}

std::string mode_str(Server *server, int id_cli) {
    std::string ret = "";
    if (server->getUserMap()[id_cli]->getMode('r'))
        ret += "r";
    if (server->getUserMap()[id_cli]->getMode('o'))
        ret += "o";
    if (server->getUserMap()[id_cli]->getMode('i'))
        ret += "i";
    return ret;
}

void    send_message(Server *server, int id_cli, int code, std::string str) {
    std::string realCode;
    std::stringstream ss;
    ss << code;
    std::string codeStr = ss.str();

    if (code < 10)
        realCode = std::string(2, '0').append(codeStr);
    else if (code < 100)
        realCode = std::string(1, '0').append(codeStr);
    else
        realCode = codeStr;

    std::string message = ":" + server->getHostname() + " " + realCode;
    if (server->getUserMap()[id_cli]->getNick().empty())
        message += " * ";
    else
        message += " " + server->getUserMap()[id_cli]->getNick() + " ";

    switch (code) {

        // REPLIES
        case RPL_WELCOME:
            message += ":Welcome to our IRC server " + server->getUserMap()[id_cli]->getNick() + " !!"; break;
        case RPL_YOURHOST:
            message += ":Your host is " + server->getHostname() + ", running version Alpha 0.1"; break;
        case RPL_CREATED:
            message += ":This server was created " + server->getDate(); break;
        case RPL_MYINFO:
            message += server->getHostname() + " Alpha 0.4 " +
            "by a magnificent team composed of\n   *** MSELIGNA ***\n   *** CHDESPON ***\n   *** ELOUCHEZ ***"; break;
        case RPL_UMODEIS:
            message += mode_str(server, id_cli); break;
        case RPL_YOUREOPER:
            message += ":You are now an IRC operator"; break;

        // ERRORS
        case ERR_UNKNOWNCOMMAND:
            message += str + " :Unknown command"; break;
        case ERR_NONICKNAMEGIVEN:
            message += ":No nickname given"; break;
        case ERR_ERRONEUSNICKNAME:
            message += str + " :Erroneous nickname"; break;
        case ERR_NICKNAMEINUSE:
            message += str + " :Nickname is already in use"; break;
        case ERR_NOTREGISTERED:
            message += ":You have not registered"; break;
        case ERR_NEEDMOREPARAMS:
            message += str + " :Not enough parameters"; break;
        case ERR_ALREADYREGISTRED:
            message += ":Unauthorized command (already registered)"; break;
        case ERR_PASSWDMISMATCH:
            message += ":Password incorrect"; break;
        case ERR_RESTRICTED:
            message += ":Your connection is restricted!"; break;
        case ERR_UMODEUNKNOWNFLAG:
            message += ":Unknown MODE flag"; break;
        case ERR_USERSDONTMATCH:
            message += ":Cannot change mode for other users"; break;
        case ERR_NOTONCHANNEL:
            message += ":The user is not on the channel"; break;
        case ERR_CHANOPRIVSNEEDED:
            message += str + " :You need channel operator privileges to use this command"; break;
        case ERR_NOPRIVILEGES:
            message += ":You need operator privileges to use this command"; break;
        case ERR_NOSUCHNICK:
            message += ":This nickname does not exist"; break;
        case ERR_NOSUCHCHANNEL:
            message += str + " :No such channel"; break;
        case RPL_KILLDONE:
            message += str + " :has been killed"; break;
        case RPL_TRYAGAIN:
            message.erase(message.find('*'));
            message += str + " :Try again..."; break;
        case ERR_NORECIPIENT:
            message += ":No recipient given " + str; break;
        case ERR_NOTEXTTOSEND:
            message += ":No text to send"; break;
    }

    message += "\r\n";
    std::cout << "REPLY --- " << message << std::endl;
    send(id_cli, message.c_str(), message.length(), MSG_DONTWAIT);
}

void    send_chan_message(Server *server, int id_cli, int code, std::string str, std::string chan) {
    std::string realCode;
    std::stringstream ss;
    ss << code;
    std::string codeStr = ss.str();

    if (code < 10)
        realCode = std::string(2, '0').append(codeStr);
    else if (code < 100)
        realCode = std::string(1, '0').append(codeStr);
    else
        realCode = codeStr;

    std::string message = ":" + server->getHostname() + " " + realCode;
    if (server->getUserMap()[id_cli]->getNick().empty())
        message += " * ";
    else
        message += " " + server->getUserMap()[id_cli]->getNick() + " ";

    switch (code) {

        // REPLIES
        case RPL_NOTOPIC:
            message += chan + " :No topic is set"; break;
        case RPL_TOPIC:
            message += chan + " :" + str; break;
        case RPL_TOPICWHOTIME:
            message += chan + " " + str; break;
        case RPL_NAMREPLY:
            message += " = " + chan + " " + str; break;
        case RPL_ENDOFNAMES:
            message += chan + " :End of /NAMES list"; break;

        // ERRORS
        case ERR_NOSUCHCHANNEL:
            message += chan + " :No such channel"; break;
        case ERR_USERNOTINCHANNEL:
            message += str + " " + chan + " :They aren't on that channel"; break;
        case ERR_NOTONCHANNEL:
            message += chan + " :You're not on that channel"; break;
        case ERR_UNKNOWNMODE:
            message += str + " :is unknown mode char to me for " + chan; break;
    }

    message += "\r\n";
    std::cout << "REPLY --- " << message << std::endl;
    send(id_cli, message.c_str(), message.length(), MSG_DONTWAIT);
}