#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#include "program.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <limits.h>



#define MAX_TIME_ACQUISTA 10  	//tempo in minuti
#define fflush(stdin) while(!getchar());

struct configuration conf;

static void test_error(MYSQL * con, int status)
{
	if (status) {
		fprintf(stderr, "Error: %s (errno: %d)\n", mysql_error(con),
			mysql_errno(con));
		execlp("./applicazione", "./applicazione", NULL);
		printf("Errore in execlp\n");
	}
}


static void test_stmt_error(MYSQL_STMT * stmt, int status)
{
	if (status) {
		fprintf(stderr, "Error: %s (errno: %d)\n",
			mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
		execlp("./applicazione", "./applicazione", NULL);
		printf("Errore in execlp\n");
	}
}


char *scegli = "Scegli tra le seguenti opzioni\n";
char *sceltaScorretta = "Scelta non disponibile, ritenta\n";
char *inserisciScelta = "Inserisci scelta: ";
const MYSQL const *con;


void cancella_db()
{

	if (mysql_real_connect(con, conf.host, conf.username, conf.password, conf.database, conf.port, NULL, 0) == NULL) {
		fprintf(stderr, "Connection error: %s\n", mysql_error(con));
		exit(1);
	}

	int status;
	MYSQL_STMT *stmt;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	status = mysql_stmt_prepare(stmt, "drop schema CatenaCinema", 25);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);
	mysql_close(con);

}


void visualizzaProiezioni()
{
	int cinema;
	char nomeFilm[128];
	char data[11];
	char ora[9];
	//char sala[3];
	int sala;
	bool avanti = false;
	while(!avanti){
		puts("Per quale cinema vuoi visualizzare le proiezioni (ID Cinema da 1 a 10) ?\n");
		scanf("%d", &cinema);
		fflush(stdin);
		if(cinema < 1 || cinema > 10){
			puts(sceltaScorretta);
		}else{
			avanti = true;
		}
	}

	puts("Per quale film vuoi visualizzare le proiezioni disponibili ?\n");
	scanf ("%[^\n]", nomeFilm);
	fflush(stdin);

	printf("Visualizzo info proiezioni su '%s', del cinema '%d'\n", nomeFilm, cinema);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[0] = strlen(nomeFilm);
	length[1] = sizeof(int);
	int num_fields, status, i;
	int j = 1;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[3];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
		
	status = mysql_stmt_prepare(stmt, "call proiezioniDisponibili(?,?)", 32);
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &cinema;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nomeFilm;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);			
	
		if (num_fields > 0) {

			if (num_fields == 0){
				printf("Il film digitato non ha proiezioni nel DB\n");
				break;
			}

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			// Retrieve the fields associated with OUT/INOUT parameters
			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = data;
			rs_bind[0].buffer_length = sizeof(data);

			rs_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = ora;
			rs_bind[1].buffer_length = sizeof(ora);
	
			rs_bind[2].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[2].is_null = &is_null[2];
			rs_bind[2].buffer = &sala;
			rs_bind[2].buffer_length = sizeof(int);		

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);
			
			while(1){		
				status = mysql_stmt_fetch(stmt);				

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else if(!*rs_bind[0].is_null){
					printf("\tProiezione disponibile numero %d\n", j);					
					printf("Data: %s\n", rs_bind[0].buffer);
					printf("Ora: %s\n", rs_bind[1].buffer);
					printf("Sala: %d\n", sala);
					printf("\t--------------------\n");
				}
			j++;
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("End of procedure output\n");
		}

		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void visualizzaDettagliFilm()
{
	char nomeFilm[128];
	char durata[4];
	char casaCin[20];

	puts("Inserisci il nome del film di cui vuoi sapere i dettagli:\n");
	scanf ("%[^\n]", nomeFilm);
	fflush(stdin);
	printf("Film: %s\n", nomeFilm);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = strlen(nomeFilm);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[2];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
		
	status = mysql_stmt_prepare(stmt, "select Durata, CasaCinematografica from Film where Nome = ?", 60);
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nomeFilm;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = durata;
			rs_bind[0].buffer_length = sizeof(durata);

			rs_bind[1].buffer_type = fields[1].type;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = casaCin;
			rs_bind[1].buffer_length = sizeof(casaCin);		


			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Durata del film: %s\n", durata);
					printf("Casa Cinematografica del film: %s\n",rs_bind[1].buffer);
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
	
}


void visualizzaFilm()
{
	char nomeFilm[128];
	MYSQL_STMT *stmt;
	int num_fields, status;
	int cinema;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[2];

	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select distinct F.Nome, P.Cinema from Film F join Proiezione P on F.Nome = P.Film";

	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	do {
		num_fields = mysql_stmt_field_count(stmt);

		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);
			rs_bind[0].buffer_type = fields[0].type;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = nomeFilm;
			rs_bind[0].buffer_length = sizeof(nomeFilm);

			rs_bind[1].buffer_type = fields[1].type;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = &cinema;
			rs_bind[1].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while (1) {
				status = mysql_stmt_fetch(stmt);

				if (status == 1 || status == MYSQL_NO_DATA)
					break;
				
				if (*rs_bind[0].is_null){
					printf(" val[%d] = NULL;", 0);
				}else{
					printf("Film: %s, Cinema: %d\n", rs_bind[0].buffer, cinema);
				}
			}

	
			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			// no columns = final status packet
			printf("--------------------------------------\n");
		}

		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
	
}


void castAttori()
{
	char nomeFilm[128];
	char nome[20];
	char cognome[20];

	puts("Inserisci il nome del film di cui vuoi sapere il cast di attori protagonisti:\n");
	scanf ("%[^\n]", nomeFilm);
	fflush(stdin);
	printf("Film: %s\n", nomeFilm);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = strlen(nomeFilm);
	int num_fields, status, i;
	int j = 1;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[2];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}
		
	status = mysql_stmt_prepare(stmt, "select NomeAttore, CognomeAttore from AttoriNelFilm where NomeFilm = ?", 71);
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nomeFilm;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = nome;
			rs_bind[0].buffer_length = sizeof(nome);

			rs_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = cognome;
			rs_bind[1].buffer_length = sizeof(cognome);		


			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("\tAttore protagonista %d\n", j);
					printf("Nome: %s\n", rs_bind[0].buffer);
					printf("Cognome: %s\n",rs_bind[1].buffer);
				}
				j++;
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);

}


char *menu1cli = "Procedura di acquisto del biglietto, dopo aver scelto la proiezione da prenotare (nel menu precedente)\nInserisci i seguenti dati:\n";


void acquistaBiglietto()
{
	/*
		Seconde le specifiche da questo momento in poi, il cliente ha 10 minuti per completare
		la procedura di acquisto del biglietto.
	*/ 

	puts(menu1cli);
	char film[128];
	memset(film, 0, 128);

	char codice[50];
	memset(codice, 0, 50);

	char numeroCarta[17];
	memset(numeroCarta, 0, 17);
	char nomeIntest[45];
	memset(nomeIntest, 0, 45);
	char cognomeIntest[45];
	memset(cognomeIntest, 0, 45);
	int meseScad;
	memset(&meseScad, 0, 4);
	int annoScad;
	memset(&annoScad, 0, 4);
	int cvv;
	memset(&cvv, 0, 4);

	int numeroPosto;
	memset(&numeroPosto, 0, 4);
	char filaPosto[1];
	memset(filaPosto, 0, 1);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	double sec; 
    	sec = time(NULL);

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	printf("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;
	printf("Fila Del Posto Scelto: ");
	scanf("%s", filaPosto);
	fflush(stdin);
	printf("Numero Del Posto Scelto: ");
	scanf("%d", &numeroPosto);
	fflush(stdin);

	printf("Ora inserisca i dati relativi alla sua carta di credito\n(Tempo rimanente: %.1f minuti)\n", 10-((time(NULL)-sec)/60));
	
	printf("Numero carta: ");
	scanf("%s", numeroCarta);
	fflush(stdin);
	printf("Nome intestatario: ");
	scanf("%[^\n]", nomeIntest);
	fflush(stdin);
	printf("Cognome intestatario: ");
	scanf("%[^\n]", cognomeIntest);
	fflush(stdin);
	printf("Mese scadenza: ");
	scanf("%d", &meseScad);
	fflush(stdin);
	printf("Anno scadenza: ");
	scanf("%d", &annoScad);
	fflush(stdin);
	printf("Codice CVV: ");
	scanf("%d", &cvv);
	fflush(stdin);

	sec = time(NULL) - sec;
	double tempo_impiegato = sec/60;

	//controllo che il tempo impiegato sia <= a MAX_TIME_ACQUISTA

	if(tempo_impiegato >= MAX_TIME_ACQUISTA*60){
		system("clear");
		printf("Ci hai messo tropo tempo, riprova per favore.\n");
		return;
	}

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[14];
	unsigned long length[14];
	length[7] = strlen(numeroCarta);
	length[8] = strlen(nomeIntest);
	length[9] = strlen(cognomeIntest);
	length[10] = sizeof(int);
	length[11] = sizeof(int);
	length[12] = sizeof(int);
	length[2] = sizeof(data);
	length[3] = sizeof(ora);
	length[4] = strlen(film);
	length[5] = sizeof(int);
	length[6] = strlen(filaPosto);
	length[0] = sizeof(int);
	length[1] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call acquistaBiglietto(?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &sala;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[4].buffer = film;
	ps_params[4].length = &length[4];
	ps_params[4].is_null = 0;
	
	ps_params[5].buffer_type = MYSQL_TYPE_LONG;
	ps_params[5].buffer = &numeroPosto;
	ps_params[5].length = &length[5];
	ps_params[5].buffer_length = sizeof(int);
	ps_params[5].is_null = 0;
	
	ps_params[6].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[6].buffer = filaPosto;
	ps_params[6].length = &length[6];
	ps_params[6].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[2].buffer = (char *)&data;
	ps_params[2].length = 0;
	ps_params[2].is_null = 0;
	
	ps_params[3].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[3].buffer = (char *)&ora;
	ps_params[3].length = 0;
	ps_params[3].is_null = 0;

	ps_params[7].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[7].buffer = numeroCarta;
	ps_params[7].length = &length[7];
	ps_params[7].is_null = 0;

	ps_params[8].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[8].buffer = nomeIntest;
	ps_params[8].length = &length[8];
	ps_params[8].is_null = 0;

	ps_params[9].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[9].buffer = cognomeIntest;
	ps_params[9].length = &length[9];
	ps_params[9].is_null = 0;

	ps_params[10].buffer_type = MYSQL_TYPE_LONG;
	ps_params[10].buffer = &meseScad;
	ps_params[10].length = &length[10];
	ps_params[10].buffer_length = sizeof(int);
	ps_params[10].is_null = 0;

	ps_params[11].buffer_type = MYSQL_TYPE_LONG;
	ps_params[11].buffer = &annoScad;
	ps_params[11].length = &length[11];
	ps_params[11].buffer_length = sizeof(int);
	ps_params[11].is_null = 0;

	ps_params[12].buffer_type = MYSQL_TYPE_LONG;
	ps_params[12].buffer = &cvv;
	ps_params[12].length = &length[12];
	ps_params[12].buffer_length = sizeof(int);
	ps_params[12].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = fields[0].type;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = codice;
			rs_bind[0].buffer_length = sizeof(codice);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{	
					printf("   --------------CODICE DI PRENOTAZIONE--------------   \n");
					printf("!! ---> %s <--- !!\n", codice);
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);	
	if(strlen(codice) > 0){
		printf("Conserva bene questo codice, se vuoi annullare tale biglietto in futuro,\n");
		printf("Oppure confermarlo all'ingresso del cinema, presentandolo alla maschera.\n");
		printf("Grazie !\n");
	}else{
		printf("Errore nella procedura di acquisto del biglietto, per favore ricontrolla i dati inseriti.\n");
	}

}


char *menu2cli = "Per vedere i posti disponibili per una certa proiezione, inserisci i seguenti dati:\n";


void postiDisponibili()
{

	puts(menu2cli);
	char film[128];
	memset(film, 0, 128);

	char fila[5];
	memset(film, 0, 5);
	char numero[5];
	memset(film, 0, 5);
	char disp[5];
	memset(film, 0, 5);

	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	printf("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[3];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[5];
	unsigned long length[5];
	length[0] = sizeof(data);
	length[1] = sizeof(ora);
	length[2] = strlen(film);
	length[3] = sizeof(int);
	length[4] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call postiDisponibiliDellaProiezione(?,?,?,?,?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &sala;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &cinema;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[0].buffer = (char *)&data;
	ps_params[0].length = 0;
	ps_params[0].is_null = 0;
	
	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = fila;
			rs_bind[0].buffer_length = sizeof(fila);

			rs_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = numero;
			rs_bind[1].buffer_length = sizeof(numero);
			
			rs_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[2].is_null = &is_null[0];
			rs_bind[2].buffer = disp;
			rs_bind[2].buffer_length = sizeof(disp);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					if(strcmp(disp, "0") == 0)
						printf("Fila Posto: %s, Numero Posto: %s\n", fila, numero); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


char *menu0cli = "1)Visualizza film\n2)Visualizza dettagli flim\n3)Visualizza il cast di attori protagonisti di un film\n4)Visualizza proiezioni disponibili di un cinema\n5)Visualizza posti disponibili di una proiezione\n6)Acquista biglietto\n7)Annulla acquisto biglietto\n8)Indietro";


void annullaAcquisto(){

	char codice[45];
	memset(codice, 0, 45);

	puts("Inserisci il codice di prenotazione del biglietto acquistato:\n");
	scanf("%s", codice);
	
	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = strlen(codice);
	int status;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call annullaBiglietto(?)";
		
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = codice;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Biglietto annullato correttamente\n");
}


void cliente()
{

	if (mysql_real_connect(con, conf.host, conf.username, conf.password, conf.database, conf.port, NULL, 0) == NULL) {
		fprintf(stderr, "Connection error: %s\n", mysql_error(con));
		exit(1);
	}

	int scelta;

	while(1){
		puts(scegli);
		puts(menu0cli);
		puts(inserisciScelta);
		scanf("%d", &scelta);
		fflush(stdin);
		system("clear");
		switch (scelta) {
			
			case 1:
				visualizzaFilm();
				break;
			case 2:
				visualizzaDettagliFilm();
				break;
			case 3:
				castAttori();
				break;
			case 4:
				visualizzaProiezioni();
				break;
			case 5:
				postiDisponibili();
				break;
			case 6:
				acquistaBiglietto();
				break;
			case 7:
				annullaAcquisto();
				break;
			case 8:
				mysql_close(con);
				return;
			default:
				puts(sceltaScorretta);
				break;
		}
	}
}


char *menu0vis = "1)Visualizza film\n2)Visualizza dettagli flim\n3)Visualizza il cast di attori protagonisti di un film\n4)Visualizza proiezioni disponibili di un cinema\n5)Indietro";



void visitatore()
{
	if (mysql_real_connect(con, conf.host, conf.username, conf.password, conf.database, conf.port, NULL, 0) == NULL) {
		fprintf(stderr, "Connection error: %s\n", mysql_error(con));
		exit(1);
	}

	int scelta;

	while(1){
		puts(scegli);
		puts(menu0vis);
		puts(inserisciScelta);
		scanf("%d", &scelta);
		fflush(stdin);
		system("clear");
		switch (scelta) {
			
			case 1:
				visualizzaFilm();
				break;
			case 2:
				visualizzaDettagliFilm();
				break;
			case 3:
				castAttori();
				break;
			case 4:
				visualizzaProiezioni();
				break;
			case 5:
				mysql_close(con);
				return;
			default:
				puts(sceltaScorretta);
				break;
		}
	}
}


void confermaBiglietto()
{
	char codice[45];
	memset(codice, 0, 45);

	puts("Inserisci il codice di prenotazione del biglietto acquistato:\n");
	scanf ("%s", codice);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = strlen(codice);
	int status;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call confermaBiglietto(?)";
		
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = codice;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Biglietto confermato correttamente\n");
}


char *menu0mas = "1)Conferma biglietto\n2)Indietro\n";


void maschera()
{
	if (mysql_real_connect(con, conf.host, conf.username, conf.password, conf.database, conf.port, NULL, 0) == NULL) {
		fprintf(stderr, "Connection error: %s\n", mysql_error(con));
		exit(1);
	}

	int scelta;

	while(1){
		puts(scegli);
		puts(menu0mas);
		puts(inserisciScelta);
		scanf("%d", &scelta);
		fflush(stdin);
		system("clear");
		switch (scelta) {
			
			case 1:
				confermaBiglietto();
				break;
			case 2:
				mysql_close(con);
				return;
			default:
				puts(sceltaScorretta);
				break;
		}
	}
}



void mostraProiezionistaDiProiez()
{

	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	char film[128];
	memset(film, 0, 128);
	char proiezionista[45];
	memset(proiezionista, 0, 45);
	MYSQL_TIME data;
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));
	memset(&data, 0, sizeof(data));
	ora.time_type = MYSQL_TYPE_DATETIME;
	data.time_type = MYSQL_TYPE_DATETIME;

	printf("Inserisci i seguenti dati per individuare il proiezionista della proiezione:\n");
	printf("Cinema: ");	
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");	
	scanf("%d", &sala);
	fflush(stdin);
	printf("Film: ");	
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");	
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");	
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");	
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");	
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");	
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;
	
	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[5];
	unsigned long length[5];
	length[0] = strlen(film);
	length[1] = sizeof(data);
	length[2] = sizeof(ora);
	length[3] = sizeof(int);
	length[4] = sizeof(int);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select Dipendente from Proiezione where Film = ? and Data = ? and Ora = ? and Cinema = ? and Sala = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &cinema;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &sala;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = film;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&data;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[2].buffer = (char *)&ora;
	ps_params[2].length = 0;
	ps_params[2].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = proiezionista;
			rs_bind[0].buffer_length = sizeof(proiezionista);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Proiezionista che gestisce la proiezione inserita: %s\n", rs_bind[0].buffer);
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void mostraMaschere()
{
	char *pom = "16-20";
	char *ser = "20-24";
	char maschera[45];
	memset(maschera, 0, 45);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	char fasciaOraria[5];
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);

	puts("Fascia oraria:\n1)16-20\n2)20-24\n");
	scanf("%d", &scelta);
	fflush(stdin);

	switch (scelta){
		case 1:
			strcpy(fasciaOraria, pom);
			break;
		case 2:
			strcpy(fasciaOraria, ser);
			break;
		default:
			printf("Scelta scorretta\n");
			return;
			
	}

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[3];
	unsigned long length[3];
	length[0] = sizeof(int);
	length[1] = strlen(fasciaOraria);
	length[2] = sizeof(data);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select D.CodiceFiscale from TurnoDelDipendente TD join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale where D.Cinema = ? and TD.FasciaOrariaTurno = ? and TD.GiornoTurno = ? and Tipo = 'Maschera'";
		
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = fasciaOraria;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[2].buffer = (char *)&data;
	ps_params[2].length = 0;
	ps_params[2].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = maschera;
			rs_bind[0].buffer_length = sizeof(maschera);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Maschera in turno: %s\n", rs_bind[0].buffer);
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}

void aggiornaProiezionista()
{
	char film[128];
	memset(film, 0, 128);
	char proiezionista[45];
	memset(proiezionista, 0, 45);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Proiezionista: ");
	scanf("%s", proiezionista);
	fflush(stdin);
	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	puts("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[6];
	unsigned long length[6];
	length[0] = strlen(proiezionista);
	length[1] = sizeof(data);
	length[2] = sizeof(ora);
	length[3] = strlen(film);
	length[4] = sizeof(int);
	length[5] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call aggiornaProiezionistaDellaProiezione(?,?,?,?,?,?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &sala;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[5].buffer_type = MYSQL_TYPE_LONG;
	ps_params[5].buffer = &cinema;
	ps_params[5].length = &length[5];
	ps_params[5].buffer_length = sizeof(int);
	ps_params[5].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = proiezionista;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[3].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[3].buffer = film;
	ps_params[3].length = &length[3];
	ps_params[3].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&data;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;
	
	ps_params[2].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[2].buffer = (char *)&ora;
	ps_params[2].length = 0;
	ps_params[2].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Operazione conclusa. Per verificare esito consultare turni del proiezionista aggiunto\n");

}

void aggiungiMaschera()
{
	char *pom = "16-20";
	char *ser = "20-24";
	char maschera[45];
	memset(maschera, 0, 45);
	int scelta;
	char fasciaOraria[5];
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));

	printf("Maschera: ");
	scanf("%[^\n]", maschera);
	fflush(stdin);

	puts("Fascia oraria:\n1)16-20\n2)20-24\n");
	scanf("%d", &scelta);
	fflush(stdin);

	switch (scelta){
		case 1:
			strcpy(fasciaOraria, pom);
			break;
		case 2:
			strcpy(fasciaOraria, ser);
			break;
		default:
			printf("Scelta scorretta\n");
			return;
			
	}

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[3];
	unsigned long length[3];
	length[2] = strlen(maschera);
	length[0] = strlen(fasciaOraria);
	length[1] = sizeof(data);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "insert into TurnoDelDipendente values (?,?,?)";
		
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = maschera;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = fasciaOraria;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&data;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Dipendente inserito\n");
}


void rimuoviProiezionistaDaProiezione()
{
	char film[128];
	memset(film, 0, 128);
	char proiezionista[45];
	memset(proiezionista, 0, 45);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	puts("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[5];
	unsigned long length[5];
	length[0] = sizeof(data);
	length[1] = sizeof(ora);
	length[2] = strlen(film);
	length[3] = sizeof(int);
	length[4] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "update Proiezione set Dipendente = null where Data = ? and Ora = ? and Film = ? and Sala = ? and Cinema = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &sala;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &cinema;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[0].buffer = (char *)&data;
	ps_params[0].length = 0;
	ps_params[0].is_null = 0;
	
	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Operazione conclusa. Per verificare esito consultare turni del proiezionista rimosso\n");

}


void rimuoviMaschera()
{
	char *pom = "16-20";
	char *ser = "20-24";
	char maschera[45];
	memset(maschera, 0, 45);
	int scelta;
	char fasciaOraria[5];
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));

	printf("Maschera: ");
	scanf("%[^\n]", maschera);
	fflush(stdin);

	puts("Fascia oraria:\n1)16-20\n2)20-24\n");
	scanf("%d", &scelta);
	fflush(stdin);

	switch (scelta){
		case 1:
			strcpy(fasciaOraria, pom);
			break;
		case 2:
			strcpy(fasciaOraria, ser);
			break;
		default:
			printf("Scelta scorretta\n");
			return;
			
	}

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[3];
	unsigned long length[3];
	length[2] = strlen(maschera);
	length[0] = strlen(fasciaOraria);
	length[1] = sizeof(data);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "delete from TurnoDelDipendente where FasciaOrariaTurno = ? and GiornoTurno = ? and CodiceFiscaleDipendente = ?";
		
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = maschera;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = fasciaOraria;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&data;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Operazione completata, verificare con turno del dipendente\n");
}


void proiezionistiDisponibili()
{
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int durata;
	memset(&durata, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));
	char proiezionista[45];
	memset(proiezionista, 0, 45);

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;
	printf("Minuti per cui deve essere disponibile: ");
	scanf("%d", &durata);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[4];
	unsigned long length[4];
	length[2] = sizeof(data);
	length[1] = sizeof(ora);
	length[0] = sizeof(int);
	length[3] = sizeof(int);
	int num_fields, status, i;
	int j = 1;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call proiezionistiDisponibili(?,?,?,?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &durata;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[2].buffer = (char *)&data;
	ps_params[2].length = 0;
	ps_params[2].is_null = 0;
	
	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);
	
	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);
			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = proiezionista;
			rs_bind[0].buffer_length = sizeof(proiezionista);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Proiezionista libero n.%d: %s\n", j, rs_bind[0].buffer);
				}
				j++;
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);

}


void saleDisponibili()
{
	int cinema;
	memset(&cinema, 0, 4);
	int durata;
	memset(&durata, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));
	int sala;
	memset(&sala, 0, 4);

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;
	printf("Minuti per cui deve essere disponibile: ");
	scanf("%d", &durata);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[4];
	unsigned long length[4];
	length[0] = sizeof(int);
	length[1] = sizeof(ora);
	length[2] = sizeof(data);
	length[3] = sizeof(int);
	int num_fields, status, i;
	int j = 1;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call saleDisponibili(?,?,?,?)";

	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[2].buffer = (char *)&data;
	ps_params[2].length = 0;
	ps_params[2].is_null = 0;
	
	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &durata;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);
	
	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);
			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &sala;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Sala libera n.%d: %d\n", j, sala);
				}
				j++;
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);

}

void aggiungiProiezione()
{
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));
	char film[128];
	memset(film, 0, 128);
	char proiezionista[45];
	memset(proiezionista, 0, 45);
	int costo;
	memset(&costo, 0, 4);

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	printf("Costo: ");
	scanf("%d", &costo);
	fflush(stdin);
	printf("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Proiezionista: ");
	scanf("%s", proiezionista);
	fflush(stdin);
	

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[7];
	unsigned long length[7];
	length[3] = sizeof(int);
	length[1] = sizeof(ora);
	length[2] = strlen(film);
	length[0] = sizeof(data);
	length[4] = sizeof(int);
	length[5] = strlen(proiezionista);
	length[6] = sizeof(int);
	int status;

	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call aggiungiProiezione(?,?,?,?,?,?,?)";

	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[0].buffer = (char *)&data;
	ps_params[0].length = 0;
	ps_params[0].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;
	
	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &sala;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &cinema;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[5].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[5].buffer = proiezionista;
	ps_params[5].length = &length[5];
	ps_params[5].is_null = 0;

	ps_params[6].buffer_type = MYSQL_TYPE_LONG;
	ps_params[6].buffer = &costo;
	ps_params[6].length = &length[6];
	ps_params[6].buffer_length = sizeof(int);
	ps_params[6].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);
	
	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);
	
	printf("Operazione completata. Verifica corretto inserimento dalle proiezioni disponibili\n");

}


void aggiungiProiezioneNoProiez()
{
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));
	char film[128];
	memset(film, 0, 128);
	int costo;
	memset(&costo, 0, 4);

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	printf("Costo: ");
	scanf("%d", &costo);
	fflush(stdin);
	printf("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[6];
	unsigned long length[6];
	length[3] = sizeof(int);
	length[1] = sizeof(ora);
	length[2] = strlen(film);
	length[0] = sizeof(data);
	length[4] = sizeof(int);
	length[5] = sizeof(int);
	int status;

	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call aggiungiProiezioneSenzaProiezionista(?,?,?,?,?,?)";

	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[0].buffer = (char *)&data;
	ps_params[0].length = 0;
	ps_params[0].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;
	
	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &sala;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &cinema;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[5].buffer_type = MYSQL_TYPE_LONG;
	ps_params[5].buffer = &costo;
	ps_params[5].length = &length[5];
	ps_params[5].buffer_length = sizeof(int);
	ps_params[5].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);
	
	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);
	
	printf("Operazione completata. Verifica corretto inserimento dalle proiezioni disponibili\n");
}


void calendarioTurni()
{
	printf("Procedura che permette di stampare i turni dalla data corrente fino alla domenica piu vicina\n");

	int cinema;
	memset(&cinema, 0, 4);
	char fasciaOraria[6];
	memset(fasciaOraria, 0, 6);
	char dipendente[45];
	memset(fasciaOraria, 0, 45);
	char giorno[10];
	memset(fasciaOraria, 0, 10);

	printf("Cinema del calendario dei turni: ");
	scanf("%d", &cinema);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = sizeof(int);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[3];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call calendarioTurni(?)";

	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);
	
	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);
			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = fasciaOraria;
			rs_bind[0].buffer_length = sizeof(fasciaOraria);

			rs_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = giorno;
			rs_bind[1].buffer_length = sizeof(giorno);

			rs_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[2].is_null = &is_null[2];
			rs_bind[2].buffer = dipendente;
			rs_bind[2].buffer_length = sizeof(dipendente);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}

				printf("Fascia Oraria: %s, Giorno: %s, Dipendente: %s\n", rs_bind[0].buffer, rs_bind[1].buffer, rs_bind[2].buffer);
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void turniDelGiorno()
{
	int cinema;
	memset(&cinema, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	char fasciaOraria[6];
	memset(fasciaOraria, 0, 6);
	char dipendente[45];
	memset(fasciaOraria, 0, 45);
	char giorno[10];
	memset(fasciaOraria, 0, 10);
	char tipo[15];
	memset(tipo, 0, 15);

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[0] = sizeof(int);
	length[1] = sizeof(data);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[4];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select TD.FasciaOrariaTurno, TD.GiornoTurno, TD.CodiceFiscaleDipendente, D.Tipo from TurnoDelDipendente TD join Dipendente D on TD.CodiceFiscaleDipendente = D.CodiceFiscale where D.Cinema = ? and TD.GiornoTurno = ?";
		
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&data;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = fasciaOraria;
			rs_bind[0].buffer_length = sizeof(fasciaOraria);

			rs_bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[1].is_null = &is_null[1];
			rs_bind[1].buffer = giorno;
			rs_bind[1].buffer_length = sizeof(giorno);

			rs_bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[2].is_null = &is_null[2];
			rs_bind[2].buffer = dipendente;
			rs_bind[2].buffer_length = sizeof(dipendente);
		
			rs_bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[3].is_null = &is_null[3];
			rs_bind[3].buffer = tipo;
			rs_bind[3].buffer_length = sizeof(tipo);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Fascia Oraria: %s, Giorno: %s, Dipendente: %s, Tipo: %s\n", rs_bind[0].buffer, rs_bind[1].buffer, rs_bind[2].buffer, rs_bind[3].buffer);
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void inserisciFilm()
{
	char film[128];
	memset(film, 0, 128);
	char casa[45];
	memset(casa, 0, 45);
	int durata;
	memset(&durata, 0, 4);
	printf("Nome del film da inserire: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	
	printf("Nome della casa cinematografica: ");
	scanf("%[^\n]", casa);
	fflush(stdin);

	printf("Durata in minuti: ");
	scanf("%d", &durata);
	fflush(stdin);
	
	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[3];
	unsigned long length[3];
	length[0] = strlen(film);
	length[2] = strlen(casa);
	length[1] = sizeof(int);
	int status;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "insert into Film values(?, ?, ?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &durata;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = film;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = casa;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	printf("Inserimento effettuato\n");
}


void inserisciAttore()
{
	char nome[45];
	memset(nome, 0, 45);
	char cognome[45];
	memset(cognome, 0, 45);
	printf("Nome dell'attore da inserire: ");
	scanf("%[^\n]", nome);
	fflush(stdin);
	
	printf("Cognome: ");
	scanf("%[^\n]", cognome);
	fflush(stdin);
	
	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[0] = strlen(nome);
	length[1] = strlen(cognome);
	int status;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "insert into Attore values(?, ?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nome;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = cognome;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	printf("Inserimento effettuato\n");
}


void legaFilmAttore()
{
	char film[128];
	memset(film, 0, 128);
	char nome[45];
	memset(nome, 0, 45);
	char cognome[45];
	memset(cognome, 0, 45);

	printf("Nome del film: ");
	scanf("%[^\n]", film);
	fflush(stdin);

	printf("Nome dell'attore: ");
	scanf("%[^\n]", nome);
	fflush(stdin);
	
	printf("Cognome: ");
	scanf("%[^\n]", cognome);
	fflush(stdin);
	
	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[3];
	unsigned long length[3];
	length[0] = strlen(nome);
	length[1] = strlen(cognome);
	length[2] = strlen(film);
	int status;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "insert into AttoriNelFilm values(?, ?, ?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = nome;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = cognome;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	printf("Inserimento effettuato\n");

}


void bigliettiConfermati()
{
	int cinema;
	memset(&cinema, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	int mese;
	memset(&mese, 0, 4);
	int totale;
	memset(&totale, 0, 4);

	printf("Cinema in cui contare i biglietti confermati: ");
	scanf("%d", &cinema);
	fflush(stdin);

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Mese in cui contare biglietti confermati: ");
	scanf("%d", &mese);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[0] = sizeof(int);
	length[1] = sizeof(int);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select count(*) from Biglietto where MONTH(DataProiezione) = ? and Tipo = 'Confermato' and CinemaDellaProiezione = ?";	
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &cinema;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &mese;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Numero biglietti confermati nel mese inserito: --> %d <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);	

}


void bigliettiAnnullati()
{
	int cinema;
	memset(&cinema, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	int mese;
	memset(&mese, 0, 4);
	int totale;
	memset(&totale, 0, 4);

	printf("Cinema in cui contare i biglietti annullati: ");
	scanf("%d", &cinema);
	fflush(stdin);

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Mese in cui contare biglietti annullati: ");
	scanf("%d", &mese);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[0] = sizeof(int);
	length[1] = sizeof(int);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select count(*) from Biglietto where MONTH(DataProiezione) = ? and Tipo = 'Annullato' and CinemaDellaProiezione = ?";	
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &cinema;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &mese;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Numero biglietti annullati nel mese inserito: --> %d <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);	
	
}


void bigliettiAcquistatiNonConfermati()
{
	int cinema;
	memset(&cinema, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	int mese;
	memset(&mese, 0, 4);
	int totale;
	memset(&totale, 0, 4);

	printf("Cinema in cui contare i biglietti acquistati non confermati: ");
	scanf("%d", &cinema);
	fflush(stdin);

	data.time_type = MYSQL_TYPE_DATETIME;
	printf("Mese in cui contare biglietti acquistati non confermati: ");
	scanf("%d", &mese);
	fflush(stdin);

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[0] = sizeof(int);
	length[1] = sizeof(int);
	int num_fields, status, i;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select count(*) from Biglietto where MONTH(DataProiezione) = ? and Tipo = 'AcquistatoNonConfermato' and CinemaDellaProiezione = ?";	
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &cinema;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &mese;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Numero biglietti acquistati ma non confermati nel mese inserito: --> %d <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);	

}


void bigliettiConfermatiProiez()
{
	char film[128];
	memset(film, 0, 128);
	int totale;
	memset(&totale, 0, 4);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	puts("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[5];
	unsigned long length[5];
	length[0] = sizeof(data);
	length[1] = sizeof(ora);
	length[2] = strlen(film);
	length[3] = sizeof(int);
	length[4] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select count(*) from Biglietto where Tipo = 'Confermato' and DataProiezione = ? and OraProiezione = ? and Film = ? and SalaDellaProiezione = ? and CinemaDellaProiezione = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &sala;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &cinema;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[0].buffer = (char *)&data;
	ps_params[0].length = 0;
	ps_params[0].is_null = 0;
	
	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Numero biglietti confermati per la proiezione inserita: --> %d <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);

}

void bigliettiAnnullatiProiez()
{
	char film[128];
	memset(film, 0, 128);
	int totale;
	memset(&totale, 0, 4);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	puts("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[5];
	unsigned long length[5];
	length[0] = sizeof(data);
	length[1] = sizeof(ora);
	length[2] = strlen(film);
	length[3] = sizeof(int);
	length[4] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select count(*) from Biglietto where Tipo = 'Annullato' and DataProiezione = ? and OraProiezione = ? and Film = ? and SalaDellaProiezione = ? and CinemaDellaProiezione = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[3].buffer_type = MYSQL_TYPE_LONG;
	ps_params[3].buffer = &sala;
	ps_params[3].length = &length[3];
	ps_params[3].buffer_length = sizeof(int);
	ps_params[3].is_null = 0;

	ps_params[4].buffer_type = MYSQL_TYPE_LONG;
	ps_params[4].buffer = &cinema;
	ps_params[4].length = &length[4];
	ps_params[4].buffer_length = sizeof(int);
	ps_params[4].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[0].buffer = (char *)&data;
	ps_params[0].length = 0;
	ps_params[0].is_null = 0;
	
	ps_params[1].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[1].buffer = (char *)&ora;
	ps_params[1].length = 0;
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Numero biglietti annullati per la proiezione inserita: --> %d <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void simulaPostProiezione()
{
	char film[128];
	memset(film, 0, 128);
	int totale;
	memset(&totale, 0, 4);
	int scelta;
	int cinema;
	memset(&cinema, 0, 4);
	int sala;
	memset(&sala, 0, 4);
	MYSQL_TIME data;
	memset(&data, 0, sizeof(data));
	MYSQL_TIME ora;
	memset(&ora, 0, sizeof(ora));

	data.time_type = MYSQL_TYPE_DATETIME;
	ora.time_type = MYSQL_TYPE_DATETIME;

	printf("Funzione che simula la conferma e l'annullamento dei biglietti\ndi una proiezione a cui sono associati dei biglietti\n");

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	printf("Sala: ");
	scanf("%d", &sala);
	fflush(stdin);
	printf("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);
	printf("Anno: ");
	scanf("%d", &data.year);
	fflush(stdin);
	printf("Mese: ");
	scanf("%d", &data.month);
	fflush(stdin);
	printf("Giorno: ");
	scanf("%d", &data.day);
	fflush(stdin);
	printf("Ora: ");
	scanf("%d", &ora.hour);
	fflush(stdin);
	printf("Minuti: ");
	scanf("%d", &ora.minute);
	fflush(stdin);
	ora.second = 0;

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[5];
	unsigned long length[5];
	length[3] = sizeof(data);
	length[4] = sizeof(ora);
	length[2] = strlen(film);
	length[0] = sizeof(int);
	length[1] = sizeof(int);
	int status;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "call simulaPostProiezione(?,?,?,?,?)";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[1].buffer_type = MYSQL_TYPE_LONG;
	ps_params[1].buffer = &sala;
	ps_params[1].length = &length[1];
	ps_params[1].buffer_length = sizeof(int);
	ps_params[1].is_null = 0;

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[2].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[2].buffer = film;
	ps_params[2].length = &length[2];
	ps_params[2].is_null = 0;

	ps_params[3].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[3].buffer = (char *)&data;
	ps_params[3].length = 0;
	ps_params[3].is_null = 0;
	
	ps_params[4].buffer_type = MYSQL_TYPE_DATETIME;
	ps_params[4].buffer = (char *)&ora;
	ps_params[4].length = 0;
	ps_params[4].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);

	mysql_stmt_close(stmt);

	printf("Simulazione completata\n");
}


void incassoFilmCinema()
{
	char film[128];
	memset(film, 0, 128);
	int totale;
	memset(&totale, 0, 4);
	int cinema;
	memset(&cinema, 0, 4);

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);
	puts("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[2];
	unsigned long length[2];
	length[1] = strlen(film);
	length[0] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select sum(IncassoTotale) from Proiezione where Cinema = ? and Film = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	ps_params[1].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[1].buffer = film;
	ps_params[1].length = &length[1];
	ps_params[1].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Incasso totale dato da tutte le poiezioni del film inserito: --> %d € <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void incassoFilm()
{
	char film[128];
	memset(film, 0, 128);
	int totale;
	memset(&totale, 0, 4);

	puts("Film: ");
	scanf("%[^\n]", film);
	fflush(stdin);

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = strlen(film);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select sum(IncassoTotale) from Proiezione where Film = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_VAR_STRING;
	ps_params[0].buffer = film;
	ps_params[0].length = &length[0];
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Incasso totale dato da tutte le poiezioni del film inserito: --> %d € <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
	
}


void incassoCinema()
{
	int totale;
	memset(&totale, 0, 4);
	int cinema;
	memset(&cinema, 0, 4);

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = sizeof(int);
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select sum(IncassoTotale) from Proiezione where Cinema = ?";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Incasso totale dato da tutte le poiezioni di tutti i film nel cinema inserito: --> %d € <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);

}


void incassoCatena()
{
	int totale;
	memset(&totale, 0, 4);

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	int num_fields, status, i;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select sum(IncassoTotale) from Proiezione";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_LONG;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = &totale;
			rs_bind[0].buffer_length = sizeof(int);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Incasso totale della catena di cinema: --> %d € <--\n", totale); 
				}
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void maschereCinema()
{
	
	char dipendente[45];
	memset(dipendente, 0, 45);
	int cinema;
	memset(&cinema, 0, 4);

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = sizeof(int);
	int num_fields, status, i;
	int j = 0;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select CodiceFiscale from Dipendente where Cinema = ? and Tipo = 'Maschera'";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = dipendente;
			rs_bind[0].buffer_length = sizeof(dipendente);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Maschera n.%d: -> %s\n", j, dipendente); 
				}
				j++;
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);
}


void proiezionistiCinema()
{
	char dipendente[45];
	memset(dipendente, 0, 45);
	int cinema;
	memset(&cinema, 0, 4);

	printf("Cinema: ");
	scanf("%d", &cinema);
	fflush(stdin);

	MYSQL_RES *rs_metadata;
	MYSQL_FIELD *fields;
	MYSQL_BIND *rs_bind;
	my_bool is_null[1];

	MYSQL_STMT *stmt;
	MYSQL_BIND ps_params[1];
	unsigned long length[1];
	length[0] = sizeof(int);
	int num_fields, status, i;
	int j = 0;
	
	stmt = mysql_stmt_init(con);
	if (!stmt) {
		printf("Could not initialize statement\n");
		exit(1);
	}

	char *query = "select CodiceFiscale from Dipendente where Cinema = ? and Tipo = 'Proiezionista'";
	status = mysql_stmt_prepare(stmt, query, strlen(query));
	test_stmt_error(stmt, status);

	memset(ps_params, 0, sizeof(ps_params));

	ps_params[0].buffer_type = MYSQL_TYPE_LONG;
	ps_params[0].buffer = &cinema;
	ps_params[0].length = &length[0];
	ps_params[0].buffer_length = sizeof(int);
	ps_params[0].is_null = 0;

	status = mysql_stmt_bind_param(stmt, ps_params);
	test_stmt_error(stmt, status);

	status = mysql_stmt_execute(stmt);
	test_stmt_error(stmt, status);
	
	do {
		num_fields = mysql_stmt_field_count(stmt);		
	
		if (num_fields > 0) {

			if (con->server_status & SERVER_PS_OUT_PARAMS)
				printf("The stored procedure has returned output in OUT/INOUT parameter(s)\n");

			rs_metadata = mysql_stmt_result_metadata(stmt);
			test_stmt_error(stmt, rs_metadata == NULL);

			fields = mysql_fetch_fields(rs_metadata);
			rs_bind = (MYSQL_BIND *)malloc(sizeof(MYSQL_BIND) * num_fields);
			if (!rs_bind) {
				printf("Cannot allocate output buffers\n");
				exit(1);
			}
			memset(rs_bind, 0, sizeof(MYSQL_BIND) * num_fields);

			rs_bind[0].buffer_type = MYSQL_TYPE_VAR_STRING;
			rs_bind[0].is_null = &is_null[0];
			rs_bind[0].buffer = dipendente;
			rs_bind[0].buffer_length = sizeof(dipendente);

			status = mysql_stmt_bind_result(stmt, rs_bind);
			test_stmt_error(stmt, status);

			while(1){
				status = mysql_stmt_fetch(stmt);

				if(status == 1 || status == MYSQL_NO_DATA){
					break;
				}else{
					printf("Proiezionista n.%d: -> %s\n", j, dipendente); 
				}
				j++;
			}

			mysql_free_result(rs_metadata);
			free(rs_bind);
		} else {
			printf("--------------------------------------\n");
		}
		
		status = mysql_stmt_next_result(stmt);
		if (status > 0)
			test_stmt_error(stmt, status);
	} while (status == 0);

	mysql_stmt_close(stmt);	

}


char *menu0ges = "------------ AZIONI PER EFFETTUARE LA 'SEGNALAZIONE'\n| 1)Mostra proiezionista che gestisce una proiezione\n| 2)Mostra maschere in una fascia oraria di un giorno\n------------ \n3)Aggiorna proiezionista della proiezione\n4)Aggiungi maschera in una fascia oraria di un giorno\n";
char *menu1ges = "5)Rimuovi proiezionista da proiezione\n6)Rimuovi maschera in una fascia oraria\n7)Mostra proiezionisti disponibili\n8)Mostra sale disponibili\n9)Aggiungi proiezione\n";
char *menu2ges = "10)Aggiungi proiezione senza proiezionista\n11)Mostra calendario dei turni\n12)Mostra turni di un giorno specifico\n13)Inserisci film\n14)Inserisci attore\n15)Lega attore al film\n";
char *menu3ges = "16)Numero biglietti confermati nel mese\n17)Numero biglietti annullati nel mese\n18)Numero biglietti acqustati ma non confermati nel mese\n";
char *menu4ges = "19)Numero biglietti confermati per una certa proiezione\n20)Numero biglietti annullati per una certa proiezione\n";
char *menu5ges = "21)Incasso di un film in un cinema\n22)Incasso di un film nella catena di cinema\n23)Incasso totale di un cinema\n24)Incasso totale della catena di cinema\n25)Simula post proiezione\n26)Mostra maschere di un cinema\n27)Mostra proiezionisti di un cinema\n28)Indietro\n";

void gestore()
{
	char *menuGes[1024];
	memset(menuGes, 0, 1024);
	strcat(menuGes, menu0ges);
	strcat(menuGes, menu1ges);
	strcat(menuGes, menu2ges);
	strcat(menuGes, menu3ges);
	strcat(menuGes, menu4ges);
	strcat(menuGes, menu5ges);
	
	if (mysql_real_connect(con, conf.host, conf.username, conf.password, conf.database, conf.port, NULL, 0) == NULL) {
		fprintf(stderr, "Connection error: %s\n", mysql_error(con));
		exit(1);
	}

	int scelta;

	while(1){
		puts(scegli);
		puts(menuGes);
		//fflush(stdin);
		puts(inserisciScelta);
		scanf("%d", &scelta);
		fflush(stdin);
		system("clear");
		switch (scelta) {
			
			case 1:
				mostraProiezionistaDiProiez();
				break;
			case 2:
				mostraMaschere();
				break;
			case 3:
				aggiornaProiezionista();
				break;
			case 4:
				aggiungiMaschera();
				break;
			case 5:
				rimuoviProiezionistaDaProiezione();
				break;
			case 6:
				rimuoviMaschera();
				break;
			case 7:
				proiezionistiDisponibili();
				break;
			case 8:
				saleDisponibili();
				break;
			case 9:
				aggiungiProiezione();
				break;
			case 10:
				aggiungiProiezioneNoProiez();
				break;
			case 11:
				calendarioTurni();
				break;
			case 12:
				turniDelGiorno();
				break;
			case 13:
				inserisciFilm();
				break;
			case 14:
				inserisciAttore();
				break;
			case 15:
				legaFilmAttore();
				break;
			case 16:
				bigliettiConfermati();
				break;
			case 17:
				bigliettiAnnullati();
				break;
			case 18:
				bigliettiAcquistatiNonConfermati();
				break;
			case 19:
				bigliettiConfermatiProiez();
				break;
			case 20:
				bigliettiAnnullatiProiez();
				break;
			case 21:
				incassoFilmCinema();
				break;
			case 22:
				incassoFilm();
				break;
			case 23:
				incassoCinema();
				break;
			case 24:
				incassoCatena();
				break;
			case 25:
				simulaPostProiezione();
				break;
			case 26:
				maschereCinema();
				break;
			case 27:
				proiezionistiCinema();
				break;
			case 28:
				mysql_close(con);
				return;
			default:
				puts(sceltaScorretta);
				break;
		}
	}
	

}


void popolaDB()
{

}


char *menu0 = "1)Entra come Gestore\n2)Entra come Cliente\n3)Entra come Visitatore\n4)Entra come Maschera\n5)Cancella DB\n6)Popola DB per testing\n7)Popola DB (sistema pseudo-reale)\n8)Esci\n";


void main(int argc, char **argv)
{
	con = mysql_init(NULL);

	if (con == NULL) {
		fprintf(stderr, "Initilization error: %s\n", mysql_error(con));
		exit(1);
	}	

	int scelta;
	char elim[1];

	while(1){
		puts(scegli);
		puts(menu0);
		puts(inserisciScelta);
		scanf("%d", &scelta);
		fflush(stdin);
		system("clear");
		switch (scelta) {

			case 1:
				load_file(&config, "config_gestore.json");
				parse_config();
				gestore();
				break;
			case 2:
				load_file(&config, "config_cliente.json");
				parse_config();
				cliente();
				break;
			case 3:
				load_file(&config, "config_visitatore.json");
				parse_config();
				visitatore();
				break;
			case 4:
				load_file(&config, "config_maschera.json");
				parse_config();
				maschera();
				break;
			case 5:
				memset(elim, 0, 1);
				printf("Sicuro di voler eliminare il DB ? (y per andare avanti): ");
				scanf("%s", elim);
				if(strcmp(elim, "y") == 0){
					load_file(&config, "config_gestore.json");
					parse_config();
					cancella_db();
				}
				break;
			case 6:
				popolaDB();
				return;
			case 7:
				popolaDB();
				return;
			case 8:
				mysql_close(con);
				return;
			default:
				puts(sceltaScorretta);
				break;
		}

		//tutte le scelte (se inserite correttamente) mi chiudono la connessione, la riapro
		if (scelta == 1 || scelta == 2 || scelta == 3 || scelta == 4 || scelta == 5){
			con = mysql_init(NULL);

			if (con == NULL) {
				fprintf(stderr, "Initilization error: %s\n", mysql_error(con));
				exit(1);
			}
		}
	
	}
}

