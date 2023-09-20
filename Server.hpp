#ifndef SERVER_HPP
# define SERVER_HPP



#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <cerrno>
#include <map>
#include <vector>
#include "User.hpp"
#include "Channel.hpp"
#include "COMMAND/command.hpp"
#include <ctime>

//#define PORT 12345
#define TRUE 1
#define FALSE 0
#define INACTIVE -1

class Server {
public:
	typedef std::map<int, User*>	mClient;
	typedef std::pair<int, User*>	pClient;
	typedef mClient::iterator		iterator;
	typedef std::map<std::string, Channel*>		mChannel;
	typedef std::pair<std::string, Channel*>	pChannel;
	typedef std::map<std::string, User*>		mUser;
	// typedef void (*FPTR)(Server, std::string, int);

private:
	struct sockaddr_in	_address;	// sd address
	std::string			_date; // date of creation of the server
	int					_sd;		// server socket descriptor
	int					_addrlen;	//= sizeof(_address);
	int					_port;		// port used for connection
	int					_killed;
	std::string			_pwd;		// password
	mClient				_clients;	// array of users
	std::string			_hostname;	//name of server
	std::string			_opPwd;
	std::string 		_keep;
	std::map<std::string, void(*)(Server*, std::string, int)> _commands;
	mChannel			_channels;

	Server();
public:
	Server(int port, std::string pwd);
	~Server();

	// CREATE SOCKET
	// void setSocket();
	// void bindSocket();
	// void listenSocket();

	void selectLoop();
	// void newUser(int fdcl);
	void readData();
	void socketError(std::string str);
	void receiveMessage(std::string buf, int id);
	void checkCmd();

	// GET
	int					getPort() const;
	const std::string	&getPwd() const;
	const std::string	&getDate() const;
	const std::string	&getHostname();
	void				setHostname(std::string name);
	const int			&getCount();
	mClient				&getUserMap();
	mUser				getUserMapName();
	const std::string	&getOpPwd();
	mChannel			&getChannelMap();
	void				setKilled(int dead);
	int					getKilled(void) const;
};

std::ostream& operator<<(std::ostream& output, const Server &sock);

#endif