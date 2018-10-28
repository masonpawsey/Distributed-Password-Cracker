all: example read

example: example.c sqlite3.c
	gcc sqlite3.c -o insert example.c -lpthread -ldl

read: red5.c sqlite3.c
	gcc sqlite3.c -o read red5.c -lpthread -ldl
