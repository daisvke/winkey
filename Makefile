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
SRC_SVC             = $(SRCS_DIR)\$(SVC).cpp
SRC_LOGGER          = $(SRCS_DIR)\$(LOGGER).cpp

OBJS_DIR            = objs
OBJ_SVC             = $(OBJS_DIR)\$(SVC).obj
OBJ_LOGGER          = $(OBJS_DIR)\$(LOGGER).obj

INCS_DIR            = incs
INCS                = $(INCS_DIR)\svc.hpp \
                      $(INCS_DIR)\winkey.hpp


# ****************************
#       BUILD RULES
# ****************************

.PHONY: all

all: $(SVC).exe $(LOGGER).exe

# ---- SVC ----
$(SVC).exe: $(OBJ_SVC)
    $(CC) $(CFLAGS) /Fe$(SVC).exe $(OBJ_SVC) $(LIBS)

$(OBJ_SVC): $(SRC_SVC) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_SVC)

# ---- LOGGER ----
$(LOGGER).exe: $(OBJ_LOGGER)
    $(CC) $(CFLAGS) /Fe$(LOGGER).exe $(OBJ_LOGGER) $(LIBS)

$(OBJ_LOGGER): $(SRC_LOGGER) $(INCS) $(OBJS_DIR)
    $(CC) $(CFLAGS) /I$(INCS_DIR) /c /Fo$@ $(SRC_LOGGER)

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
