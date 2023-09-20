#include "Server.hpp"

Server::Server(int port, std::string pwd) :_port(port), _pwd(pwd), _hostname("0.0.0.0"), _opPwd("AUPP"), _keep("") {
	_commands["KICK"] = KICK;
	_commands["KILL"] = KILL;
	_commands["kill"] = KILL;
	_commands["MODE"] = MODE;
	_commands["OPER"] = OPER;
	_commands["CAP"] = 	CAP;
	_commands["JOIN"] = JOIN;
	_commands["NICK"] = NICK;
	_commands["PASS"] = PASS;
	_commands["USER"] = user_cmd;
	_commands["PING"] = PING;
	_commands["PRIVMSG"] = PRIVMSG;
	_commands["TOPIC"] = TOPIC;
	_commands["NAMES"] = NAMES;
	_commands["PART"] = PART;
	_commands["QUIT"] = QUIT;
	_commands["INVITE"] = INVITE;
	_commands["NOTICE"] = NOTICE;
	_channels["init"] = new Channel;
	_channels["init"]->setName("init");

	time_t now = time(0);
	_date = ctime(&now);

	int on = 1;
	_addrlen = sizeof(_address);

	// Create a communication endpoint in TCP IPv4
	if ((_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	// Once the listening sd is bound to a port, no other local address can bind to the port
	if (setsockopt(_sd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
		socketError("setsockopt() failed");

	// Sets the non_blocking option on the sd and all of which will derived from it
	// if (fcntl(_sd, F_SETFL, O_NONBLOCK) < 0)
	// 	socketError("fcntl() failed");

	// Initialize sockaddr_in struct
	memset((char*)&_address, 0, _addrlen);
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_address.sin_port = htons(_port);

	// Bind the sd to an address
	if (bind(_sd, (struct sockaddr *)&_address, _addrlen) < 0)
		socketError("bind() failed");

	// Sets the sd in a passive mode where it can accept incoming connections requests
	if (listen(_sd, 3) < 0)
		socketError("listen() failed");
 	std::cout << "Listening...\n";

}

Server::~Server() {

}

void Server::selectLoop() {
	fd_set rfds;
	struct timeval tv;
	int retval;
	int sd_max;
	int curr_sd;
	sockaddr_in *cliAddress;
	sockaddr_storage address;
	socklen_t addrlen = sizeof(address);


	// select() loop
	while (TRUE) {

	tv.tv_sec = 2;
	tv.tv_usec = 0;
	FD_ZERO(&rfds);
	FD_SET(_sd,&rfds);
	sd_max = _sd;

	for (iterator it = _clients.begin(); it != _clients.end(); it++) {
		curr_sd = it->first;
		if (curr_sd > 0)
			FD_SET(curr_sd, &rfds);
		if (curr_sd > sd_max)
			sd_max = curr_sd;
	}

	retval = select(sd_max + 1, &rfds, NULL, NULL, &tv);

	if (retval < 0 && errno != EINTR)
		perror("select() failed\n");
	//check if fds are register and so, active
	if (FD_ISSET(_sd, &rfds)) {
		int fdcl;
		if ((fdcl = accept(_sd, (sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
			socketError("accept() failed\n");

		//adding a new user
		cliAddress = (sockaddr_in *) &address;
		_clients[fdcl] = new User;
		_clients[fdcl]->setHost(std::string(inet_ntoa(cliAddress->sin_addr)));
		_clients[fdcl]->setId(fdcl);
		std::cout << "\nAdding new user with fd: " << fdcl << "\nNumber of users: " << _clients.size() << std::endl << std::endl;
	}

	for (iterator it = _clients.begin(); it != _clients.end(); it++) {
		curr_sd = it->first;
		char buf[1025];
		if (FD_ISSET(curr_sd, &rfds)) {
			int valread;
			if ((valread = read(curr_sd, buf, 1024)) == 0) {
				std::cout << "User " << it->second->getNick() << " with fd " << curr_sd << " disconnected\n";
				QUIT(this, ":Lost terminal", curr_sd);
				close(curr_sd);
				delete _clients[curr_sd];
				_clients.erase(curr_sd);
				std::cout << "Number of users: " << _clients.size() << std::endl;
				for (mChannel::iterator it = _channels.begin(); it != _channels.end(); it ++) {
					if (it->first.compare("init") != 0 && ((*it).second->getMapUser().empty() || (it->second->getMapUser().size() == 1 &&
						it->second->getMapUser().begin()->second->getBot() == true))) {
						if (it->second->getMapUser().size() == 1)
							PART(this, it->second->getName(), it->second->getMapUser().begin()->second->getId());
						delete (*it).second;
						_channels.erase(it);
						break;
					}
				}
				break;
			}
			else {
				buf[valread] = '\0';
				_killed = -1;
				receiveMessage(buf, curr_sd);
				for (mChannel::iterator it = _channels.begin(); it != _channels.end(); it ++) {
					if (it->first.compare("init") != 0 && (it->second->getMapUser().empty() || (it->second->getMapUser().size() == 1 &&
						it->second->getMapUser().begin()->second->getBot() == true))) {
						if (it->second->getMapUser().size() == 1)
							PART(this, it->second->getName(), it->second->getMapUser().begin()->second->getId());
						delete it->second;
						_channels.erase(it);
						break;
					}
				}
				if (_killed != -1) {
					std::cout << "User " << _clients[_killed]->getNick() << " with fd " << _killed << " disconnected\n";
					_clients[_killed]->setNick("");
					close(_killed); // not curr but nick
					delete _clients[_killed];
					_clients.erase(_killed);
					std::cout << "Number of users: " << _clients.size() << std::endl;
					break;
				}
				std::cout << *_clients[curr_sd];
			}
		}
	}
	}
}

void Server::receiveMessage(std::string buf, int id) {
	std::string s;
	std::string cmd;
	std::string args;
	std::size_t pos;
	std::size_t poscmd;

	_keep += buf;

	while ((pos = _keep.find("\r\n")) != std::string::npos) {
		s = _keep.substr(0, pos);
		_keep.erase(0, pos + 2);
		if ((poscmd = s.find(' ')) != std::string::npos) {
			cmd = s.substr(0, poscmd);
			args = s.substr(poscmd + 1);
		}
		else {
			args = "";
			cmd = s;
		}
		std::cout << "COMMAND RECEPTION --- "<< cmd << " " << args << std::endl;
		if (_commands[cmd]) {
			_commands[cmd](this, args, id);
		}
		else
			send_message(this, id, ERR_UNKNOWNCOMMAND, cmd);
	}
}

void Server::readData() {

}

void Server::socketError(std::string str) {
	perror(str.c_str());
	close(_sd);
	exit(EXIT_FAILURE);
}

int Server::getPort() const {
	return _port;
}

const std::string &Server::getPwd() const {
	return _pwd;
}

const std::string	&Server::getHostname() {
	return _hostname;
}

void			Server::setHostname(std::string name) {
	_hostname = name;
}

std::map<int, User*>	&Server::getUserMap() {
	return _clients;
}

std::map<std::string, User*>	Server::getUserMapName()
{
	mUser map;

	for (mClient::iterator it = _clients.begin(); it != _clients.end(); ++it)
		map[(*it).second->getNick()] = (*it).second;
	return (map);
}

const std::string		&Server::getOpPwd() {
	return _opPwd;
}

std::map<std::string, Channel*> &Server::getChannelMap() {
	return _channels;
}

int Server::getKilled(void) const {return (_killed);}

void		Server::setKilled(int dead) {
	_killed = dead;
}

const std::string &Server::getDate() const {return (_date);}

std::ostream& operator<<(std::ostream& output, const Server &sock) {
	output << "port = " << sock.getPort() << std::endl;
	output << "password = " << sock.getPwd() << std::endl;
	return (output);
}