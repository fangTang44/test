#include "sql.h"

// 创建mysql连接
MYSQL* MySqlCreate(MYSQL* mysql_conn, const char* host, const char* user, const char* passwd, const char* db) {
	if (mysql_conn == NULL || host == NULL ||
		user == NULL || passwd == NULL || db == NULL) {
		perror("para is null\n");
		return NULL;
	}
	MYSQL* mysql = mysql_init(mysql_conn);
	if (!mysql) {
		perror("mysql init error\n");
	}
	mysql = mysql_real_connect(mysql, host, user, passwd, db, 0, NULL, 0);
	if (!mysql) {
		perror("mysql connect error\n");
	}
	return mysql;
}

// Book类执行sql语句
// 失败返回-1
int MySqlQuery(MYSQL* mysql, char* q, MYSQL_RES* res) {
	// 空处理
	if (mysql == NULL || q == NULL) {
		perror("para is null\n");
		return -1;
	}

	// sql语句执行
	if (mysql_query(mysql, q)) {
		perror("mysql query error\n");	
		return -1;
	}

	// 执行结果处理
	char* cmd = strtok(q, " ");
	if (cmd == NULL) {
		return -1;
	}

	if (cmd == "select") {
		res = mysql_store_result(mysql);
	}

	return 0;
}