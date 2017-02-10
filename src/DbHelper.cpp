#include "bigvocawatchnative.h"
#include "DbHelper.h"

using namespace std;

DbHelper::DbHelper() {
	dlog_print(DLOG_INFO, LOG_TAG, "DbHelper ctor");
	char* sharedPath = app_get_shared_resource_path();
	string filePath(sharedPath);
	filePath = filePath + dbFileName;
	dlog_print(DLOG_DEBUG, LOG_TAG, "filePath: %s", filePath.c_str());

	int openResult = sqlite3_open_v2(filePath.c_str(), &mDbHandle, SQLITE_OPEN_READONLY, NULL);
	dlog_print(DLOG_DEBUG, LOG_TAG, "openResult: %d", openResult);
}

DbHelper::~DbHelper() {
	dlog_print(DLOG_INFO, LOG_TAG, "DbHelper dtor");
	if (mDbHandle) {
		sqlite3_close(mDbHandle);
	}
}

/*
std::vector<word> DbHelper::getWordList() {
	if (!mDbHandle) {
		dlog_print(DLOG_WARN, LOG_TAG, "[%s,%d] db error", __FILE__, __LINE__);
		return NULL;
	}
}
*/

void DbHelper::showRecord() {
    char *sql = "select * from word";
    int counter = 0;
    int ret = 0;
    char *ErrMsg;


    sqlite3_exec(mDbHandle, sql, callback, &counter, &ErrMsg);

    return;
}

int DbHelper::callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;
	for(i=0; i<argc; i++){
		dlog_print(DLOG_INFO, LOG_TAG, "%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	return 0;
}
