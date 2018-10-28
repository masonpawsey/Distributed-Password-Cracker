#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

sqlite3 *db;
char *zErrMsg = 0;
int rc;
/*returns the result of an sql query and prints to terminal */
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
    char myTask[128], myTaskSize[128], myAdded[128];
    sprintf(myTask, "%s", argv[0] ? argv[0] : "NULL");
    sprintf(myTaskSize, "%s", argv[1] ? argv[1] : "NULL");
    sprintf(myAdded, "%s", argv[2] ? argv[2] : "NULL");

    char sqlcommand[1024];

    snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO progress (host, task, time) VALUES ('Red5', '%s', CURRENT_TIMESTAMP)", myTask);
    rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

    snprintf(sqlcommand, sizeof(sqlcommand), "DELETE FROM jobs WHERE added = '%s';", myAdded);
    printf("sqlcommand: %s", sqlcommand);
    rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

    /*
    size_t len;
	for(i=0; i<argc; i++){
		sprintf(myTask + strlen(myTask), "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
    */
    printf("%s, %s, %s\n", myTask, myTaskSize, myAdded);

	return 0;
}

int main() {
	// Store the 'task' into this character array
	char task[9];
	// Store the password to crack into a buffer. Don't forget the trailing null, which is why
	// we need a buffer of 9 bytes eventhough "password" is only 8
	//snprintf(task, sizeof(task), "password");

	// Allocate an c-string array that can store a SQL command
	char sqlcommand[1024];
	// Generate the SQL command based on
	//snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO jobs (task, tasksize, added)
                                                    //VALUES ('password', 9, CURRENT_TIMESTAMP)");
    /*This will find the task that was added the longest time ago. */

    snprintf(sqlcommand, sizeof(sqlcommand), "SELECT * FROM jobs ORDER BY added LIMIT 1;");
	printf("Command executed: %s\n", sqlcommand);

	// This opens the SQLite database, which is actually the file 'database.db'
	// If it can't open the database, error out
	rc = sqlite3_open("database.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	// Run the SQL command on the database
	rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	// Close the connection to the database
	sqlite3_close(db);


	return 0;
}
