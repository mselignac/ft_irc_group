#include "command.hpp"

void    CAP(Server *server, std::string params, int id) {
    if (params != "LS")
        return;
    (void)server;
    (void)params;
    (void)id;
}