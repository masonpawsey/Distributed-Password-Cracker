all: jobs worker progress view

jobs: job_spawner.c sqlite3.c
	gcc sqlite3.c -o jobs job_spawner.c -lpthread -ldl

worker: worker.c sqlite3.c
	gcc -fopenmp sqlite3.c -o worker worker.c -lpthread -ldl -lcrypto -lssl

progress: progress.c sqlite3.c
	gcc sqlite3.c -o progress progress.c -lpthread -ldl

# FOR MAC: gcc sqlite3.c -o worker worker.c -lpthread -ldl -lcrypto -lssl -I /usr/local/opt/openssl/include -L /usr/local/opt/openssl/lib

view: view.c sqlite3.c
	gcc sqlite3.c -o view view.c

clean:
	rm jobs worker view
