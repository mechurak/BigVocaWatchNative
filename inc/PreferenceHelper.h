#ifndef PREFERENCEHELPER_H_
#define PREFERENCEHELPER_H_

#include <app_preference.h>
#include <string>

static const std::string PREF_KEY_MODE = "mode";
static const std::string PREF_KEY_LEVEL = "level";
static const std::string PREF_KEY_LESSON = "lesson";
static const std::string PREF_KEY_RANDOM = "random";

class PreferenceHelper {
public:
	static int getInt(const std::string* key);
	static void setInt(const std::string* key, const int value);
};

#endif /* PREFERENCEHELPER_H_ */
