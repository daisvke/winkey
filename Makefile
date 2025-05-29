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
CFLAGS				= /nologo /Wall /WX


# ****************************
#       BUILD FILES
# ****************************

# Source files
SRCS_DIR			= srcs
SRC_SVC				= $(SRCS_DIR)\$(SVC).c
SRC_LOGGER			= $(SRCS_DIR)\$(LOGGER).c

# Obj files
OBJS_DIR			= objs
OBJ_SVC				= $(OBJS_DIR)\$(SVC).obj
OBJ_LOGGER			= $(OBJS_DIR)\$(LOGGER).obj

# Include files
INCS_DIR			= incs
INCS				=	$(INCS_DIR)\svc.h \
						$(INCS_DIR)\winkey.h \
						$(INCS_DIR)\ascii_format.h


# ****************************
#       BUILDING
# ****************************

.PHONY: all
all: $(SVC) $(LOGGER)

$(SVC): $(OBJ_SVC)
	$(CC) $(CFLAGS) /Fe$(SVC).exe $(OBJ_SVC)

$(LOGGER): $(OBJ_LOGGER)
	$(CC) $(CFLAGS) /Fe$(LOGGER).exe $(OBJ_LOGGER)

$(OBJ_SVC): $(SRC_SVC) $(INCS) $(OBJS_DIR)
	$(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_SVC)

$(OBJS_DIR):
	if not exist $(OBJS_DIR) mkdir $(OBJS_DIR)


# ****************************
#       CLEANING
# ****************************

clean:
	if exist $(OBJS_DIR) rmdir /S /Q $(OBJS_DIR)

fclean: clean
	if exist $(SVC).exe del /Q $(SVC).exe
	if exist $(LOGGER).exe del /Q $(LOGGER).exe

re: fclean all