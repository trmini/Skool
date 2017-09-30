# Socket UDP communication makefile # 
CC=cc 
OPS=-g 
LIBS=-lnsl -lsocket 
LINT=lint 
LOPS=-m -x -u -Xarch=v9 
LLIBS= 

all:		clientudp1 serverudp1 clientudp2 serverudp2

clientudp1:	clientudp1.c 
		$(CC) $(OPS) -o clientudp1 clientudp1.c $(LIBS) 
		
serverudp1:	serverudp1.c 
		$(CC) $(OPS) -o serverudp1 serverudp1.c $(LIBS)
			
clientudp2:	clientudp2.c 
		$(CC) $(OPS) -o clientudp2 clientudp2.c $(LIBS) 
			
serverudp2:	serverudp2.c 
		$(CC) $(OPS) -o serverudp2 serverudp2.c $(LIBS)

lintc1:		
		$(LINT) $(LOPS) clientudp1.c 

lints1: 
		$(LINT) $(LOPS) serverudp1.c 

lintc2:			
		$(LINT) $(LOPS) clientudp2.c 

lints2: 
		$(LINT) $(LOPS) serverudp2.c 

lintall: 
		$(LINT) $(LOPS) clientudp1.c serverudp1.c clientudp2.c serverudp2.c
			
clean:
		rm -f core *.o clientudp1 serverudp1 clientudp2 serverudp2
