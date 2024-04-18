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

// 定义两个全局结构体数组来记录所有的图书和读者信息
struct book astBook[BOOKNUMBER];
struct reader astReader[READERNUMBER];

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
void showBooks(MYSQL* mysql) {
	printf("%-17s%-17s%-17s%-17s\n", "编号", "书名", "作者", "数量");
	char* sql = "select * from Book";
	if (mysql_query(mysql, sql)) {
		perror("mysql query error\n");
	}

	MYSQL_RES* res = mysql_store_result(mysql);
	if (res == NULL) {
		printf("暂无图书信息\n");
		return;
	}

	int num = mysql_num_rows(res);
	int count = mysql_field_count(mysql);
	for (int i = 0; i < num; i++) {
		MYSQL_ROW row = mysql_fetch_row(res);
		for (int j = 0; j < count; j++) {
			printf("%-15s", row[j]);
		}
		printf("\n");
	}
	mysql_free_result(res);
	return;
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
	showBooks(mysql);
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
	showBooks(mysql);
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