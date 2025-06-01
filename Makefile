# ****************************
#         TINKY-WINKEY
# ****************************

SVC                 = svc
LOGGER              = winkey

# ****************************
#       BUILD COMMANDS
# ****************************

CC                  = cl
LIBS                = user32.lib psapi.lib
CFLAGS              = /nologo /Wall /WX /EHsc /std:c++17 /wd5039 /wd4668

# ****************************
#       FILE STRUCTURE
# ****************************

SRCS_DIR            = srcs
INCS_DIR            = incs
OBJS_DIR            = objs

SRC_SVC             = $(SRCS_DIR)\$(SVC).cpp
SRC_LOGGER1         = $(SRCS_DIR)\$(LOGGER).cpp
SRC_LOGGER2         = $(SRCS_DIR)\keyMapping.cpp

OBJ_SVC             = $(OBJS_DIR)\$(SVC).obj
OBJ_LOGGER1         = $(OBJS_DIR)\$(LOGGER).obj
OBJ_LOGGER2         = $(OBJS_DIR)\keyMapping.obj
OBJS_LOGGER         = $(OBJ_LOGGER1) $(OBJ_LOGGER2)

INCS                = $(INCS_DIR)\svc.hpp $(INCS_DIR)\winkey.hpp

# ****************************
#       BUILD RULES
# ****************************

.PHONY: all

all: $(SVC).exe $(LOGGER).exe

# ---- SVC ----
$(SVC).exe: $(OBJ_SVC)
    $(CC) $(CFLAGS) /Fe$(SVC).exe $(OBJ_SVC) $(LIBS)

$(OBJ_SVC): $(SRC_SVC) $(INCS) | $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_SVC)

# ---- LOGGER ----
$(LOGGER).exe: $(OBJS_LOGGER)
    $(CC) $(CFLAGS) /Fe$(LOGGER).exe $(OBJS_LOGGER) $(LIBS)

$(OBJ_LOGGER1): $(SRC_LOGGER1) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_LOGGER1)

$(OBJ_LOGGER2): $(SRC_LOGGER2) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_LOGGER2)

# ---- OBJ DIR ----
$(OBJS_DIR):
    if not exist $(OBJS_DIR) mkdir $(OBJS_DIR)

# ****************************
#       CLEAN RULES
# ****************************

.PHONY: clean fclean re

clean:
    if exist $(OBJS_DIR) rmdir /S /Q $(OBJS_DIR)

fclean: clean
    if exist $(SVC).exe del /Q $(SVC).exe
    if exist $(LOGGER).exe del /Q $(LOGGER).exe

re: fclean all
