/*************************************************************************
> File Name:     tushu.c
> Author:        fangTang
> mail:          18691171371@163.com
> Created Time:  一  3/ 4 19:12:12 2024
> Description:
************************************************************************/

#include <assert.h>
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
#define BOOK_DATE                                                              \
  astBook[i]._id, astBook[i]._name, astBook[i]._author, astBook[i]._press,     \
      astBook[i]._number

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

int readBookFile(const char *pcMode) {
  // 文件中的记录条数
  int rfd = -1;
  int iBookRecord = 0;
  if (strcmp(pcMode, "ab+")) {
    rfd = open("book.txt", O_APPEND);
  }
  if (strcmp(pcMode, "rb")) {
    rfd = open("book.txt", O_RDONLY);
  }
  if (rfd == -1) {
    return -1;
  }
  int num = 0;
  do {
    num = read(rfd, &astBook[iBookRecord], sizeof(struct book));
    if (num != 0) {
      iBookRecord++;
    }
  } while (num != 0);
  return iBookRecord;
}

// 显示图书信息
void showBooks() {
  int i, iBookRecord = -1;
  system("clear");
  iBookRecord = readBookFile("rb");
  if (iBookRecord == -1) {
    printf("打开失败请先添加图书信息\n");
    return;
  }
  if (iBookRecord == 0) {
    printf("文件中暂无图书信息\n");
    return;
  } else {
    printf("************************************************\n");
    printf("%-6s%-16s%-10s%-20s%-4s\n", "编号", "书名", "作者", "出版社",
           "库存量");
    for (int i = 0; i < iBookRecord; i++) {
      printf("%-6d%-16s%-10s%-20s%-4d\n", BOOK_DATE);
    }
    printf("************************************************\n");
  }
  return;
}

// 新增图书信息
void addBooks() {
  //  记录条数         是否已经存在    下标
  int iBookRecord = 0, iFlageExit = 0, i = 0;
  // 是否添加图书
  char flage;
  system("clear");
  iBookRecord = readBookFile("ab+");
  if (iBookRecord == -1) {
    printf("文件打开失败\n");
  } else if (iBookRecord == 0) {
    printf("没有图书记录\n");
  } else {
    showBooks();
  }
  // 循环录入图书信息
  printf("请选择是否输入信息(y/n)\n");
  flage = getchar();
  getchar();
  if (flage == 'n') {
    return;
  }
  int rfd = open("book.txt", O_WRONLY | O_APPEND);
  while (flage == 'y') {
    if (iBookRecord >= BOOKNUMBER) {
      printf("记录已满\n");
      return;
    }
    // 录入数据
    printf("录入数据,请分别输入:\n");
    printf("%s %s %s %s %s\n", "编号", "书名", "作者", "出版社", "库存量");
    struct book tmp;
    scanf("%d%s%s%s%d", &(tmp._id), tmp._name, tmp._author, tmp._press,
          &(tmp._number));
    getchar();
    for (int i = 0; i < iBookRecord; i++) {
      if (tmp._id == astBook[i]._id) {
        iFlageExit = 1;
        printf("该图书编号已经存在\n");
        break;
      }
    }
    if (iFlageExit == 0) {
      astBook[iBookRecord++] = tmp;
      int num = write(rfd, &tmp, sizeof(struct book));
      printf("num = %d, ib = %d\n", num, iBookRecord);
      printf("添加成功\n");
    }
    // 保存信息
    printf("是否要继续输入信息(y/n)\n");
    flage = getchar();
    getchar();
  }
  close(rfd);
}

int searchBooks() {
  system("clear");
  int iBookRecord = readBookFile("rb");
  int i = -1;
  if (iBookRecord == -1) {
    return -1;
  }
  if (iBookRecord == 0) {
    return -2;
  } else {
    printf("请输入编号\n");
    int iBookNum = -1;
    scanf("%d", &iBookNum);
    getchar();
    for (i = 0; i < iBookRecord; i++) {
      if (iBookNum == astBook[i]._id) {
        iBookNum = i;
        printf("************************************************\n");
        printf("%-6s%-16s%-10s%-20s%-4s\n", "编号", "书名", "作者", "出版社",
               "库存量");
        printf("%-6d%-16s%-10s%-20s%-4d\n", BOOK_DATE);
        printf("************************************************\n");
        getchar();
        return iBookNum;
      }
    }
  }
  printf("未找到记录\n");
  getchar();
  return -3;
}

// 删除图书信息
void delBooks() {
  int iBookid, iBookRecord, i;
  char cFlag;
  system("clear");
  iBookid = searchBooks();
  if (iBookid == -1) {
    printf("打开文件失败\n");
    return;
  }
  if (iBookid == -2) {
    printf("暂无图书信息\n");
    return;
  }
  if (iBookid == -3) {
    printf("未找到图书\n");
    return;
  }
  iBookRecord = readBookFile("rb");
  printf("已经找到该图书是否删除(y/n)\n");
  cFlag = getchar();

  if (cFlag == 'n') {
    return;
  }
  if (cFlag == 'y') {
    astBook[iBookid] = astBook[--iBookRecord];
    int rfd = open("./book.txt", O_TRUNC | O_WRONLY);
    assert(rfd != -1);
    for (int i = 0; i < iBookRecord; i++) {
      write(rfd, &astBook[i], sizeof(struct book));
    }
    printf("删除成功，请按任意键继续\n");
  }
}

void saveBookFile(int iBookid, int iBookRecord) {
  int rfd = open("./book.txt", O_TRUNC | O_WRONLY);
  assert(rfd != -1);
  for (int i = 0; i < iBookRecord; i++) {
    write(rfd, &astBook[i], sizeof(struct book));
  }
}

// 修改图书信息
void modifyBooks() {
  system("clear");
  int iBookid = searchBooks();
  if (iBookid == -3) {
    printf("为找到图书信息\n");
    return;
  }
  if (iBookid == -2) {
    printf("暂无图书信息\n");
    return;
  }
  if (iBookid == -1) {
    printf("文件打开失败\n");
    return;
  }

  printf("请输入想要修改的内容:\n");
  printf("1.编号\n");
  printf("2.书名\n");
  printf("3.作者\n");
  printf("4.出版社\n");
  printf("5.库存量\n");

  int choose = 0;
  int bookNum = readBookFile("rb");
  scanf("%d", &choose);
  printf("请输入修改后的内容:\n");
  switch (choose) {
  case 1: {
    int id;
    int iFlageExit;
    do {
      iFlageExit = 0;
      scanf("%d", &id);
      getchar();
      for (int i = 0; i < bookNum; i++) {
        if (astBook[i]._id == id) {
          printf("编号已经存在请重新输入\n");
          iFlageExit = 1;
          break;
        }
      }
    } while (iFlageExit == 1);
    astBook[iBookid]._id = id;
    saveBookFile(iBookid, bookNum);
    break;
  }
  case 2: {
    scanf("%s", astBook[iBookid]._name);
    getchar();
    saveBookFile(iBookid, bookNum);
    break;
  }
  case 3: {
    scanf("%s", astBook[iBookid]._author);
    getchar();
    saveBookFile(iBookid, bookNum);
    break;
  }
  case 4: {
    scanf("%s", astBook[iBookid]._press);
    getchar();
    saveBookFile(iBookid, bookNum);
    break;
  }
  case 5: {
    int number;
    scanf("%d", &number);
    getchar();
    astBook[iBookid]._number = number;
    saveBookFile(iBookid, bookNum);
    break;
  }
  default: {
    printf("输入错误,请按任意键继续\n");
    getchar();
    return;
  }
  }
  printf("求改成功，请按任意键继续\n");
  getchar();
}

// 图书管理模块
void mangerBook() {
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
      printf("%d\n", choose);
      showBooks();
      getchar();
      break;
    }
    case 2: {
      addBooks();
      break;
    }
    case 3: {
      searchBooks();
      break;
    }
    case 4: {
      delBooks();
      break;
    }
    case 5: {
      modifyBooks();
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
  printf("*************图书馆管理系统******\n");
  printf("*************读者管理菜单********\n");
  printf("*************1.显示读者信息******\n");
  printf("*************2.添加读者信息**********\n");
  printf("*************3.查找读者信息**********\n");
  printf("*************4.删除读者信息**********\n");
  printf("*************5.修改读者信息**********\n");
  printf("*************0.返回上级**********\n");
  printf("请选择0-5\n");
}

// 从文件中读取读者信息
int readReadersFile(const char *pcMode) {
  // 文件中的记录条数
  int rfd = -1;
  int iBookRecord = 0;
  if (strcmp(pcMode, "ab+")) {
    rfd = open("reader.txt", O_APPEND);
  }
  if (strcmp(pcMode, "rb")) {
    rfd = open("reader.txt", O_RDONLY);
  }
  if (rfd == -1) {
    return -1;
  }
  int num = 0;
  do {
    num = read(rfd, &astBook[iBookRecord], sizeof(struct book));
    if (num != 0) {
      iBookRecord++;
    }
  } while (num != 0);
  return iBookRecord;
}

// 显示读者信息
void showReaders() {
  int i, iBookRecord = -1;
  system("clear");
  iBookRecord = readBookFile("rb");
  if (iBookRecord == -1) {
    printf("打开失败请先添加图书信息\n");
    return;
  }
  if (iBookRecord == 0) {
    printf("文件中暂无图书信息\n");
    return;
  } else {
    printf("************************************************\n");
    printf("%-6s%-16s%-10s%-20s%-4s\n", "编号", "书名", "作者", "出版社",
           "库存量");
    for (int i = 0; i < iBookRecord; i++) {
      printf("%-6d%-16s%-10s%-20s%-4d\n", BOOK_DATE);
    }
    printf("************************************************\n");
  }
  return;
}

// 读者模块
void mangerReader() {
  int choose = 0;
  system("clear");
  showReaderMeun();
  scanf("%d", &choose);
  getchar();
  while (choose) {
    switch (choose) {
    case 0: {
      return;
    }
    case 1: {
      printf("%d\n", choose);
      showReaders();
      getchar();
      break;
    }
    case 2: {
      addReaders();
      break;
    }
    case 3: {
      searchReaders();
      break;
    }
    case 4: {
      delReaders();
      break;
    }
    case 5: {
      modifyReaders();
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

int main() {
  int rfd = open("book.txt", O_CREAT, 0666);
  close(rfd);
  showMainMeun();
  int choose = 0;
  scanf("%d", &choose);
  while (choose) {
    switch (choose) {
    case 1: {
      mangerBook();
      break;
    }
    case 2: {
      mangerReader();
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
