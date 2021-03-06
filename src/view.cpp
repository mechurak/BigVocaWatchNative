#include <watch_app_efl.h>
#include "bigvocawatchnative.h"
#include "view.h"
#include "view_defines.h"

#define MAIN_EDJ "edje/main.edj"

extern int g_mode;
extern int g_level;
extern int g_lessonFrom;
extern int g_lessonTo;
extern int g_random;

struct view_info {
	Evas_Object *win;
	Evas_Object *layout;
	Evas_Object* settingLayout;
	int w;
	int h;
	int settingLayoutEnabled;
	int mode;
	int level;
	int lesson;
	int randomEnabled;
	void(*settingCb)(int,int,int,int,int);
	int hour;
	int minute;
	int isPm;
};

static view_info s_info = {
	NULL,
	NULL,
	NULL,
	0, 0, 0, 0,
	0, 0, 1,
	NULL,
	10, 8, 0
};

static void _message_outside_object_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg);
static char *_create_resource_path(const char *file_name);
static Evas_Object *_create_layout(void);


/**
 * @brief This function is called when battery module is clicked.
 * @param[in] data A pointer to the data to pass to the callback function
 * @param[in] obj The Edje object where the signal comes from
 * @param[in] emission The signal name
 * @param[in] source The signal source
 */
static void _temp_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "temp_cb: %s. %s", emission, source);
	if (s_info.settingLayoutEnabled) {
		elm_object_signal_emit(s_info.layout, "hide", "setting");
		s_info.settingLayoutEnabled = 0;
	} else {
		elm_object_signal_emit(s_info.layout, "show", "setting");
		s_info.settingLayoutEnabled = 1;

		s_info.mode = g_mode;
		s_info.level = g_level;
		s_info.lesson = g_lessonFrom;
		s_info.randomEnabled = g_random;

		Eina_Stringshare* levelStr = eina_stringshare_printf("%d", s_info.level);
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.levelValue", (const char*)levelStr);
		eina_stringshare_del(levelStr);

		Eina_Stringshare* lessonStr = eina_stringshare_printf("%d", s_info.lesson);
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.lessonValue", (const char*)lessonStr);
		eina_stringshare_del(lessonStr);

		const char* levelTitleStr = (s_info.mode == 0) ? (const char*)"level (*)" : (const char*)"level";
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.level", (const char*)levelTitleStr);

		const char* lessonTitleStr = (s_info.mode == 1) ? (const char*)"lesson (*)" : (const char*)"lesson";
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.lesson", (const char*)lessonTitleStr);

		const char* randomStr = s_info.randomEnabled ? (const char*)"random on" : (const char*)"random off";
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.random", (const char*)randomStr);
	}
}

static void _level_cb(void *data, Evas_Object *obj, const char *emission, const char* source)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "_level_cb: %s. %s", emission, source);
	int prevMode = s_info.mode;
	if (strcmp(source, (char*)"level,down") == 0) {
		s_info.level--;
		if (s_info.level < 1) s_info.level = 1;
		s_info.mode = 0;
	}
	else if (strcmp(source, (char*)"level,up") == 0) {
		s_info.level++;
		if (s_info.level > 80) s_info.level = 80; // max level(80) : no relation with lesson
		s_info.mode = 0;
	}
	else if (strcmp(source, (char*)"lesson,down") == 0) {
		s_info.lesson--;
		if (s_info.lesson < 1) s_info.lesson = 1;
		s_info.mode = 1;
	}
	else if (strcmp(source, (char*)"lesson,up") == 0) {
		s_info.lesson++;
		if (s_info.lesson > 78) s_info.lesson = 78; // max lesson(80) - 2 (currently, 3 lessons are one set)
		s_info.mode = 1;
	}
	Eina_Stringshare* levelStr = eina_stringshare_printf("%d", s_info.level);
	elm_layout_text_set(s_info.settingLayout, (const char*)"txt.levelValue", (const char*)levelStr);
	eina_stringshare_del(levelStr);

	Eina_Stringshare* lessonStr = eina_stringshare_printf("%d", s_info.lesson);
	elm_layout_text_set(s_info.settingLayout, (const char*)"txt.lessonValue", (const char*)lessonStr);
	eina_stringshare_del(lessonStr);

	if (prevMode != s_info.mode) {
		const char* levelTitleStr = (s_info.mode == 0) ? (const char*)"level (*)" : (const char*)"level";
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.level", (const char*)levelTitleStr);

		const char* lessonTitleStr = (s_info.mode == 1) ? (const char*)"lesson (*)" : (const char*)"lesson";
		elm_layout_text_set(s_info.settingLayout, (const char*)"txt.lesson", (const char*)lessonTitleStr);
	}
}

static void _random_cb(void *data, Evas_Object *obj, const char *emission, const char* source)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "_random_cb: %s. %s", emission, source);
	s_info.randomEnabled = s_info.randomEnabled ? 0 : 1;

	const char* randomStr = s_info.randomEnabled ? (const char*)"random on" : (const char*)"random off";
	elm_layout_text_set(s_info.settingLayout, (const char*)"txt.random", (const char*)randomStr);
}

static void _submit_cb(void *data, Evas_Object *obj, const char *emission, const char* source)
{
	dlog_print(DLOG_DEBUG, LOG_TAG, "_submit_cb: %s. %s", emission, source);
	if (strcmp(source, (char*)"submit") == 0) {
		s_info.settingCb(s_info.mode, s_info.randomEnabled, s_info.level, s_info.lesson, s_info.lesson + 2);
	}
	elm_object_signal_emit(s_info.layout, "hide", "setting");
	s_info.settingLayoutEnabled = 0;
}


/*
 * @brief Creates the application view with received size
 * @param width - Application width
 * @param height - Application height
 */
void view_create_with_size(int width, int height, void(*cb)(int, int, int, int, int))
{
	s_info.w = width;
	s_info.h = height;
	s_info.settingCb = cb;
	view_create();
}

/*
 * @brief: Create Essential Object window, conformant and layout
 */
void view_create(void)
{
	/* Create window */
	s_info.win = view_create_win(PACKAGE);
	if (s_info.win == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create a window.");

		return;
	}

	/* Layout */
	s_info.layout = _create_layout();
	if (!s_info.layout) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to create main layout.");
		return;
	}

	view_set_customized_event_callback(s_info.layout, (char*)"mouse,clicked", (char*)"setting", _temp_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"level,down", _level_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"level,up", _level_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"lesson,down", _level_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"lesson,up", _level_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"random", _random_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"submit", _submit_cb, NULL);
	view_set_customized_event_callback(s_info.settingLayout, (char*)"mouse,clicked", (char*)"cancel", _submit_cb, NULL);

	/* Show window after main view is set up */
	evas_object_show(s_info.win);
}

/*
 * @brief: Make a basic window named package
 * @param[pkg_name]: Name of the window
 */
Evas_Object *view_create_win(const char *pkg_name)
{
	Evas_Object *win = NULL;
	int ret;

	/*
	 * Window
	 * Create and initialize elm_win.
	 * elm_win is mandatory to manipulate window
	 */
	ret = watch_app_get_elm_win(&win);
	if (ret != APP_ERROR_NONE)
		return NULL;

	elm_win_title_set(win, pkg_name);
	elm_win_borderless_set(win, EINA_TRUE);
	elm_win_alpha_set(win, EINA_FALSE);
	elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
	elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_BG_TRANSPARENT);
	elm_win_prop_focus_skip_set(win, EINA_TRUE);
	elm_win_role_set(win, "no-effect");

	evas_object_resize(win, s_info.w, s_info.h);
	return win;
}

/*
 * @brief: Make and set a layout to the part
 * @param[parent]: Object to which you want to set this layout
 * @param[file_path]: File path of EDJ will be used
 * @param[group_name]: Group name in EDJ that you want to set to layout
 * @param[part_name]: Part name to which you want to set this layout
 */
Evas_Object *view_create_layout_for_part(Evas_Object *parent, char *file_path, char *group_name, char *part_name)
{
	Evas_Object *layout = NULL;

	if (parent == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "parent is NULL.");
		return NULL;
	}

	layout = elm_layout_add(parent);
	elm_layout_file_set(layout, file_path, group_name);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(layout);
	elm_object_part_content_set(parent, part_name, layout);
	edje_object_message_handler_set(elm_layout_edje_get(layout), _message_outside_object_cb, NULL);

	return layout;
}

/*
 * @brief: Destroy window and free important data to finish this application
 */
void view_destroy(void)
{
	if (s_info.win == NULL)
		return;

	evas_object_del(s_info.win);
}

/*
 * @brief Stores the hour value
 * @param hour - New hour value
 */
void view_set_clock(int hour, int minute)
{
	int isPm = 0;
	if (hour > 12) {
		hour -= 12;
		isPm = 1;
	}
	if (hour == 0) hour = 12;

	if (isPm != s_info.isPm) {
		const char* pmStr = isPm ? (const char*)"PM" : (const char*)"AM";
		elm_layout_text_set(s_info.layout, (const char*)"txt.pm", (const char*)pmStr);
		s_info.isPm = isPm;
	}

	if (minute != s_info.minute || hour != s_info.hour) {
		Eina_Stringshare *str = eina_stringshare_printf("%.2d  %.2d", hour, minute);
		elm_layout_text_set(s_info.layout, (const char*)PART_CLOCK_TEXT, (const char*)str);
		eina_stringshare_del(str);
		s_info.hour = hour;
		s_info.minute = minute;
	}
}

void view_set_word(int index, int mode, const char* spelling, const char* phonetic, const char* meaning) {
	Eina_Stringshare* indexStr = eina_stringshare_printf("%d", index);
	elm_layout_text_set(s_info.layout, (const char*)PART_WORD_INDEX_TEXT, (const char*)indexStr);
	eina_stringshare_del(indexStr);

	if (!mode) {
		elm_layout_text_set(s_info.layout, (const char*)"txt.spelling", spelling);
		elm_layout_text_set(s_info.layout, (const char*)"txt.phonetic", phonetic);
		elm_layout_text_set(s_info.layout, (const char*)PART_WORD_TEXT, (const char*)"");
	} else {
		elm_layout_text_set(s_info.layout, (const char*)"txt.spelling", (const char*)"");
		elm_layout_text_set(s_info.layout, (const char*)"txt.phonetic", (const char*)"");
		elm_layout_text_set(s_info.layout, (const char*)PART_WORD_TEXT, meaning);
	}
}

char* get_day_name(int day)
{
	const char* day_name[7] = { "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
	return strdup(day_name[day]);
}

void view_set_date(int day, int date) {
	char* day_name = NULL;
	day_name = get_day_name(day - 1);
	Eina_Stringshare* dateStr = eina_stringshare_printf("%s %d", day_name, date);
	elm_layout_text_set(s_info.layout, (const char*)"txt.date", (const char*)dateStr);
	eina_stringshare_del(dateStr);
}

void view_set_chapter() {
	Eina_Stringshare* chapterStr = NULL;
	if (!g_mode) {
		chapterStr = eina_stringshare_printf("Lv. %d", g_level);
	} else {
		chapterStr = eina_stringshare_printf("D.%d~%d", g_lessonFrom, g_lessonTo);
	}
	elm_layout_text_set(s_info.layout, (const char*)"txt.chapter", (const char*)chapterStr);
	eina_stringshare_del(chapterStr);
}

/*
 * @brief Sets the hour value displayed by the edje layout
 * @param hour - Hour value
 */
void view_reset_display_state(void)
{
	elm_layout_signal_emit(s_info.layout, "load", "");
}

/*
 * @brief Callback invoked when a message from the edje is received. Used to update the value of the last 'minute' part
 * @param data - User data
 * @param obj - Edje object which sent the message
 * @param type - The message type
 * @param id - The message id
 * @param msg - The message value
 */
static void _message_outside_object_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
	Edje_Message_String *edje_msg = NULL;

	if (type != EDJE_MESSAGE_STRING || id != 1) { // MSG_ID_MINUTE_PART_NEW_OUTSIDE 1
		dlog_print(DLOG_ERROR, LOG_TAG, "[%s:%d] Unknown message", __FILE__, __LINE__);
		return;
	}

	edje_msg = (Edje_Message_String *)msg;
	dlog_print(DLOG_INFO, LOG_TAG, "msg: %s", edje_msg);
}

/*
 * @brief: Creates path to the given resource file by concatenation of the basic resource path and the given file_name.
 * @param[file_name]: File name or path relative to the resource directory.
 * @return: The absolute path to the resource with given file_name is returned.
 */
static char *_create_resource_path(const char *file_name)
{
	static char res_path_buff[PATH_MAX] = {0,};
	char *res_path = NULL;

	res_path = app_get_resource_path();
	if (res_path == NULL) {
		dlog_print(DLOG_ERROR, LOG_TAG, "failed to get resource path.");
		return NULL;
	}

	snprintf(res_path_buff, PATH_MAX, "%s%s", res_path, file_name);
	free(res_path);

	return &res_path_buff[0];
}

/*
 * @brief: Creates the application's layout.
 * @return: The Evas_Object of the layout created.
 */
static Evas_Object *_create_layout(void)
{
	char *edj_path = NULL;

	edj_path = _create_resource_path(MAIN_EDJ);

	Evas_Object *layout = view_create_layout_for_part(s_info.win, edj_path, (char*)LAYOUT_GROUP_MAIN, (char*)"default");
	if (!layout)
		return NULL;

	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_min_set(layout, s_info.w, s_info.h);
	evas_object_resize(layout, s_info.w, s_info.h);
	evas_object_show(layout);

	dlog_print(DLOG_INFO, LOG_TAG, "_create_layout. %d, %d", s_info.w, s_info.h);


	s_info.settingLayout = elm_layout_add(layout);
	elm_layout_file_set(s_info.settingLayout, edj_path, (char*)"layout_group_wordPopup");
	elm_object_part_content_set(layout, "swallow.wordPopup", s_info.settingLayout);

	return layout;
}

/**
 * @brief Sends signal with source to the EDJE object.
 * @param[in] layout The layout object will receive the signal
 * @param[in] signal The appointed signal to trigger the function
 * @param[in] source The appointed source that normally indicate the object triggered the event
 */
void view_send_signal_to_edje(Evas_Object *layout, const char *signal, const char *source)
{
	elm_object_signal_emit(layout, signal, source);
}

/**
 * @brief Sets the function will be called when the appointed signal is occurred.
 * @param[in] item The object triggered the signal
 * @param[in] signal The appointed signal to trigger the function
 * @param[in] source The appointed source that normally indicate the object triggered the event
 * @param[in] signal_cb The function will be called when the signal is detected
 * @param[in] user_data The data passed to the 'signal_cb' function
 */
void view_set_customized_event_callback(Evas_Object *item, char *signal, char *source, Edje_Signal_Cb signal_cb, void *user_data)
{
	elm_object_signal_callback_add(item, signal, source, signal_cb, user_data);
}
