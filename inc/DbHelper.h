#ifndef DBHELPER_H_
#define DBHELPER_H_

#include <sqlite3.h>
#include "Word.h"

const std::string dbFileName("wordlist_170304.db");

class DbHelper {
private:
	sqlite3* mDbHandle;
	static int mSelectRowCount;


public:
	static Word wordList[100];
	static int wordCount;

	DbHelper();
	~DbHelper();
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);
	void getWordListByLevel(int level, int randomEnabled);
	void getWordListByLesson(int fromLesson, int toLesson, int randomEnabled);
};

#endif
