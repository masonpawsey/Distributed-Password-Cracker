#include <stdio.h>
#include "sqlite3.h"

#define RED   "\x1B[31m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"

int main(int argc, char **argv) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;

	if(argc != 2) {
		printf(RED "Please supply a table to view\n" RESET);
		return 1;
	}
	
	sqlite3_open("database.db", &db);

	if (db == NULL) {
		printf(RED "Failed to open DB\n" RESET);
		return 1;
	}


	char table[64];
	snprintf(table, sizeof(table), "SELECT * FROM %s", argv[1]);

	sqlite3_prepare_v2(db, table, -1, &stmt, NULL);
	int num_cols = sqlite3_column_count(stmt);
	if(num_cols < 1) {
		printf(RED "Table %s not found\n" RESET, argv[1]);
		return 1;
	}

	printf("Performing query on table:");
	printf(BLU " jobs\n" RESET);
	
	printf("Got results:\n");
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
		int num_cols = sqlite3_column_count(stmt);
		printf(BLU "| " RESET);
		for (i = 0; i < num_cols; i++) {
			switch (sqlite3_column_type(stmt, i)) {
			case (SQLITE3_TEXT):
				printf(BLU "%s | " RESET, sqlite3_column_text(stmt, i));
				break;
			case (SQLITE_INTEGER):
				printf(BLU "%d | " RESET, sqlite3_column_int(stmt, i));
				break;
			case (SQLITE_FLOAT):
				printf(BLU "%g | " RESET, sqlite3_column_double(stmt, i));
				break;
			default:
				break;
			}
		}
		printf("\n--------------------------------------\n");
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return 0;
}
