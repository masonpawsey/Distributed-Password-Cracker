#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sqlite3.h"

sqlite3 *db;
char *zErrMsg = 0;
char sqlcommand[1024];
int rc;
int lastID= 0;


static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	char host[12], hash[34], task[10], size[10], id[10], dtime[25];

	sprintf(host, "%s", argv[0] ? argv[0] : "NULL");
	sprintf(hash, "%s", argv[1] ? argv[1] : "NULL");
	sprintf(task, "%s", argv[2] ? argv[2] : "NULL");
	sprintf(size, "%s", argv[3] ? argv[3] : "NULL");
	sprintf(id, "%s", argv[4] ? argv[4] : "NULL");
	sprintf(dtime, "%s", argv[5] ? argv[5] : "NULL");

	if (atoi(id) < 1) {
		if (strcmp(task, "") == 0) {
			printf("*PASSWORD NOT FOUND*\n");
            printf("Time: %s\n", dtime);
		}
		else{
			printf("*PASSWORD FOUND*\n%s\n%s\n", hash, task);
            printf("Time: %s\n", dtime);
        }
		exit(0);
	}
	if (atoi(id) > lastID) {
		if (strcmp(task, "NULL") == 0) {
			printf("%s All %s character passwords. Time: %s\n", host, size, dtime);
		} else {
			printf("%s, %s character passwords starting with '%s' Time: %s\n", host, size, task, dtime);
		}
		lastID = atoi(id);
	}
	return 0;
}



int main(int argc, char *argv[])
{
	rc = sqlite3_open("database.db", &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
	}
	while(1) {
		snprintf(sqlcommand, sizeof(sqlcommand), "SELECT * FROM progress WHERE ID<1;");

		rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			//fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		snprintf(sqlcommand, sizeof(sqlcommand), "SELECT * FROM progress ORDER BY ID DESC LIMIT 1;");

		rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			//fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		sleep(1);
	}

	sqlite3_close(db);
	return 0;
}
