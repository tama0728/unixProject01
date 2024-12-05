//
// Created by DongYun-Kim on 12/5/24.
//

#ifndef UNIXPROJECT01_MAIN_H
#define UNIXPROJECT01_MAIN_H

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>

// 전역 변수로 문자열 길이 저장
static int string_length = 0;
static char solution[11] = "64382";

// 클릭 이벤트 핸들러
static void on_button_clicked(GtkWidget *button, gpointer entry);
static void on_popup_confirm(GtkWidget *button, gpointer entry);
static void show_popup();
static void on_activate(GtkApplication *app, gpointer user_data);


#endif //UNIXPROJECT01_MAIN_H
