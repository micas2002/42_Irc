NAME = ircserv

HEADERS_LIST = Server.hpp User.hpp Channel.hpp
HEADERS_DIRECTORY = ./includes/
HEADERS = $(addprefix $(HEADERS_DIRECTORY), $(HEADERS_LIST))

SOURCES_LIST = main.cpp Server.cpp User.cpp Channel.cpp Sockets.cpp
SOURCES_DIRECTORY = ./sources/
SOURCES = $(addprefix $(SOURCES_DIRECTORY), $(SOURCES_LIST))

OBJECTS_LIST = $(patsubst %.cpp, %.o, $(SOURCES_LIST))
OBJECTS_DIRECTORY = objects/
OBJECTS = $(addprefix $(OBJECTS_DIRECTORY), $(OBJECTS_LIST))

CC = c++
FLAGS = -Wall -Werror -Wextra -Wshadow -std=c++98 -g3

INCLUDES = -I $(HEADERS_DIRECTORY)

# ECHO

ifeq "$(TERM)" "xterm-256color"
	ECHO = echo
else
	ECHO = echo -e
endif

# COLORS
RESET		=	\033[0m
BLACK		=	\033[30m
RED			=	\033[31m
GREEN		=	\033[32m
YELLOW		=	\033[33m
BLUE		=	\033[34m
MAGENTA		=	\033[35m
CYAN		=	\033[36m
WHITE		=	\033[37m
BOLDBLACK	=	\033[1m\033[30m
BOLDRED		=	\033[1m\033[31m
BOLDGREEN	=	\033[1m\033[32m
BOLDYELLOW	=	\033[1m\033[33m
BOLDBLUE	=	\033[1m\033[34m
BOLDMAGENTA	=	\033[1m\033[35m
BOLDCYAN	=	\033[1m\033[36m
BOLDWHITE	=	\033[1m\033[37m


# RULES

all: ## Compiles the whole project
	@make -s $(NAME)

$(NAME): $(OBJECTS_DIRECTORY) $(OBJECTS)
	@$(ECHO) "\n\nStarting compile, now have some $(BLUE)colorful $(RED)text $(YELLOW)slayyyy$(RESET)"
	@$(CC) $(FLAGS) $(INCLUDES) $(OBJECTS) -o $(NAME)
	@$(ECHO) "$(CYAN)$(NAME):$(RESET) $(GREEN)object files$(RESET) were created and have come to free us, $(YELLOW)monkeys$(RESET) united."
	@$(ECHO) "$(CYAN)$(NAME):$(RESET) $(GREEN)$(NAME)$(RESET) was created, like $(BLUE)god$(RESET) intended"
	@$(ECHO) "$(CYAN)$(NAME):$(RESET) now watch this video : $(RED)https://www.youtube.com/watch?v=xvFZjo5PgG0$(RESET)" 

$(OBJECTS_DIRECTORY): ## Creates the objects directory
	@mkdir -p $(OBJECTS_DIRECTORY)
	@$(ECHO) "$(CYAN)$(NAME): $(GREEN)$(OBJECTS_DIRECTORY)$(RESET) was created $(YELLOW)WOW $(BLUE)OMG$(RESET) i'm so $(GREEN)talented$(RESET)"

$(OBJECTS_DIRECTORY)%.o : $(SOURCES_DIRECTORY)%.cpp ## Compiles each '*.cpp'
	@$(CC) $(FLAGS) -c $(INCLUDES) $< -o $@
	@$(ECHO) -n "$(GREEN).$(RESET)"

clean: ## Cleans all the '*.o' files and deletes the objects directory
	@rm -rf $(OBJECTS_DIRECTORY)

fclean: clean ## Calls clean and deletes all executables and compiled libs
	@rm -f $(NAME)

re: ## Deep cleans the project and recompiles it
	@make -s fclean
	@make -s all
	
help: ## Shows all the options and it's uses
	@echo "usage: make [target]"
	@echo ""
	@egrep "^(.+)\:\ .*##\ (.+)" ${MAKEFILE_LIST} | sed 's/:.*##/#/' | column -t -c 2 -s '#'
	@curl -L https://raw.githubusercontent.com/keroserene/rickrollrc/master/roll.sh | bash

.PHONY	: re fclean clean all