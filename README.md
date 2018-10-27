# Distributed Password Cracker - CMPS 3640 Fall 2018
This is the working repo for the distributed password cracked created for the CMPS 3640 Distributed and Parallel Computing class in Fall 2018.

## File Structure
##### `database.db`
This is the [SQLite database](https://www.sqlite.org/about.html). It's a flat-file that contains our tuple spaces.

##### `example.c`
This program inserts some data into the tuple space. Right now, it's an example that shows how to execute SQL on the tuple space. In the future, we should be able to `READ`, `INSERT`, and `REMOVE` tuple spaces.

Compiled on Mac OS High Sierra 10.13.16 with `gcc sqlite3.c -o insert example.c`

Compiled on Odin with `gcc sqlite3.c -o insert example.c -lpthread -ldl`

You can then run it with `./insert`

##### `sqlite3.c` & `sqlite3.h`
These files are used together to compile the SQLite3 drivers for `example.c`

### To do

[x] Create an example to `INSERT` into a tuple space

[ ] Flesh out example to include `READ` and `DELETE`

[ ] Integrate password cracking ablity

[ ] Distribute the process

##### Helpful Links
https://www.sqlite.org/cintro.html

https://github.com/danielmiessler/SecLists/tree/master/Passwords/Common-Credentials
