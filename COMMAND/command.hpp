#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <vector>
#include <sstream>
#include <cctype>
#include <unistd.h>
#include "../RepliesCodeDef.hpp"
#include "../utils.hpp"

typedef std::map<std::string, Channel*> mChannel;

void	NICK(Server *server, std::string nick, int id);
void	user_cmd(Server *server, std::string params, int id);
void	PASS(Server *serv, std::string pass, int id);
void	MODE(Server *server, std::string params, int id);
void	OPER(Server *server, std::string params, int id);
void	PING(Server *server, std::string params, int id);
void	PRIVMSG(Server *server, std::string params, int id);
void	NOTICE(Server *server, std::string params, int id);
void	KICK(Server *serv, std::string params, int id);
void	KILL(Server *serv, std::string params, int id);
void	JOIN(Server *server, std::string params, int id);
void	TOPIC(Server *server, std::string params, int id);
void	NAMES(Server *server, std::string params, int id);
void	PART(Server *server, std::string params, int id);
void	QUIT(Server *server, std::string params, int id);
void	INVITE(Server *server, std::string params, int id);
void    WHO(Server *server, std::string params, int id);
void    CAP(Server *server, std::string params, int id);

#include "../Server.hpp"
#endif