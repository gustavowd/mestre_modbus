
#DEBUG	= -g -O0
DEBUG	= -O3
CC	= gcc
INCLUDE	= -I/usr/local/include
CFLAGS	= $(DEBUG) -Wall $(INCLUDE) -Winline -pipe

LDFLAGS	= -L/usr/local/lib
LIBS    = -lserial -lpthread

# Should not alter anything below this line
###############################################################################

SRC	=	mestre_modbus.c

OBJ	=	mestre_modbus.o

all:		mestre_modbus

mestre_modbus:	$(OBJ)
		@echo [link]
		$(CC) -o $@ mestre_modbus.o $(LDFLAGS) $(LIBS)
	
.c.o:
	@echo [CC] $<
	@$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ) mestre_modbus

