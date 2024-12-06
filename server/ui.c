#include "ui.h"
#include "gameLogic.h"

static GtkApplication *global_app = NULL; // 전역 앱 객체
static GtkWidget *log_list = NULL; // 로그를 표시할 ListBox

char result[50];

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

void add_log(const char *input, const char *result) {
    if (!log_list) return; // log_list가 초기화되지 않았으면 종료

    // 로그 행(Row) 생성
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 35);

    // 입력과 결과를 결합한 텍스트
    char *log_text = g_strconcat(input, ": ", result, NULL);
    GtkWidget *input_label = gtk_label_new(log_text);
    g_free(log_text); // 동적 메모리 해제

    // 라벨을 로그 행에 추가
    gtk_box_append(GTK_BOX(row), input_label);

    // 로그 행을 ListBox에 추가
    gtk_list_box_append(GTK_LIST_BOX(log_list), row);

    // 화면 갱신: 로그 항목을 보이게 설정
    gtk_widget_set_visible(row, TRUE);
}

// 버튼 클릭 이벤트
static void on_button_clicked(GtkWidget *button, gpointer entry) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    add_input(current_text, label);
    gtk_editable_set_text(GTK_EDITABLE(entry), get_current_input()); // 입력 창 초기화
    if (is_game_complete()) {
        add_log(gtk_editable_get_text(GTK_EDITABLE(entry)), numBaseball());
    }
}

// Clear 버튼 클릭 이벤트 핸들러
static void on_clear_button_clicked(GtkWidget *button, gpointer data) {
    gtk_editable_set_text(GTK_EDITABLE(data), ""); // 입력 창 초기화
    reset_game(); // 게임 로직 초기화
    g_print("입력이 초기화되었습니다.\n");
}

// 메인 창 표시
void show_main_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "숫자 야구 게임");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);

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

    GtkWidget *clear_button = gtk_button_new_with_label("Clear");
    gtk_box_append(GTK_BOX(vbox), clear_button);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), entry);

    // 로그를 표시할 ListBox 추가
    log_list = gtk_list_box_new();
    gtk_box_append(GTK_BOX(vbox), log_list);

    gtk_window_present(GTK_WINDOW(window));
}


// 활성화 이벤트 핸들러
void on_activate(GtkApplication *app, gpointer user_data) {
    show_popup(app);
}
