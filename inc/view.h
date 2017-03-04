#if !defined(_VIEW_H)
#define _VIEW_H

#include <Elementary.h>
#include <efl_extension.h>

void view_create_with_size(int width, int height, void(*settingCb)(int,int,int,int,int));
void view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_layout_for_part(Evas_Object *parent, char *file_path, char *group_name, char *part_name);
void view_destroy(void);
void view_set_clock(int hour, int minute, int second);
void view_set_word(int index, int mode, const char* spelling, const char* phonetic, const char* meaning);
char* get_day_name(int day);
void view_set_date(int day, int date);
void view_reset_display_state(void);

void view_send_signal_to_edje(Evas_Object *layout, const char *signal, const char *source);
void view_set_customized_event_callback(Evas_Object *item, char *signal, char *source, Edje_Signal_Cb signal_cb, void *user_data);

#endif
