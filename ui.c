#include "ui.h"
#include "gameLogic.h"

static GtkApplication *global_app = NULL; // 전역 앱 객체

// 팝업 창에서 확인 버튼 이벤트
static void on_popup_confirm(GtkWidget *button, gpointer entry) {
    const char *input = gtk_editable_get_text(GTK_EDITABLE(entry));
    int string_length = validate_input(input);

    if (string_length > 0) {
        set_string_length(string_length); // 게임 로직에 문자열 길이 설정
        gtk_window_destroy(GTK_WINDOW(gtk_widget_get_root(button))); // 팝업 창 닫기
        show_main_window(global_app); // 메인 창 표시
    } else {
        g_print("유효하지 않은 입력입니다.\n");
    }
}

// 팝업 창 표시
void show_popup(GtkApplication *app) {
    global_app = app;
    GtkWidget *popup = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(popup), "Select String Length");
    gtk_window_set_default_size(GTK_WINDOW(popup), 300, 150);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(popup), vbox);

    GtkWidget *label = gtk_label_new("Enter the maximum string length:");
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "e.g., 5");
    gtk_box_append(GTK_BOX(vbox), entry);

    GtkWidget *button = gtk_button_new_with_label("Confirm");
    gtk_box_append(GTK_BOX(vbox), button);

    g_signal_connect(button, "clicked", G_CALLBACK(on_popup_confirm), entry);

    gtk_window_present(GTK_WINDOW(popup));
}

// 버튼 클릭 이벤트
static void on_button_clicked(GtkWidget *button, gpointer entry) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    if (!add_input(current_text, label)) {
        g_print("중복되었거나 길이를 초과했습니다.\n");
        return;
    }

    gtk_editable_set_text(GTK_EDITABLE(entry), get_current_input());

    if (is_game_complete()) {
        g_print("게임 완료!\n");
        reset_game();
    }
}
// Clear 버튼 클릭 이벤트 핸들러
static void on_clear_button_clicked(gpointer entry) {
    reset_game(); // 게임 입력 데이터 초기화
    gtk_editable_set_text(GTK_EDITABLE(entry), ""); // 입력 필드 초기화
    g_print("입력이 초기화되었습니다.\n");
}

// 메인 창 표시
void show_main_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "숫자 야구 게임");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    GtkWidget *label = gtk_label_new("숫자 야구 게임");
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
    gtk_box_append(GTK_BOX(vbox), entry);

    GtkWidget *grid = gtk_grid_new();
    gtk_box_append(GTK_BOX(vbox), grid);

    for (int i = 0; i < 10; i++) {
        char label_text[2];
        snprintf(label_text, sizeof(label_text), "%d", i);

        GtkWidget *button = gtk_button_new_with_label(label_text);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(grid), button, i % 5, i / 5, 1, 1);
    }

    // Clear 버튼 추가
    GtkWidget *clear_button = gtk_button_new_with_label("Clear");
    gtk_box_append(GTK_BOX(vbox), clear_button);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), entry);

    gtk_window_present(GTK_WINDOW(window));
}


// 활성화 이벤트 핸들러
void on_activate(GtkApplication *app, gpointer user_data) {
    show_popup(app);
}
