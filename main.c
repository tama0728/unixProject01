#include <gtk/gtk.h>

// 클릭 이벤트 핸들러
static void on_button_clicked(GtkWidget *button, gpointer entry) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    // 버튼의 숫자를 입력 창에 추가
    char new_text[100];
    // 겹치는 문자 무시
    for (int i = 0; i < current_text[i] != '\0'; ++i) {
        if (current_text[i] == label[0]) {
            g_print("중복된 숫자입니다.\n");
            return;
        }
    }
    snprintf(new_text, sizeof(new_text), "%s%s", current_text, label);
    gtk_editable_set_text(GTK_EDITABLE(entry), new_text);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    // 윈도우 생성
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "숫자 야구 게임");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    // 메인 박스 (수직 방향)
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    // 제목 라벨
    GtkWidget *label = gtk_label_new("숫자 야구 게임");
    gtk_box_append(GTK_BOX(vbox), label);

    // 입력 필드
    GtkWidget *entry = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE); // 입력 불가능하도록 설정
    gtk_box_append(GTK_BOX(vbox), entry);

    // 버튼 박스 (숫자 버튼 2x5 배치)
    GtkWidget *grid = gtk_grid_new();
    gtk_box_append(GTK_BOX(vbox), grid);

    // 숫자 버튼 생성 및 그리드에 추가
    for (int i = 0; i < 10; i++) {
        char label_text[2];
        snprintf(label_text, sizeof(label_text), "%d", i); // 숫자 라벨 생성

        GtkWidget *button = gtk_button_new_with_label(label_text);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), entry);

        // 버튼 그리드 배치
        gtk_grid_attach(GTK_GRID(grid), button, i % 5, i / 5, 1, 1);
    }

    // 스타일 설정
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_bottom(vbox, 20);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);

    // 윈도우 표시
    gtk_widget_set_visible(window, TRUE);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("com.example.numbergame", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
