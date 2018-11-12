all: jobs worker

jobs: example.c sqlite3.c
	gcc sqlite3.c -o jobs job_spawner.c -lpthread -ldl

worker: worker.c sqlite3.c
	gcc sqlite3.c -o worker worker.c -lpthread -ldl
