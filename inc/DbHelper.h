#ifndef DBHELPER_H_
#define DBHELPER_H_

#include <sqlite3.h>
#include <string>
#include <vector>

const std::string dbFileName("wordlist_170120.db");

class DbHelper {
private:
	sqlite3* mDbHandle;

public:
	DbHelper();
	~DbHelper();
	//std::vector<word> getWordList();
	void showRecord();
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);
};

#endif
