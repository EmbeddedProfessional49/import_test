CROSS_COMPILE=arm-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
CFLAGS=-c -g -rdynamic -funwind-tables -Wall -I../inlcude/
LDFLAGS=  -lpthread -lrt

STRIP=$(CROSS_COMPILE)strip

SOURCES= main.c rfid_card.c Card1.c

OBJECTS=$(SOURCES:.c=.o)

TARGET=access_system

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)

	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
# 	$(STRIP) $(TARGET)
.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) *.o $(TARGET)
