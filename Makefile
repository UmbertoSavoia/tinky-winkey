TARGET_SERVICE = svc.exe
TARGET_KEYLOGGER = winkey.exe

CC = cl
LINK = link

SRC_SERVICE = .\service\src\main.c
SRC_KEYLOGGER = .\keylogger\src\main.c

OBJ_SERVICE = $(SRC_SERVICE:.c=.obj)
OBJ_KEYLOGGER = $(SRC_KEYLOGGER:.c=.obj)

.c.obj:
	$(CC) /nologo /c $< /Fo: $@

all: $(TARGET_SERVICE) $(TARGET_KEYLOGGER)

$(TARGET_SERVICE): $(OBJ_SERVICE)
	$(LINK) /nologo $(OBJ_SERVICE) /OUT:$(TARGET_SERVICE)

$(TARGET_KEYLOGGER): $(OBJ_KEYLOGGER)
	$(LINK) /nologo $(OBJ_KEYLOGGER) /OUT:$(TARGET_KEYLOGGER)

clean:
	del $(OBJ_KEYLOGGER) $(OBJ_SERVICE)

fclean: clean
	del $(TARGET_KEYLOGGER) $(TARGET_SERVICE) winkey.log

re: fclean all