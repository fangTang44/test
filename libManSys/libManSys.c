#include "libManSys.h"
#include <mysql/mysql.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define BOOKNUMBER 200
#define READERNUMBER 100
#define BORROWMAX 128

struct book {
  // 书名
  char _name[15];
  // 作者
  char _author[15];
  // 出版社
  char _press[15];
  // 编号
  int _id;
  // 库存量
  int _number;
};

struct reader {
  // 姓名
  char _name[15];
  // 编号
  int _id;
  // 性别
  char _sex[4];
  // 最大可借阅数量
  int _iMax;
  // 当前可借阅数量
  int _iAmout;
  // 读者已经借阅的图书列表
  int _bllkLd[10];
};

#define BOOK_DATE                                                              \
  astBook[i]._id, astBook[i]._name, astBook[i]._author, astBook[i]._press,     \
      astBook[i]._number
#define READER_DATE                                                            \
  astReader[i]._id, astReader[i]._name, astReader[i]._sex, astReader[i]._iMax,  \
      astReader[i]._iAmout                                    

void showMainMeun() {
  system("clear");
  printf("*************欢迎进入****************\n");
  printf("*************图书馆管理系统**********\n");
  printf("*************主菜单******************\n");
  printf("*************1.图书管理**************\n");
  printf("*************2.读者管理**************\n");
  printf("*************3.借书还书登记**********\n");
  printf("*************0.关闭系统**************\n");
  printf("请选择0-3\n");
}

// 显示图书信息
int showBooks(MYSQL* mysql) {
	char* sql = "select * from Book";
	if (mysql_query(mysql, sql)) {
		perror("mysql query error\n");
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	if (res == NULL) {
		return -1;
	}

	int num = mysql_num_rows(res);
	int count = mysql_field_count(mysql);
	if (num == 0) {
		printf("暂无图书信息\n");
		return -1;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "书名", "作者", "数量");
	for (int i = 0; i < num; i++) {
		MYSQL_ROW row = mysql_fetch_row(res);
		for (int j = 0; j < count; j++) {
			printf("%-15s", row[j]);
		}
		printf("\n");
	}
	mysql_free_result(res);
	return 0;
}

// 显示图书信息
void showBookMeun() {
  printf("*************欢迎进入************\n");
  printf("*************图书馆管理系统******\n");
  printf("*************书籍管理菜单********\n");
  printf("*************1.显示书籍信息******\n");
  printf("*************2.添加书籍**********\n");
  printf("*************3.查找书籍**********\n");
  printf("*************4.删除书籍**********\n");
  printf("*************5.修改书籍**********\n");
  printf("*************0.返回上级**********\n");
  printf("请选择0-5\n");
}

// 新增图书信息
void addBooks(MYSQL* mysql) {
	showBooks(mysql);
	printf("是否要继续添加图书信息(Y/N)\n");
	char f = getchar();
	getchar();
	if (f == 'N') {
		return;
	}
	int iNum;
	char acName[16];
	char acPress[16];
	int iAmout;
	while (f == 'Y') {
		printf("请输入图书编号:");
		scanf("%d", &iNum);
		printf("\n请输入书名:");
		scanf("%s", acName);
		printf("\n请输入图书作者:");
		scanf("%s", acPress);
		printf("\n请输入图书数量:");
		scanf("%d", &iAmout);
		getchar();
		char sql[500] = {""};
		sprintf(sql, "insert into Book values(%d, '%s', '%s', %d)", iNum, acName, acPress, iAmout);
		if (mysql_query(mysql, sql) != 0) {
			printf("insert error%s\n", mysql_error(mysql));
			bzero(sql, sizeof(char) * 500);
			printf("是否继续插入(Y/N)\n");
			scanf("%c", &f);
			continue;
		}
		bzero(sql, sizeof(char) * 500);
		printf("插入成功\n");
		printf("是否继续插入(Y/N)\n");
		scanf("%c", &f);
	}
	return;
}

// 查找图书信息
int searchBooks(MYSQL* mysql) {
	printf("请输入想要查询的图书编号\n");
	int iNum;
	scanf("%d", &iNum);
	getchar();
	char sql[500];

	sprintf(sql, "select * from Book where iNum = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	MYSQL_RES* res = mysql_store_result(mysql);

	int count = mysql_field_count(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该图书信息\n");
		getchar();
		mysql_free_result(res);
		return -1;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "书名", "作者", "数量");
	for (int i = 0; i < count; i++) {
		printf("%-15s", row[i]);
	}
	printf("\n");
	getchar();
	mysql_free_result(res);
	return iNum;
}

// 删除图书信息
void delBooks(MYSQL* mysql) {
	if (showBooks(mysql) == -1) {
		getchar();
		return;
	}
	int iNum = searchBooks(mysql);
	if (iNum == -1) {
		return;
	}
	char sql[500];

	sprintf(sql, "delete from Book where iNum = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("delete error%s", mysql_error(mysql));	
		getchar();
	}
	printf("删除成功\n");
	getchar();
	return;
}

// 修改图书信息
void modifyBooks(MYSQL* mysql) {
	if (showBooks(mysql) == -1) {
		getchar();
		return;
	}
	int iNum = searchBooks(mysql);
	if (iNum == -1) {
		return;
	}
	char sql[500];

	int choose;
	printf("请输入想要修改的数据\n");
	printf("1.iNum\n");
	printf("2.acName\n");
	printf("3.acPress\n");
	printf("4.Amout\n");
	scanf("%d", &choose);
	getchar();

	char temp[15];
	switch (choose)
	{
	case 1:
		strcpy(temp, "iNum");
		break;
	case 2:
		strcpy(temp, "acName");
		break;
	case 3:
		strcpy(temp, "acPress");
		break;
	case 4:
		strcpy(temp, "iAmout");
		break;
	default:
		break;
	}
	printf("请输入要修改后的值");
	char update[16];
	scanf("%s", update);
	getchar();

	sprintf(sql, "update Book set %s = '%s' where iNum = %d", temp, update, iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}
	printf("修改成功\n");
	getchar();
	return;
}

// 图书管理模块
void mangerBook(MYSQL* mysql) {
  int choose = 0;
  system("clear");
  showBookMeun();
  scanf("%d", &choose);
  getchar();
  while (choose) {
    switch (choose) {
    case 0: {
      return;
    }
    case 1: {
      showBooks(mysql);
      getchar();
      break;
    }
    case 2: {
      addBooks(mysql);
      break;
    }
    case 3: {
      searchBooks(mysql);
      break;
    }
    case 4: {
      delBooks(mysql);
      break;
    }
    case 5: {
      modifyBooks(mysql);
      break;
    }
    default: {
      printf("输入错误，请输入正确的数字!\n");
      printf("按任意键返回!\n");
      char tmp;
      scanf("%c", &tmp);
      break;
    }
    }
    system("clear");
    showBookMeun();
    scanf("%d", &choose);
    getchar();
  }
}
// 打印读者模块选项
void showReaderMeun() {
  printf("*************欢迎进入************\n");
  printf("*************图书馆管理系统*******\n");
  printf("*************读者管理菜单*********\n");
  printf("*************1.显示读者信息*******\n");
  printf("*************2.添加读者信息*******\n");
  printf("*************3.查找读者信息*******\n");
  printf("*************4.删除读者信息*******\n");
  printf("*************5.修改读者信息*******\n");
  printf("*************0.返回上级***********\n");
  printf("请选择0-5:\n");
}

// 显示读者信息
int showReaders(MYSQL* mysql) {
	char* sql = "select * from Reader";
	if (mysql_query(mysql, sql)) {
		perror("mysql query error\n");
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	if (res == NULL) {
		return -1;
	}

	int num = mysql_num_rows(res);
	int count = mysql_field_count(mysql);
	if (num == 0) {
		printf("暂无读者信息\n");
		return -1;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "姓名", "最大借阅数", "已借阅图书数量");
	for (int i = 0; i < num; i++) {
		MYSQL_ROW row = mysql_fetch_row(res);
		for (int j = 0; j < count; j++) {
			printf("%-15s", row[j]);
		}
		printf("\n");
		if (strcmp(row[count - 1], "0")) {
			int iNum;
			sscanf(row[0], "%d", &iNum);
			searchBorrowBooks(mysql, iNum);
		}
		printf("\n");
	}
	mysql_free_result(res);
	return 0;
}

// 新增读者信息
void addReaders(MYSQL* mysql) {
	showReaders(mysql);
	printf("是否要继续添加读者信息(Y/N)\n");
	char f = getchar();
	getchar();
	if (f == 'N') {
		return;
	}
	int iNum;
	char acName[16];
	int iMax;
	int numberOfBorrowings = 0;
	while (f == 'Y') {
		printf("请输入读者编号:");
		scanf("%d", &iNum);
		printf("\n请输入读者姓名:");
		scanf("%s", acName);
		printf("\n请输入最大借阅量:");
		scanf("%d", &iMax);
		getchar();
		char sql[500] = {""};
		sprintf(sql, "insert into Reader values(%d, '%s', '%d', %d)", iNum, acName, iMax, numberOfBorrowings);
		if (mysql_query(mysql, sql) != 0) {
			printf("insert error%s\n", mysql_error(mysql));
			bzero(sql, sizeof(char) * 500);
			printf("是否继续插入(Y/N)\n");
			scanf("%c", &f);
			continue;
		}
		bzero(sql, sizeof(char) * 500);
		sprintf(sql, "insert into BorrowBooks values(%d, '%s')", iNum, NULL);
		if (mysql_query(mysql, sql) != 0) {
			printf("insert error%s\n", mysql_error(mysql));
			bzero(sql, sizeof(char) * 500);
			continue;
		}
		printf("插入成功\n");
		printf("是否继续插入(Y/N)\n");
		scanf("%c", &f);
	}
	return;
}

// 查找已借阅图书
int searchBorrowBooks(MYSQL* mysql, int iNum) {
	char sql[500];
	sprintf(sql, "select * from BorrowBooks where id = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	int count = mysql_field_count(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该读者借阅信息\n");
		getchar();
		mysql_free_result(res);
		return -1;
	}

	printf("借阅图书编号: ");
	for (int i = 1; i < count; i++) {
		printf("%-3s", row[i]);
		if (i % 10 == 0) {
			printf("\n");
		}
	}
	printf("\n");
	getchar();
	mysql_free_result(res);
	return iNum;
}

// 查找读者信息
int searchReaders(MYSQL* mysql) {
	printf("请输入想要查询的读者编号\n");
	int iNum;
	scanf("%d", &iNum);
	getchar();
	char sql[500];
	sprintf(sql, "select * from Reader where iNum = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	int count = mysql_field_count(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该读者信息\n");
		getchar();
		mysql_free_result(res);
		return -1;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "姓名", "最大借阅量", "已借阅图书数量");
	for (int i = 0; i < count; i++) {
		printf("%-15s", row[i]);
	}
	printf("\n");
	if (strcmp(row[count - 1], "0")) {
		searchBorrowBooks(mysql, iNum);
	}
	getchar();
	mysql_free_result(res);
	return iNum;
}

// 删除读者信息
void delReaders(MYSQL* mysql) {
	if (showReaders(mysql) == -1) {
		getchar();
		return;
	}
	int iNum = searchReaders(mysql);
	if (iNum == -1) {
		return;
	}
	char sql[500];

	sprintf(sql, "delete from Reader where iNum = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("delete error%s", mysql_error(mysql));	
		getchar();
		return;
	}
	bzero(sql, sizeof(sql));
	sprintf(sql, "delete from BorrowBooks where id = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("delete error%s", mysql_error(mysql));	
		getchar();
		return;
	}
	printf("删除成功\n");
	getchar();
	return;
}

// 修改图书信息
void modifyReaders(MYSQL* mysql) {
	if (showReaders(mysql) == -1) {
		getchar();
		return;
	}
	int iNum = searchReaders(mysql);
	if (iNum == -1) {
		return;
	}
	char sql[500];

	int choose;
	printf("请输入想要修改的数据\n");
	printf("1.iNum\n");
	printf("2.acName\n");
	printf("3.iMax\n");
	scanf("%d", &choose);
	getchar();

	char temp[15];
	switch (choose)
	{
	case 1:
		strcpy(temp, "iNum");
		break;
	case 2:
		strcpy(temp, "acName");
		break;
	case 3:
		strcpy(temp, "iMax");
		break;
	default:
		break;
	}
	printf("请输入要修改后的值");
	char update[16];
	scanf("%s", update);
	getchar();

	sprintf(sql, "update Reader set %s = '%s' where iNum = %d", temp, update, iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}
	printf("修改成功\n");
	getchar();
	return;
}

// 读者模块
void mangerReader(MYSQL* mysql) {
  int choose = 0;
  system("clear");
  showReaderMeun(mysql);
  scanf("%d", &choose);
  getchar();
  while (choose) {
    switch (choose) {
    case 0: {
      return;
    }
    case 1: {
      showReaders(mysql);
      getchar();
      break;
    }
    case 2: {
      addReaders(mysql);
      break;
    }
    case 3: {
      searchReaders(mysql);
      break;
    }
    case 4: {
      delReaders(mysql);
      break;
    }
    case 5: {
      modifyReaders(mysql);
      break;
    }
    default: {
      printf("输入错误，请输入正确的数字!\n");
      printf("按任意键返回!\n");
      char tmp;
      scanf("%c", &tmp);
      break;
    }
    }
    system("clear");
    showReaderMeun();
    scanf("%d", &choose);
    getchar();
  }
}

// 打印借还书菜单
void showBRMeun() {
	system("clear");
  printf("*************欢迎进入************\n");
  printf("*************图书馆管理系统******\n");
  printf("*************借还管理菜单********\n");
  printf("*************1.租界图书**********\n");
  printf("*************2.归还图书**********\n");
  printf("*************0.返回上级**********\n");
  printf("请选择0-2\n");
}

// 借书
void borrowBooks(MYSQL* mysql) {
	// 读者信息获取
	showReaders(mysql);
	int iMax;
	int borrowNum;
	printf("请输入想要借书的读者编号\n");
	int iNum;
	scanf("%d", &iNum);
	getchar();
	char sql[500];
	sprintf(sql, "select * from Reader where iNum = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	int count = mysql_field_count(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该读者信息\n");
		getchar();
		mysql_free_result(res);
		return;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "姓名", "最大借阅量", "已借阅图书数量");
	for (int i = 0; i < count; i++) {
		printf("%-15s", row[i]);
		if (i == count - 1) {
			sscanf(row[i], "%d", &borrowNum);
		}
		if (i == count - 2) {
			sscanf(row[i], "%d", &iMax);
		}
	}
	printf("\n");
	if (strcmp(row[count - 1], "0")) {
		searchBorrowBooks(mysql, iNum);
	}
	if (borrowNum >= iMax) {
		printf("已借阅书籍数量大于最大可借阅数量\n");
		getchar();
		return;
	}
	bzero(sql, sizeof(sql));
	printf("\n");

	// 图书信息获取
	showBooks(mysql);
	printf("请输入想要借阅的图书编号\n");
	int iNum2;
	int bookNum;
	scanf("%d", &iNum2);
	getchar();

	sprintf(sql, "select * from Book where iNum = %d", iNum2);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	res = mysql_store_result(mysql);

	count = mysql_field_count(mysql);
	row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该图书信息\n");
		getchar();
		mysql_free_result(res);
		return;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "书名", "作者", "数量");
	for (int i = 0; i < count; i++) {
		printf("%-15s", row[i]);
		if (i == count - 1) {
			sscanf(row[i], "%d", &bookNum);
		}
	}
	printf("\n");
	if (bookNum == 0) {
		printf("当前图书已借空\n");
	}

	// 修改读者信息
	bzero(sql, sizeof(sql));
	sprintf(sql, "update Reader set BookId = '%d' where iNum = %d", borrowNum + 1, iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}

	// 修改图书信息
	bzero(sql, sizeof(sql));
	sprintf(sql, "update Book set iAmout = '%d' where iNum = %d", bookNum - 1, iNum2);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}

	// 修改借阅信息
	bzero(sql, sizeof(sql));
	sprintf(sql, "select * from BorrowBooks where id = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}
	char books[129] = {0};
	res = mysql_store_result(mysql);
	count = mysql_field_count(mysql);
	row = mysql_fetch_row(res);
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			sscanf(row[i], "%s", books);
		}
	}
	char bookss[128] = {0};
	if (strcmp("(null)", books) != 0) {
		sprintf(bookss, "%s,%d", books, iNum2);
	}
	else {
		sprintf(bookss, "%d", iNum2);
	}


	bzero(sql, sizeof(sql));
	sprintf(sql, "update BorrowBooks set bookIds = '%s' where id = %d", bookss, iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}

	getchar();
	mysql_free_result(res);
	return;
}

// 还书
void returnBooks(MYSQL* mysql) {
	// 读者信息获取
	showReaders(mysql);
	int iMax;
	int borrowNum;
	printf("请输入想要还书的读者编号\n");
	int iNum;
	scanf("%d", &iNum);
	getchar();
	char sql[500];
	sprintf(sql, "select * from Reader where iNum = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	int count = mysql_field_count(mysql);
	MYSQL_ROW row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该读者信息\n");
		getchar();
		mysql_free_result(res);
		return;
	}
	printf("%-17s%-17s%-17s%-17s\n", "编号", "姓名", "最大借阅量", "已借阅图书数量");
	for (int i = 0; i < count; i++) {
		printf("%-15s", row[i]);
		if (i == count - 1) {
			sscanf(row[i], "%d", &borrowNum);
		}
		if (i == count - 2) {
			sscanf(row[i], "%d", &iMax);
		}
	}
	printf("\n");
	if (strcmp(row[count - 1], "0")) {
		searchBorrowBooks(mysql, iNum);
	}
	if (borrowNum <= 0) {
		printf("暂无可归还图书\n");
		getchar();
		return;
	}
	bzero(sql, sizeof(sql));
	printf("\n");



	// 图书信息获取
	showBooks(mysql);
	printf("请输入想要归还的图书编号\n");
	int iNum2;
	int bookNum;
	scanf("%d", &iNum2);

	sprintf(sql, "select * from Book where iNum = %d", iNum2);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
	}

	res = mysql_store_result(mysql);
	count = mysql_field_count(mysql);
	row = mysql_fetch_row(res);
	if (row == NULL) {
		printf("暂无该图书信息\n");
		getchar();
		mysql_free_result(res);
		return;
	}
	for (int i = 0; i < count; i++) {
		if (i == count - 1) {
			sscanf(row[i], "%d", &bookNum);
		}
	}
	printf("\n");
	getchar();


	// 修改借阅信息
	bzero(sql, sizeof(sql));
	sprintf(sql, "select * from BorrowBooks where id = %d", iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}
	char books[129] = {0};
	res = mysql_store_result(mysql);
	count = mysql_field_count(mysql);
	row = mysql_fetch_row(res);
	for (int i = 0; i < count; i++) {
		if (i > 0) {
			sscanf(row[i], "%s", books);
		}
	}
	// 获取已借阅列表
	char* borBooks[borrowNum];
	for (int i = 0; i < borrowNum; i++) {
		borBooks[i] = (char*)malloc(sizeof(char*) * 8);
	}

	char* temp = strtok(books, ",");
	strcpy(borBooks[0], temp);
	for (int i = 1; i < borrowNum && temp != NULL; i++) {
		temp = strtok(NULL, ",");
		strcpy(borBooks[i], temp);
	}

	int id;
	int falg = 0;
	char* outPut = (char*)malloc(sizeof(char) * 128);
	bzero(outPut, sizeof(char) * 128);
	for (int i = 0; i < borrowNum; i++) {
		sscanf(borBooks[i], "%d", &id);
		if (id == iNum2 && falg == 0) {
			falg = 1;
			continue;
		}
		if (strncmp(outPut, "0", 1) && falg == 1) {
			strcpy(outPut, borBooks[i]);
			falg = 2;
			continue;
		}
		sprintf(outPut, "%s,%s", outPut, borBooks[i]);
	}
	bzero(sql, sizeof(sql));
	sprintf(sql, "update BorrowBooks set bookIds = '%s' where id = %d", outPut, iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}

	// 修改读者信息
	bzero(sql, sizeof(sql));
	sprintf(sql, "update Reader set BookId = '%d' where iNum = %d", borrowNum - 1, iNum);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}

	// 修改图书信息
	bzero(sql, sizeof(sql));
	sprintf(sql, "update Book set iAmout = '%d' where iNum = %d", bookNum + 1, iNum2);
	if (mysql_query(mysql, sql) != 0) {
		printf("search error%s", mysql_error(mysql));	
		getchar();
		return;
	}

	getchar();
	mysql_free_result(res);
	return;
}


// 借还书模块
void borrowReturnManger(MYSQL* mysql) {
  int choose = 0;
  system("clear");
  showBRMeun();
  scanf("%d", &choose);
  getchar();
  while (choose) {
    switch (choose) {
    case 0: {
      return;
    }
    case 1: {
      borrowBooks(mysql);
      break;
    }
    case 2: {
      returnBooks(mysql);
      break;
    }
    default: {
      printf("输入错误，请输入正确的数字!\n");
      printf("按任意键返回!\n");
      char tmp;
      scanf("%c", &tmp);
			getchar();
      break;
    }
    }
    system("clear");
    showBRMeun();
    scanf("%d", &choose);
    getchar();
  }
}