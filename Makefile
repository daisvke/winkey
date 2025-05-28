# ****************************
#         TINKY-WINKEY
# ****************************

NAME				= svc.exe


# ****************************
#       ANSI ESCAPE CODES
# ****************************

# ANSI escape codes for stylized output
RESET 				= \033[0m
GREEN				= \033[32m
YELLOW				= \033[33m
RED					= \033[31m

# Logs levels
INFO 				= $(YELLOW)[INFO]$(RESET)
ERROR				= $(RED)[ERROR]$(RESET)
DONE				= $(GREEN)[DONE]$(RESET)


# ****************************
#       BUILD COMMANDS
# ****************************

CC					= cl
CFLAGS				= /Wall /WX /Fe


# ****************************
#       BUILD FILES
# ****************************



# ****************************
#       BUILDING
# ****************************

.PHONY: all
all: $(NAME)

$(NAME): 
