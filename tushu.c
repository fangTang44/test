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
    rfd = open("../target/book.txt", O_APPEND);
  }
  if (strcmp(pcMode, "rb")) {
    rfd = open("../target/book.txt", O_RDONLY);
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
  close(rfd);
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
  int rfd = open("../target/book.txt", O_WRONLY | O_APPEND);
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
    int rfd = open("../target/book.txt", O_TRUNC | O_WRONLY);
    assert(rfd != -1);
    for (int i = 0; i < iBookRecord; i++) {
      write(rfd, &astBook[i], sizeof(struct book));
    }
    printf("删除成功，请按任意键继续\n");
  }
}

// 保存文件到book
void saveBookFile(int iBookid, int iBookRecord) {
  int rfd = open("../target/book.txt", O_TRUNC | O_WRONLY);
  assert(rfd != -1);
  for (int i = 0; i < iBookRecord; i++) {
    write(rfd, &astBook[i], sizeof(struct book));
  }
	close (rfd);
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

// 从文件中读取读者信息
int readReadersFile(const char *pcMode) {
  // 文件中的记录条数
  int rfd = -1;
  int iReaderRecord = 0;
  if (strcmp(pcMode, "ab+")) {
    rfd = open("../target/reader.txt", O_APPEND);
  }
  if (strcmp(pcMode, "rb")) {
    rfd = open("../target/reader.txt", O_RDONLY);
  }
  if (rfd == -1) {
    return -1;
  }
  int num = 0;
  do {
    num = read(rfd, &astReader[iReaderRecord], sizeof(struct reader));
    if (num != 0) {
      iReaderRecord++;
    }
  } while (num != 0);
  close(rfd);
  return iReaderRecord;
}

// 显示读者信息
void showReaders() {
  int i, iReaderRecord = -1;
  system("clear");
  iReaderRecord = readReadersFile("rb");
  if (iReaderRecord == -1) {
    printf("打开失败请先添加读者信息\n");
    return;
  }
  if (iReaderRecord == 0) {
    printf("文件中暂无读者信息\n");
    return;
  } else {
    printf("************************************************\n");
    printf("%-6s%-16s%-10s%-20s%-4s\n", "编号", "姓名", "性别", "可借阅最大数量",
           "以借阅数量");
    for (int i = 0; i < iReaderRecord; i++) {
      printf("%-6d%-16s%-10s%-20d%-4d\n", READER_DATE);
      printf("以借阅图书编号列表\n");
      for (int j = 0; j < astReader[i]._iAmout; j++) {
        printf("%d ", astReader[i]._bllkLd[j]);
      }
      printf("\n");
    }
    
    printf("************************************************\n");
  }
  return;
}

// 新增读者信息
void addReaders() {
  //  记录条数         是否已经存在    下标
  int iReaderRecord = 0, iFlageExit = 0, i = 0;
  // 是否添加读者信息
  char flage;
  system("clear");
  iReaderRecord = readReadersFile("ab+");
  if (iReaderRecord == -1) {
    printf("文件打开失败\n");
  } else if (iReaderRecord == 0) {
    printf("没有读者信息记录\n");
  } else {
    showReaders();
  }
  // 循环录入读者信息
  printf("请选择是否输入信息(y/n)\n");
  flage = getchar();
  getchar();
  if (flage == 'n') {
    return;
  }
  int rfd = open("../target/reader.txt", O_WRONLY | O_APPEND);
  while (flage == 'y') {
    if (iReaderRecord >= BOOKNUMBER) {
      printf("记录已满\n");
      return;
    }
    // 录入数据
    printf("录入数据,请分别输入:\n");
    printf("%s %s %s %s %s\n", "编号", "姓名", "性别", "可借阅最大数量", "已借阅数量");
    struct reader tmp;
    scanf("%d%s%s%d%d", &(tmp._id), tmp._name, tmp._sex, &tmp._iMax,
          &(tmp._iAmout));
    getchar();
    printf("请输入以借阅的%d本图书编号:\n", tmp._iAmout);
    for (int i = 0; i < tmp._iAmout; i++) {
      printf("请输入第%d本:\n", i + 1);
      scanf("%d", &tmp._bllkLd[i]);
      getchar();
    }

    for (int i = 0; i < iReaderRecord; i++) {
      if (tmp._id == astReader[i]._id) {
        iFlageExit = 1;
        printf("该读者编号已经存在\n");
        break;
      }
    }
    if (iFlageExit == 0) {
      astReader[iReaderRecord++] = tmp;
      int num = write(rfd, &tmp, sizeof(struct reader));
      printf("num = %d, ib = %d\n", num, iReaderRecord);
      printf("添加成功\n");
    }
    // 保存信息
    printf("是否要继续输入信息(y/n)\n");
    flage = getchar();
    getchar();
  }
  close(rfd);
}

// 查找读者信息
int searchReaders() {
  system("clear");
  int iReaderRecord = readReadersFile("rb");
  int i = -1;
  if (iReaderRecord == -1) {
    printf("文件打开失败\n");
    getchar();
    return -1;
  }
  if (iReaderRecord == 0) {
    printf("暂无读者信息\n");
    getchar();
    return -2;
  } else {
    printf("请输入编号\n");
    int iReaderNum = -1;
    scanf("%d", &iReaderNum);
    getchar();
    for (i = 0; i < iReaderRecord; i++) {
      if (iReaderNum == astReader[i]._id) {
        iReaderNum = i;
        printf("************************************************\n");
        printf("%-6s%-16s%-10s%-20s%-4s\n", "编号", "姓名", "性别", "可借阅最大数量",
               "已借阅数量");
        printf("%-6d%-16s%-10s%-20d%-4d\n", READER_DATE);
        printf("以借阅图书编号列表\n");
      for (int j = 0; j < astReader[i]._iAmout; j++) {
        printf("%d ", astReader[i]._bllkLd[j]);
      }
      printf("\n");
        printf("************************************************\n");
        getchar();
        return iReaderNum;
      }
    }
  }
  printf("未找到该读者\n");
  getchar();
  return -3;
}

// 删除读者信息
void delReaders() {
  int iReaderid, iReaderRecord, i;
  char cFlag;
  system("clear");
  iReaderid = searchReaders();
  if (iReaderid == -1) {
    printf("打开文件失败\n");
    return;
  }
  if (iReaderid == -2) {
    printf("暂无读者信息\n");
    return;
  }
  if (iReaderid == -3) {
    printf("未找到图书\n");
    return;
  }
  iReaderRecord = readReadersFile("rb");
  printf("已经找到该读者是否删除(y/n)\n");
  cFlag = getchar();

  if (cFlag == 'n') {
    return;
  }
  if (cFlag == 'y') {
    astReader[iReaderid] = astReader[--iReaderRecord];
    int rfd = open("../target/reader.txt", O_TRUNC | O_WRONLY);
    assert(rfd != -1);
    for (int i = 0; i < iReaderRecord; i++) {
      write(rfd, &astReader[i], sizeof(struct reader));
    }
    printf("删除成功，请按任意键继续\n");
  }
}

// 保存文件到book
void saveReaderFile(int iReaderid, int iReaderRecord) {
  int rfd = open("../target/reader.txt", O_TRUNC | O_WRONLY);
  assert(rfd != -1);
  for (int i = 0; i < iReaderRecord; i++) {
    write(rfd, &astReader[i], sizeof(struct reader));
  }
	close (rfd);
}

// 修改图书信息
void modifyReaders() {
  system("clear");
  int iReaderid = searchReaders();
  if (iReaderid == -3) {
    printf("为找到读者信息\n");
    return;
  }
  if (iReaderid == -2) {
    printf("暂无读者信息\n");
    return;
  }
  if (iReaderid == -1) {
    printf("文件打开失败\n");
    return;
  }

  printf("请输入想要修改的内容:\n");
  printf("1.编号\n");
  printf("2.姓名\n");
  printf("3.性别\n");
  printf("4.可借阅最大数量\n");
  printf("5.已借阅数量及编号\n");


  int choose = 0;
  int readerNum = readReadersFile("rb");
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
      for (int i = 0; i < readerNum; i++) {
        if (astReader[i]._id == id) {
          printf("编号已经存在请重新输入\n");
          iFlageExit = 1;
          break;
        }
      }
    } while (iFlageExit == 1);
    astReader[iReaderid]._id = id;
    saveReaderFile(iReaderid, readerNum);
    break;
  }
  case 2: {
    scanf("%s", astReader[iReaderid]._name);
    getchar();
    saveReaderFile(iReaderid, readerNum);
    break;
  }
  case 3: {
    scanf("%s", astReader[iReaderid]._sex);
    getchar();
    saveReaderFile(iReaderid, readerNum);
    break;
  }
  case 4: {
    scanf("%d", &astReader[iReaderid]._iMax);
    getchar();
    saveReaderFile(iReaderid, readerNum);
    break;
  }
  case 5: {
    int number;
    scanf("%d", &number);
    getchar();
    astReader[iReaderid]._iAmout = number;
    for (int i = 0; i < number; i++) {
      printf("请输入第%d本书的编号：", i + 1);
      scanf("%d", &astReader[iReaderid]._bllkLd[i]);
      getchar();
    }
    saveReaderFile(iReaderid, readerNum);
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


void borrowBooks() {
	int readerNum = readReadersFile("rb");
	if (readerNum == 0) {
		printf("暂无读者数据请添加后重试\n");
		return;
	}
	int bookNum = readBookFile("rb");
	if (bookNum == 0) {
		printf("暂无图书信息请添加后重试\n");
		return;
	}

	int readerIndex = -1;
	do {
		printf("请输入读者编号\n");
		int readerId;  scanf("%d", &readerId);
		getchar();
		readerIndex = -1;

	for (int i = 0; i < readerNum; i++) {
		if (astReader[i]._id == readerId) {
			readerIndex = i;
			break;
		}
	}
	if (readerIndex == -1) {
		printf("未找到编号为%d的读者信息\n", readerId);
		getchar();
	}
	} while(readerIndex == -1);

	if (astReader[readerIndex]._iAmout >= astReader[readerIndex]._iMax) {
		printf("已借阅图书数量大于可借阅最大数量，请增加可借阅数量或先进行还书\n");
		getchar();
		return;
	}

	int bookIndex = -1;
	do {
		printf("请输入想要借阅的图书的编号\n");
		int bookId;  scanf("%d", &bookId);
		getchar();
		bookIndex = -1;
		for (int i = 0; i < bookNum; i++) {
			if (bookId == astBook[i]._id) {
				bookIndex = i;
				break;
			}
		}
		if (bookIndex == -1) {
			printf("未找到该图书\n");
			getchar();
		}
	} while(bookIndex == -1);
	if (astBook[bookIndex]._number == 0) {
		printf("该图书已借空\n");
		getchar();
		return;
	}
	astReader[readerIndex]._bllkLd[astReader[readerIndex]._iAmout++] = astBook[bookIndex]._id;
	astBook[bookIndex]._number--;
	saveBookFile(0, bookNum);
	saveReaderFile(0, readerNum);
	printf("已成功借阅\n");
	getchar();
}

void returnBooks() {
	int readerNum = readReadersFile("rb");
	if (readerNum == 0) {
		printf("暂无读者数据请添加后重试\n");
		return;
	}
	int bookNum = readBookFile("rb");
	if (bookNum == 0) {
		printf("暂无图书信息请添加后重试\n");
		return;
	}

	int readerIndex = -1;
	do {
		printf("请输入读者编号\n");
		int readerId;  scanf("%d", &readerId);
		getchar();
		readerIndex = -1;

	for (int i = 0; i < readerNum; i++) {
		if (astReader[i]._id == readerId) {
			readerIndex = i;
			break;
		}
	}
	if (readerIndex == -1) {
		printf("未找到编号为%d的读者信息\n", readerId);
		getchar();
	}
	} while(readerIndex == -1);

	if (astReader[readerIndex]._iAmout == 0) {
		printf("没有该用户的借书记录\n");
		getchar();
		return;
	}

	printf("该用户借阅列表:\n");
	for (int i = 0; i < astReader[readerIndex]._iAmout; i++) {
		printf("%d ", astReader[readerIndex]._bllkLd[i]);
	}
	printf("\n");

	int bookIndexInReader = -1;
	int bookIndexInBook = -1;
	int bookId;
	do {
		printf("请输入想要归还的图书的编号\n");
		scanf("%d", &bookId);
		getchar();
		bookIndexInReader = -1;
		for (int i = 0; i < astReader[i]._iAmout; i++) {
			if (bookId == astReader[i]._bllkLd[i]) {
				bookIndexInReader = i;
				break;
			}
		}
		if (bookIndexInReader == -1) {
			printf("该用户未借阅该图书\n");
			getchar();
		}
	} while(bookIndexInReader == -1);

	for (int i = 0; i < bookNum; i++) {
		if (bookId == astBook[i]._id) {
			bookIndexInBook = i;
			break;
		}
	}
	if (bookIndexInBook == -1) {
		perror("error\n");
	}
	astBook[bookIndexInBook]._number++;
	astReader[readerIndex]._bllkLd[bookIndexInReader] = 
										astReader[readerIndex]._bllkLd[--astReader[readerIndex]._iAmout];
	saveBookFile(0, bookNum);
	saveReaderFile(0, readerNum);
	printf("已成功归还\n");
	getchar();
}

// 借还书模块
void borrowReturnManger() {
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
      borrowBooks();
      break;
    }
    case 2: {
      returnBooks();
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

int main() {
  int rfd = open("../target/book.txt", O_CREAT, 0666);
  close(rfd);
  int rfd2 = open("../target/reader.txt", O_CREAT, 0666);
  close(rfd2);
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
      borrowReturnManger();
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
