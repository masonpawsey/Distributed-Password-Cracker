#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

sqlite3 *db;
char *zErrMsg = 0;
char sqlcommand[1024];
int rc;

void usage()
{
    printf("Usage: ./jobs [max number of characters] [base]\n");
    exit(0);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

void sanitizer(char *hash, char *unsafe_array, int prefix, int base, int rc)
{
    char safe[2*prefix+1];
    for (int i = 0; i < 2*prefix + 1; i++) {
        safe[i] = '\0';
    }
    int j = 0;
    for (int i = 0; i < prefix; i++) {
        if (unsafe_array[i] == 39) {
            safe[j] = 39;
            safe[j+1] = 39;
            j+=2;
        } else {
            safe[j] = unsafe_array[i];
            j++;
        }
    }
    snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO JOBS (hash, task, tasksize, added, ID) VALUES ('%s', '%s', %d, CURRENT_TIMESTAMP, NULL)", hash, safe, base);
    rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

}


void breaker(char *hash, char *first, int index, int prefix, int base, int rc)
{

    while (first[index] <= 126) {
        if (index + 1 < prefix) {
            first[index + 1] = 32;
            breaker(hash, first, index + 1, prefix, base, rc);
        }
        int flag = 0;
        for (int i = 0; i < prefix; i++) {
            if (first[i] > 126) {
                flag = 1;
            }
        }
        if (flag == 0) {
            sanitizer(hash, first, prefix, base, rc);
        }
        first[index]++;
    }

}


int main(int argc, char *argv[])
{
    if (argc != 3 || atoi(argv[1]) < atoi(argv[2]))
        usage();

    char hash[33];
    printf("Please enter a hash: ");
    fgets(hash, 32, stdin);
    hash[33] = '\0';

    rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
    int characters = atoi(argv[1]);
    int base = atoi(argv[2]);
    int passwords = 1;

    while (passwords <= base) {
        snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO JOBS (hash, tasksize, added, ID) VALUES ('%s', %d, CURRENT_TIMESTAMP, NULL)", hash, passwords);
        rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
        if( rc!=SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        passwords++;
    }
    while (characters >= passwords) {
        int prefix = passwords - base;
        char first[prefix + 1];
        for (int i = 0; i < prefix; i++) {
            first[i] = 32;
        }
        first[prefix] = '\0';
        //for (int i = 0; i < base; i++) {
        //    second[i] = 'j';
        //}
        breaker(hash, first, 0, prefix, base, rc);
        passwords++;
    }

    //if (characters > base)

    //char second[base];
    //char together[characters];



    sqlite3_close(db);
    return 0;
}
