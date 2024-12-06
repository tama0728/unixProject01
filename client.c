#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "gameLogic.h"

// 전역 변수 선언
static GtkApplication *global_app = NULL;
GtkWidget *window;
GtkWidget *log_list; // 로그를 표시할 ListBox
static GSocketConnection *current_connection = NULL;  // 연결 객체를 전역 변수로 선언
static GInputStream *input_stream = NULL;  // 입력 스트림
static GOutputStream *output_stream = NULL;  // 출력 스트림

void set_Connection(GSocketConnection *connection) {
    // 연결이 종료되지 않도록 연결을 전역 변수에 저장
    current_connection = connection;
    g_object_ref(current_connection);  // 연결 객체의 참조를 유지하도록 함

    // 입력 스트림과 출력 스트림을 설정
//    input_stream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
//    output_stream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
}

// 서버로부터 문제를 받고 정답을 제출하는 함수들
static void on_server_response_received(GSocketConnection *connection, GObject *source_object, gpointer user_data) {
    gchar buffer[1024];  // 수신된 데이터를 저장할 버퍼
    gsize bytes_read = 0;

    // 데이터를 읽기 위한 GError 객체
    GError *read_error = NULL;

    // 비동기적으로 데이터를 읽기
    bytes_read = g_input_stream_read(input_stream, buffer, sizeof(buffer) - 1, NULL, &read_error);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';  // 읽은 데이터의 끝에 null 문자를 추가

        g_print("서버에서 받은 데이터: %s\n", buffer);  // 디버깅용 출력

        // 데이터 처리
        int length = atoi(buffer);
        set_string_length(length);
    } else {
        if (read_error) {
            g_print("데이터 읽기 오류: %s\n", read_error->message);
            g_error_free(read_error);  // 오류 객체 해제
        } else {
            g_print("서버로부터 데이터를 받지 못했습니다.\n");
        }
    }
}

void connect_to_server(const char *address, int port) {
    GSocketClient *client = g_socket_client_new();
    GError *error = NULL;
    GSocketConnection *connection = g_socket_client_connect_to_host(client, address, port, NULL, &error);

    if (connection == NULL) {
        g_print("연결 오류: %s\n", error->message);
        g_error_free(error);
        return;
    }

    // 연결 후 GInputStream의 데이터를 읽기 위한 콜백 연결
    input_stream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    output_stream = g_io_stream_get_output_stream(G_IO_STREAM(connection));

    // "received" 시그널을 GSocketConnection에 연결
    g_signal_connect(connection, "received", G_CALLBACK(on_server_response_received), NULL);

    // 연결이 성공적으로 이루어졌음을 알림
    g_print("서버에 연결되었습니다: %s:%d\n", address, port);
    set_Connection(connection);
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

// 게임 로그 초기화
void clear_logs() {
    if (!log_list) return; // log_list가 초기화되지 않았으면 종료

    // GtkListBox의 모든 자식 위젯 제거
    GtkWidget *child = gtk_widget_get_first_child(GTK_WIDGET(log_list));
    while (child) {
        GtkWidget *next = gtk_widget_get_next_sibling(child);
        gtk_list_box_remove(GTK_LIST_BOX(log_list), child);
        child = next;
    }

    g_print("모든 로그가 삭제되었습니다.\n");
}

// 게임 로직 초기화
void reset_game() {
    clear_logs(); // 게임 로그 초기화
    gtk_window_destroy(window); // 로그 창 닫기
//    set_solution(""); // 정답 초기화
//    set_input(""); // 입력 초기화
    reset_input();
//    show_length_popup(global_app);          // 길이 입력 팝업 표시
}

// 숫자 버튼 클릭 시 호출되는 함수
static void on_button_clicked(GtkWidget *button, gpointer entry) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    add_input(current_text, label, entry);
    if (get_game_status() == 1) {
        g_print("게임이 종료되었습니다.\n");
        reset_game();
        clear_game_status();
    }
    if (is_game_complete()) {
        add_log(gtk_editable_get_text(GTK_EDITABLE(entry)), numBaseball());
    }
}

// Clear 버튼 클릭 이벤트 핸들러
static void on_clear_button_clicked(GtkWidget *button, gpointer data) {
    gtk_editable_set_text(GTK_EDITABLE(data), ""); // 입력 창 초기화
    set_input("");
}

// 메인 창 표시 함수
void show_main_window(GtkApplication *app) {
    window = gtk_application_window_new(app);
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

void on_activate(GtkApplication *app, gpointer user_data) {
    global_app = app;
    connect_to_server("127.0.0.1", 12345);  // 서버와 연결
    show_main_window(app);
}
