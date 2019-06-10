#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define NUM_TUPLE 50

//da usare singolarmente o unirne 2 con 'e' o unirne 3 con 'e' e 'con'
char* nomi[] = {"Nicolas", "Clint", "Tom", "Robert", "Richard", 
			"Woody", "George", "Brad", "Leonardo", 
			"Jennifer", "Penelope", "Willem", 
			"Claudio", "Christian", "Massimo", "Paolo", "Bradley", 
			"Johnny", "Monica", "Adriano", "Matt",
			"Elisabetta", "Serena", "Marini", "Ezio", "Enzino",
			"Lino", "Maria", "Maurizio", "Belen",
			"Valentina", "Will", "Barbara", "Jessica", "Russel", 0};

char* cognomi[] = {"Cage", "Eastwood", "Cruise", "De Niro", "Gere", 
			"Allen", "Clooney", "Pit", "Di Caprio", 
			"Anniston", "Cruz", "Dafoe", 
			"Bisio", "De Sica", "Boldi", "Villaggio", "Cooper", 
			"Deep", "Bellucci", "Celentano", "Deamom",
			"Canalis", "Marini", "Gomma", "Greggio", "Iacchetti",
			"Banfi", "De Filippi", "Costanzo", "Rodriguez",
			"Cortese", "Smith", "D'Urso", "Alba", "Crowe", 0};

char *insert = "INSERT INTO Attore VALUES \n";

void main(){

	//crea un file con 50 tuple del tipo
	//(nome, cognome)
	
	char *tupla = (char*)malloc(512);
	char *nome = (char*)malloc(30);
	char *cognome = (char*)malloc(20);
	int fileDescriptor = open("attoriDump.dump", O_RDWR|O_CREAT|O_TRUNC, 0666);

	if(fileDescriptor == -1){
		printf("Errore nell'aprire il file\n");
		exit(0);
	}

	if(write(fileDescriptor, insert, (size_t)strlen(insert)) == -1){
		printf("Errore in scrittura dell' insert\n");
		exit(5);
	}

	for(int i = 0; i < NUM_TUPLE; ++i){

		strcat(nome, nomi[rand()%35]);
		strcat(cognome, cognomi[rand()%35]);
		
		//possibili doppi nomi
		switch(rand()%3){
			case 0:
				strcat(nome, " ");
				strcat(nome, nomi[rand()%35]);
				break;
		}

		strcat(tupla, "(");
		strcat(tupla, "'");
		strcat(tupla, nome);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");
		strcat(tupla, cognome);
		strcat(tupla, "'");
		strcat(tupla, "),\n");

		puts(tupla);

		if(write(fileDescriptor, tupla, (size_t)strlen(tupla)) == -1){
			printf("Errore in scrittura della tupla %d\n", i);
			exit(2);
		}
		//evito sovrapposizioni
		memset(tupla, (char *)0, 512);
		memset(nome, (char *)0, 30);
		memset(cognome, (char *)0, 20);
		
	}

	//cancello l'ultima virgola
	if(lseek(fileDescriptor, -2, SEEK_CUR) == -1){
		printf("Errore in posizionare il file pointer\n");
		exit(4);
	}
	
	if(write(fileDescriptor, ";", (size_t)1) == -1){
		printf("Errore in scrittura della fine\n");
		exit(3);
	}

	if(close(fileDescriptor) == -1){
		printf("Errore nel chiudere il file\n");
		exit(1);
	}
	

}


