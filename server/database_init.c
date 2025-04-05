#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>

void exit_with_error(PGconn *conn, PGresult *res) {
    fprintf(stderr, "%s\n", PQerrorMessage(conn));

    PQclear(res);
    PQfinish(conn);

    exit(1);
}

int main() {
    PGconn *conn = PQconnectdb("user=postgres dbname=testdb");

    if (PQstatus(conn) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }


    PGresult *res = PQexec(conn, "DROP TABLE IF EXISTS Users"); 
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        exit_with_error(conn, res);
    }
    PQclear(res);
    
    res = PQexec(conn, "CREATE TABLE Users(id SERIAL PRIMARY KEY, Username VARCHAR(20), Password INT)");  
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        exit_with_error(conn, res); 
    }
    PQclear(res);

    res = PQexec(conn, "INSERT INTO Users VALUES(DEFAULT, 'Asim', 123)");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        exit_with_error(conn, res); 
    }
    PQclear(res);

    res = PQexec(conn, "INSERT INTO Users VALUES(DEFAULT, 'Ahmad', 345)");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        exit_with_error(conn, res); 
    }
    PQclear(res);

    res = PQexec(conn, "INSERT INTO Users VALUES(DEFAULT, 'Ali', 567)");
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        exit_with_error(conn, res); 
    }
    PQclear(res);


    PQfinish(conn);

    return 0;
}
