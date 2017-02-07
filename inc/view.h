#if !defined(_VIEW_H)
#define _VIEW_H

#include <Elementary.h>
#include <efl_extension.h>

void view_create_with_size(int width, int height);
void view_create(void);
Evas_Object *view_create_win(const char *pkg_name);
Evas_Object *view_create_layout_for_part(Evas_Object *parent, char *file_path, char *group_name, char *part_name);
void view_destroy(void);
void view_set_clock(int hour, int minute, int second);
void view_reset_display_state(void);

#endif
