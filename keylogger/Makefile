# ****************************
#           WINKEY
# ****************************

NAME                = winkey.exe
LOGGER_LOG          = winkey.log

# ****************************
#       BUILD COMMANDS
# ****************************

CC                  = cl
LIBS                = user32.lib psapi.lib
CFLAGS              = /nologo /Wall /W3 /EHsc

# ****************************
#       FILE STRUCTURE
# ****************************

SRCS_DIR            = .
OBJS_DIR            = objs
INCS_DIR            = .

SRC_LOGGER1         = $(SRCS_DIR)\Winkey.cpp
SRC_LOGGER2         = $(SRCS_DIR)\keyMapping.cpp
SRC_LOGGER3         = $(SRCS_DIR)\main.cpp

OBJ_LOGGER1         = $(OBJS_DIR)\Winkey.obj
OBJ_LOGGER2         = $(OBJS_DIR)\keyMapping.obj
OBJ_LOGGER3         = $(OBJS_DIR)\main.obj
OBJS_LOGGER         = $(OBJ_LOGGER1) $(OBJ_LOGGER2) $(OBJ_LOGGER3)

INCS                = $(INCS_DIR)\winkey.hpp

# ****************************
#       BUILD RULES
# ****************************

all: $(NAME)

$(NAME): $(OBJS_LOGGER)
    $(CC) $(CFLAGS) /Fe$@ $(OBJS_LOGGER) $(LIBS)

$(OBJ_LOGGER1): $(SRC_LOGGER1) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_LOGGER1)

$(OBJ_LOGGER2): $(SRC_LOGGER2) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_LOGGER2)

$(OBJ_LOGGER3): $(SRC_LOGGER3) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_LOGGER3)

$(OBJS_DIR):
    if not exist $(OBJS_DIR) mkdir $(OBJS_DIR)

# ****************************
#       CLEAN RULES
# ****************************

clean:
    if exist $(OBJS_DIR) rmdir /S /Q $(OBJS_DIR)
    if exist $(LOGGER_LOG) del /Q $(LOGGER_LOG)

fclean: clean
    if exist $(NAME) del /Q $(NAME)

re: fclean all
