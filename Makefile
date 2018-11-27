all: jobs worker

jobs: job_spawner.c sqlite3.c
	gcc sqlite3.c -o jobs job_spawner.c -lpthread -ldl

worker: worker.c sqlite3.c
	gcc sqlite3.c -o worker worker.c -lpthread -ldl -lcrypto -lssl

# FOR MAC: gcc sqlite3.c -o worker worker.c -lpthread -ldl -lcrypto -lssl -I /usr/local/opt/openssl/include -L /usr/local/opt/openssl/lib

clean:
	rm jobs worker
