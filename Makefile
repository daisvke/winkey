# ****************************
#         TINKY-WINKEY
# ****************************

SVC					= svc
LOGGER				= winkey


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
CFLAGS				= /Wall /WX


# ****************************
#       BUILD FILES
# ****************************

# Source files
SRCS_DIR			= srcs/
SRC_SVC				= $(addprefix $(SRCS_DIR), $(SVC).c)	
SRC_LOGGER			= $(addprefix $(SRCS_DIR), $(LOGGER).c)

# Obj files
OBJS_DIR			= objs/
OBJ_SVC				= $(addprefix $(OBJS_DIR), $(SVC).obj)	
OBJ_LOGGER			= $(addprefix $(OBJS_DIR), $(LOGGER).obj)

# Include files
INCS_DIR			= incs/
INCS				= $(wildcard $(INCS_DIR)*.h)


# ****************************
#       BUILDING
# ****************************

.PHONY: all
all: $(SVC).exe $(LOGGER).exe

$(SVC).exe: $(OBJ_SVC)
	$(CC) $(CFLAGS) -o $@

$(OBJ_SVC): $(SRC_SVC)
	$(CC) $(CFLAGS) /C -o $<


# ****************************
#       CLEANING
# ****************************

clean:
	rmdir /S /Q $(OBJS_DIR)

fclean: clean
	del /Q $(SVC).EXE $(LOGGER).exe
