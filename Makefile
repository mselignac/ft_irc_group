NAME = ircserv

BOT = ircbot

SRCS_BOT = bot.cpp

SRCS = main1.cpp \
		Server.cpp \
		User.cpp \
		COMMAND/USER.cpp \
		COMMAND/PASS.cpp \
		COMMAND/NICK.cpp \
		COMMAND/PING.cpp \
		COMMAND/PRIVMSG.cpp \
		COMMAND/NOTICE.cpp \
		COMMAND/JOIN.cpp \
		COMMAND/QUIT.cpp \
		COMMAND/TOPIC.cpp \
		COMMAND/NAMES.cpp \
		COMMAND/WHO.cpp \
		COMMAND/CAP.cpp \
		COMMAND/OP_CMD/PART.cpp \
		COMMAND/OP_CMD/INVITE.cpp \
		COMMAND/OP_CMD/MODE.cpp \
		COMMAND/OP_CMD/OPER.cpp \
		COMMAND/OP_CMD/KILL.cpp \
		COMMAND/OP_CMD/KICK.cpp \
		utils.cpp \
		Channel.cpp


OBJS = ${SRCS:.cpp=.o}

OBJS_BOT = ${SRCS_BOT:.cpp=.o}

#INCLUDES=

CXX = c++
#CC = cc

CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g3 #-fsanitize=address
#CFLAGS = -Wall -Wextra -Werror

all: ${NAME}

${NAME}: ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${NAME}

irbot: ${BOT}

${BOT}: ${OBJS_BOT}
	${CXX} ${CXXFLAGS} ${OBJS_BOT} -o ${BOT}

clean:
	rm -f ${OBJS} *.o

fclean: clean
	rm -f ${NAME} ${BOT}

re: fclean all