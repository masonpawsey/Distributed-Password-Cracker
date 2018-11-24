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

void sanitizer(char *unsafe_array, int prefix, int base, int rc)
{
    //rc = sqlite3_open("database.db", &db);
    //if (rc) {
    //    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    //    sqlite3_close(db);
    //}
    //size_t length = sizeof(unsafe_array)/sizeof(unsafe_array[0]);
    char safe[2*prefix+1];
    for (int i = 0; i < 2*prefix + 1; i++) {
        safe[i] = '\0';
    }
    int j = 0;
    for (int i = 0; i < prefix; i++) {
        if (unsafe_array[i] == 39) {
            //printf("Unsafe array: ");
            //printf("%c", unsafe_array[i]);
            safe[j] = 39;
            safe[j+1] = 39;
            j+=2;
        } else {
            safe[j] = unsafe_array[i];
            j++;
        }
    }
    //printf("\n Safe array: ");
    //for (int k = 0; k < prefix + 2; k++) {
    //    printf("%c", safe[k]);
    //}
    //printf("\n");
    snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO JOBS (task, tasksize, added, ID) VALUES ('%s', %d, CURRENT_TIMESTAMP, NULL)", safe, base);
    rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

}


void breaker(char *first, int index, int prefix, int base, int rc)
{

    while (first[index] <= 126) {
        if (index + 1 < prefix) {
            first[index + 1] = 32;
            breaker(first, index + 1, prefix, base, rc);
        }
        int flag = 0;
        for (int i = 0; i < prefix; i++) {
            if (first[i] > 126) {
                flag = 1;
            }
        }
        if (flag == 0) {
            sanitizer(first, prefix, base, rc);
            //printf("Find all 4 character passwords starting with ");
            //for (int i = 0; i < prefix; i++) {
                //printf("%c", first[i]);
                //snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO JOBS (task, tasksize, added, ID) VALUES (\"%s\", %d, CURRENT_TIMESTAMP, NULL)", first, base);
                //snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO JOBS (task, tasksize, added, ID) VALUES ('%s', %d, CURRENT_TIMESTAMP, NULL)", first, base);
                //rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
                //if( rc!=SQLITE_OK ){
		        //    fprintf(stderr, "SQL error: %s\n", zErrMsg);
		        //    sqlite3_free(zErrMsg);
	            //}
            //}
        }
        first[index]++;
    }

}


int main(int argc, char *argv[])
{
    if (argc != 3 || atoi(argv[1]) < atoi(argv[2]))
    usage();

    rc = sqlite3_open("database.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }


    int characters = atoi(argv[1]);
    int base = atoi(argv[2]);
    int prefix = characters - base;

    char first[prefix + 1];
    char second[base];
    char together[characters];

    for (int i = 0; i < prefix; i++) {
        first[i] = 32;
    }
    first[prefix] = '\0';
    for (int i = 0; i < base; i++) {
        second[i] = 'j';
    }

    breaker(first, 0, prefix, base, rc);

    sqlite3_close(db);
    return 0;
}
