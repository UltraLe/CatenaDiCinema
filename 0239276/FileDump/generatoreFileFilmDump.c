#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define NUM_TUPLE 100

//da usare singolarmente o unirne 2 con 'e' o unirne 3 con 'e' e 'con'
char* nomi[] = {"Pesce", "Squalo", "Tornado", "Apocalisse", "Giorno", 
			"Storia", "Cammino", "Computer", "Bottiglia", 
			"Assassino", "Sterminatore", "Baby Sitter", 
			"Guerra", "Stermino", "Luna", "Pianeta", "Marte", 
			"Plutone", "Giardino", "Tempesta", "Scrivania",
			"Penna", "Astuccio", "Gomma", "Uomo", "Donna",
			"Patata", "Cavolfiore", "Carota", "Zucchina",
			"Gatto", "Cane", "Animale", "Pianoforte", "Sassofono", 0};

char* verbiPlurale[] = {"Fanno", "Costruiscono", "Mangiano", "Distruggono",
				"Inventano", "Trovano", "Seppelliscono",
				"Tagliano", "Studiano", "Lanciano", "Leggono",
				"Disegnano", "Dipingono"};

char* verbiSingolare[] = {"Fa", "Costruisce", "Mangia", "Distrugge", 
				"Inventa", "Trova", "Seppellisce",
				"Taglia", "Studia", "Lancia", "Legge",
				"Disegna", "Dipinge"};

char* finali[] = {"2", "3", "4", "Il Ritorno", "La Vendetta"};

char* caseCinematografiche[] = {"Disney", "Pixar", "Walt", "Universal",
					"Dream", "Work", "20th", "Century", "Fox",
					"Paramount", "Metro", "Goldwyn", "Mayer"};

char *insert = "INSERT INTO Film VALUES \n";

void main(){

	//crea un file con 100 tuple del tipo
	//(nomeFilm, durata, casaCinematografica)
	
	char *nomeFilm = (char*)malloc(128);
	char *casa = (char*)malloc(128);
	char durata[3];
	char *tupla = (char*)malloc(1024);
	srand(time(NULL));
	int r, rr, ra, rb;

	puts(nomi[4]);

	int fileDescriptor = open("filmDump.dump", O_RDWR|O_CREAT|O_TRUNC, 0666); 
	if(fileDescriptor == -1){
		printf("Errore nell'aprire il file\n");
		exit(0);
	}

	if(write(fileDescriptor, insert, (size_t)strlen(insert)) == -1){
		printf("Errore in scrittura dell' insert\n");
		exit(5);
	}

	for(int i = 0; i < NUM_TUPLE; ++i){

		switch(rand()%3){;
			case 0:
				strcat(nomeFilm, nomi[rand()%35]);
				strcat(nomeFilm, " ");
				strcat(nomeFilm, verbiSingolare[rand()%13]);
				strcat(nomeFilm, " ");
				strcat(nomeFilm, nomi[rand()%35]);
				break;
			case 1:
				strcat(nomeFilm, nomi[rand()%35]);
				strcat(nomeFilm, " E ");
				strcat(nomeFilm, nomi[rand()%35]);
				strcat(nomeFilm, " ");
				strcat(nomeFilm, verbiPlurale[rand()%13]);
				strcat(nomeFilm, " ");
				strcat(nomeFilm, nomi[rand()%35]);
				break;
			case 2:
				strcat(nomeFilm, nomi[rand()%35]);
				strcat(nomeFilm, " E ");
				strcat(nomeFilm, nomi[rand()%35]);
				strcat(nomeFilm, " Con ");
				strcat(nomeFilm, nomi[rand()%35]);
				strcat(nomeFilm, " ");
				strcat(nomeFilm, verbiPlurale[rand()%13]);
				strcat(nomeFilm, " ");
				strcat(nomeFilm, nomi[rand()%35]);
				break;
		}
			
		switch(rand()%5){
			case 0:
				strcat(nomeFilm, " ");
				strcat(nomeFilm, finali[rand()%5]);
				break;
		}

		strcat(casa, caseCinematografiche[rand()%13]);

		strcat(tupla, "(");
		strcat(tupla, "'");
		strcat(tupla, nomeFilm);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");
		//assumo che un film possa avere  durata massima di 2 ore e 10
		sprintf(durata, "%d", ((rand()%(130-70))+70));
		strcat(tupla, durata);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");
		strcat(tupla, casa);
		strcat(tupla, "'");
		strcat(tupla, "),\n");

		puts(tupla);

		if(write(fileDescriptor, tupla, (size_t)strlen(tupla)) == -1){
			printf("Errore in scrittura della tupla %d\n", i);
			exit(2);
		}
		//evito sovrapposizioni
		memset(tupla, (char *)0, 1024);
		memset(casa, (char *)0, 128);
		memset(nomeFilm, (char *)0, 128);
		
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


