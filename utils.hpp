#ifndef UTILS_HPP
# define UTILS_HPP

# include "RepliesCodeDef.hpp"
# include <sstream>

class Server;

bool    isspecial(char c);
bool    isforbidenuser(char c);
int     searchNick(Server *server, std::string name);
void    send_message(Server *server, int id_cli, int code, std::string str);
void    send_chan_message(Server *server, int id_cli, int code, std::string str, std::string chan);

# include "Server.hpp"
#endif