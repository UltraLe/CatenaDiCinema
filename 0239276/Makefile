all:
	gcc -g *.c -o applicazione `mysql_config --cflags --include --libs` -lncurses 

clean:
	-rm applicazione
