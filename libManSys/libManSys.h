#include "../sql/sql.h"
struct book;
struct reader;



// 显示主菜单信息
void showMainMeun();
// 显示图书菜单
void showBookMeun();
// 显示图书信息
void showBooks(MYSQL* mysql);
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