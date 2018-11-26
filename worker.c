#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include "sqlite3.h"

sqlite3 *db;
char *zErrMsg = 0;
int rc;
/*returns the result of an sql query and prints to terminal */

char myHash[33], myTask[12], myTaskSize[128], myAdded[128];
char sqlcommand[1024];
int myID, flag = 1;
static int callback(void *NotUsed, int argc, char **argv, char **azColName);

void breaker(char *hash, char *task, char *size, char *pass, int passLength, int index, int prefix)
{
	/*
	"together" stores our prefix + generated portion in one array
	*/
	char together[passLength];


	while(pass[index] <= 126) {
		//printf("Index + 1: %d, passLength: %d", index + 1, passLength);
		if (index + 1 < passLength - prefix) {
			pass[index + 1] = 32;
			breaker(hash, task, size, pass, passLength, index + 1, prefix);
		}
		int flag = 0;
		for (int i = 0; i < passLength; i++) {
            if (pass[i] > 126) {
                flag = 1;
            }
        }
		if (flag == 0) {
			memset(together, '\0', sizeof(together));
			if (prefix > 0) {
				strcat(together, task);
			}
			strcat(together, pass);

			unsigned char thisHash[MD5_DIGEST_LENGTH];

			MD5(together, passLength, thisHash);
			//for (int i = 0; i < passLength; i++) {
			//	printf("%c", together[i]);
			//}
			//printf("\n");
			char md5string[33];
			for (int i = 0; i < 16; i++) {
				sprintf(&md5string[i*2], "%02x", (unsigned int)thisHash[i]);
			}

			if (strncmp(md5string, hash, 32) == 0) {
				printf("\n\n*HASH FOUND*\n\n");
				flag = 0;
				for (int i = 0; i < passLength; i++) {
					printf("%c", together[i]);
				}
				printf("\n");
				exit(0);
			}
		}
        pass[index]++;
	}
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int myID;
    //char myHash[33], myTask[12], myTaskSize[128], myAdded[128];

	for (int i = 0; i < sizeof(myTask); i++) {
		myTask[i] = '\0';
	}
	for (int i = 0; i < sizeof(myHash)/sizeof(myHash[0]); i++) {
		myHash[i] = '\0';
	}

	sprintf(myHash, "%s", argv[0] ? argv[0] : "NULL");
    sprintf(myTask, "%s", argv[1] ? argv[1] : "NULL");
    sprintf(myTaskSize, "%s", argv[2] ? argv[2] : "NULL");
    sprintf(myAdded, "%s", argv[3] ? argv[3] : "NULL");
	myID = atoi(argv[4]);

    char sqlcommand[1024];
	/*
	Insert which job this host will begin into the progress table
	*/
    snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO progress (host, hash, task, size, ID, time) VALUES ('Red5', '%s', '%s', '%s', %d, CURRENT_TIMESTAMP)", myHash, myTask, myTaskSize, myID);
    rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	/*
	Delete the job we took from the jobs table
	*/
    snprintf(sqlcommand, sizeof(sqlcommand), "DELETE FROM jobs WHERE ID = %d;", myID);
    printf("sqlcommand: %s", sqlcommand);
    rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	return 0;
}

int main() {
	while (flag == 1) {
		// Store the 'task' into this character array
		char task[9];

		// Allocate a c-string array that can store a SQL command
		//char sqlcommand[1024];

	    /*This will find the task that was added the longest time ago. */

	    snprintf(sqlcommand, sizeof(sqlcommand), "SELECT * FROM jobs ORDER BY ID LIMIT 1;");
		printf("Command executed: %s\n", sqlcommand);

		/* This opens the SQLite database, which is actually the file 'database.db'
		 If it can't open the database, error out */
		rc = sqlite3_open("database.db", &db);
		if( rc ){
			fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return(1);
		}

		// Run the SQL command on the database. Calls "callback", which handles
		// the return values from the DB
		rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}

		sqlite3_close(db);
		printf("\nHash: ");
		for (int i = 0; i < sizeof(myHash)/sizeof(myHash[0]); i++) {
			printf("%c", myHash[i]);
		}
		printf("\n");

		/* the size of our prefix, in characters */
		int prefix;
		//printf("strlen(mytask): %ld\n", strlen(myTask));
		if (strcmp(myTask, "NULL") == 0) {
			prefix = 0;
			printf("\nTASK: All %d character passwords\n", atoi(myTaskSize));
		} else {
			prefix = strlen(myTask);
			printf("\nTASK: All %d character passwords starting with: ", atoi(myTaskSize) + prefix);
			for (int i = 0; i < sizeof(myTask)/sizeof(myTask[0]); i++) {
				printf("%c", myTask[i]);
			}
			printf("\n");
		}
		//printf("Prefix: %d", prefix);
		int passLength = prefix + atoi(myTaskSize);
		//printf("\nPasslength: %d\n", passLength);
		char pass[passLength];
		for (int i = 0; i < passLength; i++) {
			pass[i] = 32;
		}
		int index = 0;
		//for (int)
		breaker(myHash, myTask, myTaskSize, pass, passLength, index, prefix);

		// Close the connection to the database
	}
	snprintf(sqlcommand, sizeof(sqlcommand), "DELETE from jobs;");

	rc = sqlite3_open("database.db", &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}

	// Run the SQL command on the database. Calls "callback", which handles
	// the return values from the DB
	rc = sqlite3_exec(db, sqlcommand, callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db);

	return 0;
}
