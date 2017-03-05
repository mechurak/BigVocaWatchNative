#include "PreferenceHelper.h"

int PreferenceHelper::getInt(const std::string* key) {
	int ret = 0;
	preference_get_int(key->c_str(), &ret);
	return ret;
}
void PreferenceHelper::setInt(const std::string* key, const int value) {
	preference_set_int(key->c_str(), value);
}
