#include "../sql/sql.h"
struct book;
struct reader;



// 显示主菜单信息
void showMainMeun();
// 显示图书菜单
void showBookMeun();
// 显示图书信息
int showBooks(MYSQL* mysql);
// 新增图书信息
void addBooks(MYSQL* mysql);
// 查找图书信息
int searchBooks(MYSQL* mysql);
// 删除图书信息
void delBooks(MYSQL* mysql);
// 修改图书信息
void modifyBooks(MYSQL* mysql);
// 图书管理模块
void mangerBook(MYSQL* mysql);

// 打印读者菜单
void showReaderMeun();
// 读者模块
void mangerReader(MYSQL* mysql);
// 显示读者信息
int showReaders(MYSQL* mysql);
// 新增读者信息
void addReaders(MYSQL* mysql);
// 查找已借阅图书
int searchBorrowBooks(MYSQL* mysql, int iNum);
// 查找读者信息
int searchReaders(MYSQL* mysql);
// 删除读者信息
void delReaders(MYSQL* mysql);
// 修改图书信息
void modifyReaders(MYSQL* mysql);

// 打印借还书菜单
void showBRMeun();
// 借还书模块
void borrowReturnManger(MYSQL* mysql);
// 借书
void borrowBooks(MYSQL* mysql);
// 还书
void returnBooks(MYSQL* mysql);