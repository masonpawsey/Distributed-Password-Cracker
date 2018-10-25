#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

sqlite3 *db;
int rc;

int main() {
	// This opens the SQLite database, which is actually the file 'c.db'
	rc = sqlite3_open("c.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	// Allocate an c-string array that can store a SQL command
	char sqlcommand[1024];
	// Generate the SQL command based on 
	snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO carddata (cardid, position, load, `time`) VALUES ('0', %d, %f, %lu)", motorCount, sample, timestamp);
	
	// printf("%s\n", sqlcommand);
	
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