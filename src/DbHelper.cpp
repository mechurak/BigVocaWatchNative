#include "bigvocawatchnative.h"
#include "DbHelper.h"

using namespace std;

int DbHelper::mSelectRowCount = 0;
Word DbHelper::wordList[100];
int DbHelper::wordCount = 0;


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

int DbHelper::callback(void *NotUsed, int argc, char **argv, char **azColName) {
	dlog_print(DLOG_DEBUG, LOG_TAG, "callback. mSelectRowCount: %d", mSelectRowCount);

	wordList[mSelectRowCount].id = atoi(argv[0]);
	wordList[mSelectRowCount].day = atoi(argv[1]);
	wordList[mSelectRowCount].spelling = string(argv[2]);
	wordList[mSelectRowCount].phonetic = "[" + string(argv[3]) + "]";
	wordList[mSelectRowCount].meaning = string(argv[4]);

	dlog_print(DLOG_DEBUG, LOG_TAG, "  %d, %d, %s, %s, %s", wordList[mSelectRowCount].id, wordList[mSelectRowCount].day, wordList[mSelectRowCount].spelling.c_str(), wordList[mSelectRowCount].phonetic.c_str(), wordList[mSelectRowCount].meaning.c_str());

	mSelectRowCount++;
	return SQLITE_OK;
}

void DbHelper::getWordListByLevel(int level, int randomEnabled) {
	int fromId = (level - 1) * 100 + 1;
	int toId = level * 100;
	dlog_print(DLOG_INFO, LOG_TAG, "getWordListByLevel. from: %d, to: %d", fromId, toId);

	char sql[200];
	const char* orderByValue = randomEnabled ? "random()" : "id";
	snprintf(sql, 200, "SELECT * FROM word WHERE id >= %d AND id <= %d ORDER BY %s;", fromId, toId, orderByValue);
    int counter = 0;
    char *ErrMsg;

    mSelectRowCount = 0;

    sqlite3_exec(mDbHandle, sql, callback, &counter, &ErrMsg);
    dlog_print(DLOG_INFO, LOG_TAG, "after sqlite3_exec. %d. %s", counter, ErrMsg);
    wordCount = mSelectRowCount;

    return;
}

void DbHelper::getWordListByLesson(int fromLesson, int toLesson, int randomEnabled) {
	char sql[200];
	const char* orderByValue = randomEnabled ? "random()" : "id";
	snprintf(sql, 200, "SELECT * FROM word WHERE day >= %d AND day <= %d ORDER BY day, %s;", fromLesson, toLesson, orderByValue);
    int counter = 0;
    char *ErrMsg;

    mSelectRowCount = 0;

    sqlite3_exec(mDbHandle, sql, callback, &counter, &ErrMsg);
    dlog_print(DLOG_ERROR, LOG_TAG, "after sqlite3_exec. %d. %s", counter, ErrMsg);
    wordCount = mSelectRowCount;

    return;
}
