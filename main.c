//#include "main.h"
//
//static GtkApplication *global_app = NULL; // 앱 객체를 전역 변수로 저장
//
//// 클릭 이벤트 핸들러
//static void on_button_clicked(GtkWidget *button, gpointer entry) {
//    const char *label = gtk_button_get_label(GTK_BUTTON(button));
//    const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));
//
//    // 버튼의 숫자를 입력 창에 추가
//    char new_text[11];
//    if (strlen(current_text) >= string_length) {
//        g_print("문자열 길이를 초과했습니다.\n");
//        return;
//    }
//    // 겹치는 문자 무시
//    for (int i = 0; current_text[i] != '\0'; ++i) {
//        if (current_text[i] == label[0]) {
//            g_print("중복된 숫자입니다.\n");
//            return;
//        }
//    }
//    snprintf(new_text, sizeof(new_text), "%s%s", current_text, label);
//    gtk_editable_set_text(GTK_EDITABLE(entry), new_text);
//}
//
//// 메인 창 표시
//static void show_main_window() {
//    // 윈도우 생성
//    GtkWidget *window = gtk_application_window_new(global_app);
//    gtk_window_set_title(GTK_WINDOW(window), "숫자 야구 게임");
//    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
//
//    // 메인 박스
//    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//    gtk_window_set_child(GTK_WINDOW(window), vbox);
//
//    // 제목 라벨
//    GtkWidget *label = gtk_label_new("숫자 야구 게임");
//    gtk_box_append(GTK_BOX(vbox), label);
//
//    // 입력 필드
//    GtkWidget *entry = gtk_entry_new();
//    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE); // 입력 불가능하도록 설정
//    gtk_box_append(GTK_BOX(vbox), entry);
//
//    // 버튼 박스 (숫자 버튼 5x2 배치)
//    GtkWidget *grid = gtk_grid_new();
//    gtk_box_append(GTK_BOX(vbox), grid);
//
//    // 숫자 버튼 생성 및 그리드에 추가
//    for (int i = 0; i < 10; i++) {
//        char label_text[2];
//        snprintf(label_text, sizeof(label_text), "%d", i); // 숫자 라벨 생성
//
//        GtkWidget *button = gtk_button_new_with_label(label_text);
//        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
//
//        // 버튼 그리드 배치
//        gtk_grid_attach(GTK_GRID(grid), button, i % 5, i / 5, 1, 1);
//    }
//
//    // 스타일 설정
//    gtk_widget_set_margin_top(vbox, 20);
//    gtk_widget_set_margin_bottom(vbox, 20);
//    gtk_widget_set_margin_start(vbox, 20);
//    gtk_widget_set_margin_end(vbox, 20);
//
//    gtk_window_present(GTK_WINDOW(window));
//}
//
//// 팝업 창에서 확인 버튼 이벤트
//static void on_popup_confirm(GtkWidget *button, gpointer entry) {
//    const char *input = gtk_editable_get_text(GTK_EDITABLE(entry));
//    string_length = atoi(input); // 문자열 길이 저장
//
//    if (string_length > 0 && string_length < 10) {
//        g_print("Selected string length: %d\n", string_length);
//        gtk_window_destroy(GTK_WINDOW(gtk_widget_get_root(button))); // 팝업 창 닫기
//        show_main_window(); // 팝업 닫힌 후 메인 창 표시
//    } else {
//        g_print("Invalid length entered.\n");
//    }
//}
//
//// 팝업 창 표시
//static void show_popup() {
//    GtkWidget *popup = gtk_application_window_new(global_app);
//    gtk_window_set_title(GTK_WINDOW(popup), "Select String Length");
//    gtk_window_set_default_size(GTK_WINDOW(popup), 300, 150);
//
//    // 팝업 박스
//    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//    gtk_window_set_child(GTK_WINDOW(popup), vbox);
//
//    // 입력 안내 라벨
//    GtkWidget *label = gtk_label_new("Enter the maximum string length:");
//    gtk_box_append(GTK_BOX(vbox), label);
//
//    // 입력 필드
//    GtkWidget *entry = gtk_entry_new();
//    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "e.g., 5");
//    gtk_box_append(GTK_BOX(vbox), entry);
//
//    // 확인 버튼
//    GtkWidget *button = gtk_button_new_with_label("Confirm");
//    gtk_box_append(GTK_BOX(vbox), button);
//
//    // 버튼 클릭 이벤트
//    g_signal_connect(button, "clicked", G_CALLBACK(on_popup_confirm), entry);
//
//    gtk_window_present(GTK_WINDOW(popup));
//}
//
//static void on_activate(GtkApplication *app, gpointer user_data) {
//    global_app = app; // 앱 객체를 전역 변수에 저장
//    show_popup();     // 팝업 창 표시
//}
//
//int main(int argc, char **argv) {
//    GtkApplication *app = gtk_application_new("com.example.popup", G_APPLICATION_DEFAULT_FLAGS);
//    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
//
//    int status = g_application_run(G_APPLICATION(app), argc, argv);
//    g_object_unref(app);
//
//    return status;
//}

#include "ui.h"

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.numberbaseball", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}

