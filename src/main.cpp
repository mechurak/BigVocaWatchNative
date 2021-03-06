#include <tizen.h>
#include <system_settings.h>
#include "bigvocawatchnative.h"
#include "view.h"
#include "view_defines.h"
#include "DbHelper.h"
#include "PreferenceHelper.h"

int g_wordStatus = 0;
int g_wordIndex = 0;
int g_mode = 0;
int g_level = 25;
int g_lessonFrom = 1;
int g_lessonTo = 3;
int g_random = 0;

struct main_info {
	int hour;
	int minute;
	int cur_day;
	int cur_date;
};

static main_info s_time_info = {0, 0, 0};

static void _time_get(watch_time_h watch_time);
static void _curent_time_get(void);
static void _set_calendar(int day, int date);
static void settingChangedCb(int mode, int randomEnabled, int level, int lessonFrom, int lessonTo) {
	dlog_print(DLOG_INFO, LOG_TAG, "setting changed. mode: %d, random: %d, level: %d, lesson: %d ~ %d", mode, randomEnabled, level, lessonFrom, lessonTo);
	g_mode = mode;
	g_random = randomEnabled;
	g_level = level;
	g_lessonFrom = lessonFrom;
	g_lessonTo = lessonTo;

	PreferenceHelper::setInt(&PREF_KEY_MODE, g_mode);
	PreferenceHelper::setInt(&PREF_KEY_LEVEL, g_level);
	PreferenceHelper::setInt(&PREF_KEY_LESSON, g_lessonFrom);
	PreferenceHelper::setInt(&PREF_KEY_RANDOM, g_random);

	DbHelper* dbHelper = new DbHelper();
	if (g_mode == 0) {
		dbHelper->getWordListByLevel(g_level, g_random);
	}
	else {
		dbHelper->getWordListByLesson(g_lessonFrom, g_lessonTo, g_random);
	}
	g_wordStatus = 0;
	g_wordIndex = 0;

	view_set_chapter();
}

/*
 * @brief The system language changed event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void lang_changed(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when language setting is changed
	 */
	char *locale = NULL;

	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	if (locale == NULL)
		return;

	elm_language_set(locale);
	free(locale);

	return;
}

/*
 * @brief The region format changed event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void region_changed(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when region setting is changed
	 */
}

/*
 * @brief The low battery event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_battery(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low battery
	 */
	watch_app_exit();
}

/*
 * @brief The low memory event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void low_memory(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when system is running on low memory
	 */
	watch_app_exit();
}

/*
 * @brief The device orientation changed event callback function
 * @param[in] event_info The system event information
 * @param[in] user_data The user data passed from the add event handler function
 */
void device_orientation(app_event_info_h event_info, void* user_data)
{
	/*
	 * Takes necessary actions when device orientation is changed
	 */
}

/*
 * @brief Called when the application starts.
 * @param[in] width The width of the window of idle screen that will show the watch UI
 * @param[in] height The height of the window of idle screen that will show the watch UI
 * @param[in] user_data The user data passed from the callback registration function
 */
static bool app_create(int width, int height, void* user_data)
{
	/*
	 * Hook to take necessary actions before main event loop starts
	 * Initialize UI resources and application's data
	 */

	app_event_handler_h handlers[5] = {NULL, };

	/*
	 * Register callbacks for each system event
	 */
	if (watch_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, lang_changed, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, region_changed, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, low_battery, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, low_memory, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	if (watch_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, device_orientation, NULL) != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_add_event_handler () is failed");

	g_mode = PreferenceHelper::getInt(&PREF_KEY_MODE);
	g_level = PreferenceHelper::getInt(&PREF_KEY_LEVEL);
	g_lessonFrom = PreferenceHelper::getInt(&PREF_KEY_LESSON);
	g_lessonTo = g_lessonFrom + 2;
	g_random = PreferenceHelper::getInt(&PREF_KEY_RANDOM);
	if (g_level == 0 && g_lessonFrom == 0) {
		// no setting
		g_level = 25;
		g_lessonFrom = 1;
		g_lessonTo = g_lessonFrom + 2;
	}
	dlog_print(DLOG_INFO, LOG_TAG, "g_mode: %d", g_mode);
	dlog_print(DLOG_INFO, LOG_TAG, "g_level: %d", g_level);
	dlog_print(DLOG_INFO, LOG_TAG, "g_lessonFrom: %d", g_lessonFrom);
	dlog_print(DLOG_INFO, LOG_TAG, "g_random: %d", g_random);

	DbHelper* dbHelper = new DbHelper();
	if (!g_mode) {
		dbHelper->getWordListByLevel(g_level, g_random);
	} else {
		dbHelper->getWordListByLesson(g_lessonFrom, g_lessonTo, g_random);
	}

	view_create_with_size(width, height, settingChangedCb);

	_curent_time_get();

	return true;
}

/*
 * @brief: This callback function is called when another application
 * sends the launch request to the application
 */
static void app_control(app_control_h app_control, void *user_data)
{
	/*
	 * Handle the launch request.
	 */
}

/*
 * @brief: This callback function is called each time
 * the application is completely obscured by another application
 * and becomes invisible to the user
 */
static void app_pause(void *user_data)
{
	/*
	 * Take necessary actions when application becomes invisible.
	 */
}

/*
 * @brief: This callback function is called each time
 * the application becomes visible to the user
 */
static void app_resume(void *user_data)
{
	/*
	 * Take necessary actions when application becomes visible.
	 */

	view_reset_display_state();
	_curent_time_get();
}

/*
 * @brief: This callback function is called once after the main loop of the application exits
 */
static void app_terminate(void *user_data)
{
	view_destroy();
}

/*
 * @brief Called at each second. This callback is not called while the app is paused or the device is in ambient mode.
 * @param[in] watch_time The watch time handle. watch_time will not be available after returning this callback. It will be freed by the framework.
 * @param[in] user_data The user data to be passed to the callback functions
 */
static void app_time_tick(watch_time_h watch_time, void* user_data)
{
	int day = 0;
	int date = 0;
	watch_time_get_day_of_week(watch_time, &day);
	watch_time_get_day(watch_time, &date);
	_set_calendar(day, date);

	_time_get(watch_time);
	view_set_clock(s_time_info.hour, s_time_info.minute);

	int index = DbHelper::wordList[g_wordIndex].id;
	int mode;
	if (g_wordStatus % 2 == 0) {
		mode = 0;
	} else {
		mode = 1;
	}
	view_set_word(index, mode, DbHelper::wordList[g_wordIndex].spelling.c_str(), DbHelper::wordList[g_wordIndex].phonetic.c_str(), DbHelper::wordList[g_wordIndex].meaning.c_str());

	g_wordStatus++;
	if (g_wordStatus > 3) {
		g_wordStatus = 0;
		g_wordIndex++;
		if (g_wordIndex > DbHelper::wordCount - 1) {
			g_wordIndex = 0;
		}
	}
}

/*
 * @brief: Main function of the application
 */
int main(int argc, char *argv[])
{
	int ret = 0;

	watch_app_lifecycle_callback_s event_callback = {0, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;
	event_callback.time_tick = app_time_tick;
	event_callback.ambient_tick = NULL;
	event_callback.ambient_changed = NULL;

	ret = watch_app_main(argc, argv, &event_callback, NULL);
	if (ret != APP_ERROR_NONE)
		dlog_print(DLOG_ERROR, LOG_TAG, "watch_app_main() is failed. err = %d", ret);

	return ret;
}

/*
 * @brief Gets the time from a watch_time_h handler and stores it in the s_info structure
 * @param watch_time - watch_time_h structure to read the time from
 */
static void _time_get(watch_time_h watch_time)
{
	watch_time_get_minute(watch_time, &s_time_info. minute);
	watch_time_get_hour24(watch_time, &s_time_info.hour);
}

/*
 * @brief Reads the current time.
 */
static void _curent_time_get(void)
{
	watch_time_h watch_time;

	watch_time_get_current_time(&watch_time);
	_time_get(watch_time);
	view_set_clock(s_time_info.hour, s_time_info.minute);
	watch_time_delete(watch_time);

	view_set_chapter();
}

static void _set_calendar(int day, int date)
{
	if (s_time_info.cur_day != day || s_time_info.cur_date != date) {
		view_set_date(day, date);
		s_time_info.cur_day = day;
		s_time_info.cur_date = date;
	}
}
