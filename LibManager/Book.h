#pragma once

#include"include.h"

#define BOOKLIST_FILE "bookList.dat"

#define ISBN_MAX 13
#define BOOK_NAME_MAX 100
#define BOOK_PUBLISHED_MAX 4
#define BOOK_PUBLISHER_MAX 50
#define BOOK_TYPE_MAX 50

#define MAX_BORROW 5

struct Book {
    char ISBN[ISBN_MAX];
    char book_Name[BOOK_NAME_MAX];
    char book_Author[NAME_MAX];
    char book_published_year[BOOK_PUBLISHED_MAX];
    char book_publisher[BOOK_PUBLISHER_MAX];
	char book_type[BOOK_TYPE_MAX];
    int price;
    int count;
};

struct borrowBook {
    int maDocGia;
    // Them ngay muon
    // Them ngay tra (du kien)
    // Them ngay tra (thuc te)
    Book listISBN[MAX_BORROW];
    bool returned;
};

struct bookNode
{
	Book bookInfo;
	bookNode* nextBook;
	bookNode* prevBook;
};

struct bookList
{
	bookNode* firstBook;
	bookNode* lastBook;
	int bookCount;
};



bookNode* creatBookNode(Book newBook);
bookNode* isBookExist(char* ISBN, bookList list);
void enterBookInfo(bookList& a);
void add_book_to_list(bookList& a, bookNode* newbook);
void deleteOneBook(bookList& a);
void delete_ALL_book(bookList& a);
void cout_Book_List(bookList a);
void getBookList(bookList& a);
void write_bookList_to_DAT(bookList list);