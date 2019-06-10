#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define NUM_TUPLE 200

char* nomi[] = {"Bezalel", "Parto", "Chirstian", "Saniya", "Berni", 
			"Bridgland", "Ramzi", "Arif", "Sailaja", 
			"Lillian", "Roddy", "Hisao", 
			"Claudio", "Xinyu", "Eben", "Eben", "Hugo", 
			"Johnny", "Kish", "Amstein", "Marc",
			"Syozo", "Maren", "Marwedel", "Mohua", "Enzino",
			"Moty", "Hercules", "Kauko", "Aksel",
			"Falguni", "Wilmer", "Goa", "Shigei", "Masali", 0};

char* cognomi[] = {"Vitaly", "Famili", "Comellas", "Spelt", "Shuky", 
			"Dietrich", "Lucien", "Seshu", "Rettelbach", 
			"Restivo", "Ozeri", "Porotnikoff", 
			"Mondadori", "Coors", "Subbu", "Setia", "Reeker", 
			"Morris", "Bianchi", "Rossi", "Neri",
			"Canalis", "Marini", "Bobbo", "Fini", "Iammetti",
			"Perna", "Giap", "Miguel", "Denunzius",
			"Cortesis", "Smothos", "Dantres", "Alatro", "Corveto", 0};

char *insert = "INSERT INTO Dipendente VALUES \n";

void main(){

	//crea un file con 50 tuple del tipo
	//(codiceFiscale, nome, cognome, tipo)
	
	char *tupla = (char*)malloc(1024);
	char *nome = (char*)malloc(30);
	char *num = (char*)malloc(4);
	char *CF = (char*)malloc(30);
	char *tipo = (char*)malloc(15);
	char *cinema = (char*)malloc(1);
	char *cognome = (char*)malloc(20);
	int fileDescriptor = open("dipendentiCatenaDump.dump", O_RDWR|O_CREAT|O_TRUNC, 0666);

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
				strcat(nome, nomi[rand()%35]);
				break;
		}

		strcat(CF, nome);
		strcat(CF, cognome);
		strcat(CF, num);
		sprintf(num, "%d", (rand()%10000));
		strcat(CF, num);

		strcat(tupla, "(");
		strcat(tupla, "'");
		strcat(tupla, CF);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");
		strcat(tupla, nome);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");
		strcat(tupla, cognome);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");

		//il 70% dei tipendenti sono di tipo 'Proiezionista'
		//il restante di tipo 'Maschera'
		switch(rand()%3){
			case 0:
				strcat(tipo, "Maschera");
				break;
			default:
				strcat(tipo, "Proiezionista");
				break;
		}

		strcat(tupla, tipo);
		strcat(tupla, "'");
		strcat(tupla, ",");
		strcat(tupla, "'");
		
		//scelgo un cinema casuale per ogni dipendente
		//(rand()%10)+1
		sprintf(cinema, "%d", (rand()%10)+1);

		strcat(tupla, cinema);
		strcat(tupla, "'");
		strcat(tupla, "),\n");

		puts(tupla);

		if(write(fileDescriptor, tupla, (size_t)strlen(tupla)) == -1){
			printf("Errore in scrittura della tupla %d\n", i);
			exit(2);
		}
		//evito sovrapposizioni
		memset(tupla, (char *)0, 1024);
		memset(nome, (char *)0, 30);
		memset(cognome, (char *)0, 20);
		memset(CF, (char *)0, 20);
		memset(tipo, (char *)0, 15);
		memset(cinema, (char *)0, 1);
		
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


