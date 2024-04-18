#include "../libManSys/libManSys.h"
#include "../sql/sql.h"

int main() {
	MYSQL mysql_conn;
	MYSQL* mysql = MySqlCreate(&mysql_conn, "127.0.0.1", "root", "LHw18691171371..", "db_book");
	if (mysql == NULL) {

	}
	printf("mysql create ok\n");

  showMainMeun();
  int choose = 0;
  scanf("%d", &choose);
  while (choose) {
    switch (choose) {
    case 1: {
      mangerBook(mysql);
      break;
    }
    case 2: {
      // mangerReader();
      break;
    }
    case 3: {
      // borrowReturnManger();
      break;
    }
    default: {
      printf("\t\t\t\t输入错误，请输入正确的数字!\n");
      printf("\t\t\t\t程序将在三秒后返回主菜单!\n");
      sleep(3);
      break;
    }
    }
    showMainMeun();
    scanf("%d", &choose);
    getchar();
  }

  exit(0);
}
