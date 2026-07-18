include config.mk
TARGET = img
SRC = \
	main.c \
	files/files.c \
	x11/x11.c \
	backend/backend.c
CPPFLAGS += -I.
LDLIBS = -lX11
ifeq ($(WITH_PNG),1)
SRC += backend/png/backend.c
CPPFLAGS += -DWITH_PNG
LDLIBS += -lpng
endif
ifeq ($(WITH_JPEG),1)
SRC += backend/jpeg/backend.c
CPPFLAGS += -DWITH_JPEG
LDLIBS += -ljpeg
endif
ifeq ($(WITH_WEBP),1)
SRC += backend/webp/backend.c
CPPFLAGS += -DWITH_WEBP
LDLIBS += -lwebp
endif
OBJ = $(SRC:.c=.o)
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
static: LDFLAGS += -static
static: LDLIBS += -lxcb -lXau -lXdmcp -lz -lm
static: $(TARGET)
clean:
	rm -f $(OBJ) $(TARGET)
.PHONY: all static clean
