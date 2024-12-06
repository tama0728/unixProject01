#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib.h>

static GSocketService *socket_service = NULL;
// 애플리케이션 활성화 함수
void on_activate(GtkApplication *app, gpointer user_data);

// 메인 창 표시
void show_main_window(GtkApplication *app);

// 팝업 창 표시
void show_length_popup(GtkApplication *app);

// 버튼 클릭 이벤트 핸들러
static void on_clear_button_clicked(GtkWidget *button, gpointer entry);

#endif // UI_H
