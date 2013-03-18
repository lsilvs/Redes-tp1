# Compile/Link
GCC = gcc
OPTMIZE = -g3 -O3
LIBS = -lc

#Objetos
OBJS1 =  servidor4.c socket4.c funcoes.c arquivo.c
OBJS2 =  servidor6.c socket6.c funcoes.c arquivo.c

OBJS3 =  cliente4.c
OBJS4 =  cliente6.c

# Nome do aplicativo
APPNAME1 = servidor4
APPNAME2 = servidor6
APPNAME3 = cliente4
APPNAME4 = cliente6

release: ; $(GCC) $(OPTMIZE) $(LIBS) $(OBJS1) -o $(APPNAME1)
		$(GCC) $(OPTMIZE) $(LIBS) $(OBJS2) -o $(APPNAME2)
		$(GCC) $(OPTMIZE) $(LIBS) $(OBJS3) -o $(APPNAME3)
		$(GCC) $(OPTMIZE) $(LIBS) $(OBJS4) -o $(APPNAME4)

clean:
		rm -f $(APPNAME1) $(APPNAME2) $(APPNAME3) $(APPNAME4) *.o
		clear

servidor:
		make
		./$(APPNAME1)

cliente:
		./$(APPNAME3) meunomedeusuario qwerty localhost

test:
		make
