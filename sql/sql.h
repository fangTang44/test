#ifndef MYSQL_H
#define MYSQL_H
#include <mysql/mysql.h>
#include <string.h>
MYSQL* MySqlCreate(MYSQL* mysql_conn, const char* host, const char* user, const char* passwd, const char* db);
int MySqlQuery(MYSQL* mysql, char* q, MYSQL_RES* res);
#endif // MYSQL_H