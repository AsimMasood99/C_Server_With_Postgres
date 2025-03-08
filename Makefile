build: 
	gcc -o .build/main Backend/main.c -I /usr/include/postgresql -lpq -lcjson

run: 
	.build/main

db: 
	gcc -o .build/dbinit Backend/database_init.c -I /usr/include/postgresql -lpq 

initdb: 
	.build/dbinit
