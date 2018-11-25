#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include "sqlite3.h"

sqlite3 *db;
char *zErrMsg = 0;
int rc;
/*returns the result of an sql query and prints to terminal */

void breaker(char *hash, char *task, char *size, char *pass, int passLength, int index, int prefix)
{
	//printf("\n**IN BREAKER** \n%s, %lu, %d", hash, sizeof(task)/sizeof(task[0]), atoi(size));
	//if (strcmp(task, "NULL")) {
	//	printf("\nNull here");
	//}
	//printf("\n\nTASK: ");
	//for (int i = 0; i < sizeof(task)/sizeof(task[0]); i++) {
	//	printf("%c", task[i]);
	//}
	char together[passLength];


	while(pass[index] <= 126) {
		if (index + 1 < passLength - 1) {
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
			char md5string[33];
			for (int i = 0; i < 16; i++) {
				sprintf(&md5string[i*2], "%02x", (unsigned int)thisHash[i]);
			}
			//printf(" Hash: ");
			//for (int i = 0; i < sizeof(md5string); i++) {
				//printf("%02x", thisHash[i]);
			//	printf("%c", md5string[i]);
			//}
			//printf("\n");
			/*
			This still doesn't find the correct hash :[
			*/
			if (strncmp(md5string, hash, 32) == 0) {
				printf("\n\n*HASH FOUND*\n\n");
				for (int i = 0; i < passLength; i++) {
					printf("%c", together[i]);
				}
				break;
			}
		}
        pass[index]++;
	}




/*	printf("\n");
	char together[128];
	for (int i = 0; i < sizeof(together); i++) {
		together[i] = '\0';
	}
	strcat(together, task);

	char suffix[3] = {'a', 'a', '\0'};

	strcat(together, suffix);
	printf("\nTOGETHER IN BREAKER: ");
	for (int i = 0; i < sizeof(together); i++) {
		printf("%c", together[i]);
	}
	printf("\n");
	*/
}
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i, myID;
    char myHash[33], myTask[12], myTaskSize[128], myAdded[128];

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
	if(sqlite3_close(db)) {
		printf("Database closed.\n");
	}
	/*
	"myTask" is where we store the prefix of this job.
	i.e. if the job is "all 4 character passwords starting with 't', then
	myTask = {'t', '\0'}"
	*/
	printf("\nHash: ");
	for (int i = 0; i < sizeof(myHash)/sizeof(myHash[0]); i++) {
		printf("%c", myHash[i]);
	}
	printf("\n");

	/* the size of our prefix, in characters */
	int prefix;
	if (strcmp(myTask, "NULL") == 0) {
		prefix = 0;
	} else {
		prefix = strlen(myTask);
		printf("\n\nTASK: ");
		for (int i = 0; i < sizeof(myTask)/sizeof(myTask[0]); i++) {
			printf("%c", myTask[i]);
		}
		printf("\n");
	}

	int passLength = prefix + atoi(myTaskSize);

	char pass[passLength];
	for (int i = 0; i < passLength; i++) {
		pass[i] = 32;
	}
	int index = 0;
	//for (int)
	breaker(myHash, myTask, myTaskSize, pass, passLength, index, prefix);
    /*
    size_t len;
	for(i=0; i<argc; i++){
		sprintf(myTask + strlen(myTask), "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
    */
    //printf("\n%s, %s, %s, %s\n", myHash, myTask, myTaskSize, myAdded);

	return 0;
}

int main() {
	// Store the 'task' into this character array
	char task[9];
	// Store the password to crack into a buffer. Don't forget the trailing null, which is why
	// we need a buffer of 9 bytes eventhough "password" is only 8
	//snprintf(task, sizeof(task), "password");

	// Allocate a c-string array that can store a SQL command
	char sqlcommand[1024];
	// Generate the SQL command based on
	//snprintf(sqlcommand, sizeof(sqlcommand), "INSERT INTO jobs (task, tasksize, added)
                                                    //VALUES ('password', 9, CURRENT_TIMESTAMP)");
    /*This will find the task that was added the longest time ago. */

    snprintf(sqlcommand, sizeof(sqlcommand), "SELECT * FROM jobs ORDER BY ID LIMIT 1;");
	printf("Command executed: %s\n", sqlcommand);

	// This opens the SQLite database, which is actually the file 'database.db'
	// If it can't open the database, error out
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
		sqlite3_close(db);
	}

	// Close the connection to the database


	return 0;
}
