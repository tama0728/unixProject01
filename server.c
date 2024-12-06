#include "ui.h"
#include "gameLogic.h"

static GtkApplication *global_app = NULL; // 전역 앱 객체
static GtkWidget *log_list = NULL; // 로그를 표시할 ListBox
static GSocketConnection *current_connection = NULL;  // 연결 객체를 전역 변수로 선언
static GInputStream *input_stream = NULL;  // 입력 스트림
static GOutputStream *output_stream = NULL;  // 출력 스트림

GtkWidget *window = NULL;       // 메인 창

char result[50];

void set_Connection(GSocketConnection *connection) {
    // 연결이 종료되지 않도록 연결을 전역 변수에 저장
    current_connection = connection;
    g_object_ref(current_connection);  // 연결 객체의 참조를 유지하도록 함

    // 입력 스트림과 출력 스트림을 설정
    input_stream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
    output_stream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
}

// 정답 길이를 클라이언트에게 전송하는 함수
void send_answer_length_to_client(GSocketConnection *connection, int length) {
    gchar length_str[10];
    snprintf(length_str, sizeof(length_str), "%d", length);

    GOutputStream *output_stream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
    g_output_stream_write(output_stream, length_str, strlen(length_str), NULL, NULL);
    g_print("정답 길이 %d을 클라이언트에 전송했습니다.\n", length);
}

// 클라이언트 연결 핸들러
static gboolean on_client_connected(GSocketService *service, GSocketConnection *connection, GObject *source_object, gpointer user_data) {
    g_print("클라이언트가 연결되었습니다!\n");

    // 연결 후 대기 팝업을 닫고 문제 입력 팝업으로 이동
    GtkWidget *popup = GTK_WIDGET(user_data);
    gtk_window_destroy(GTK_WINDOW(popup)); // 대기 팝업 창 닫기

    set_Connection(connection);

    show_length_popup(global_app);          // 문제 입력 팝업 표시
    return TRUE; // 연결 이벤트 유지
}

// 클라이언트 대기 팝업 창 표시
void show_waiting_popup(GtkApplication *app) {
    global_app = app;

    // UI 생성
    GtkWidget *popup = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(popup), "Waiting for Client");
    gtk_window_set_default_size(GTK_WINDOW(popup), 300, 150);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(popup), vbox);

    GtkWidget *label = gtk_label_new("Waiting for Client...");
    gtk_box_append(GTK_BOX(vbox), label);

    gtk_window_present(GTK_WINDOW(popup));  // 대기 창 표시

    // 소켓 서버 설정
    socket_service = g_socket_service_new();
    GError *error = NULL;

    // 소켓 서버 설정 (포트 12345)
    if (!g_socket_listener_add_inet_port(G_SOCKET_LISTENER(socket_service), 12345, NULL, &error)) {
        g_printerr("소켓 열기 실패: %s\n", error->message);
        g_error_free(error);
        return;
    }

    // 연결을 기다리며, 클라이언트 연결 시 호출되는 핸들러 설정
    g_signal_connect(socket_service, "incoming", G_CALLBACK(on_client_connected), popup);

    g_print("클라이언트 연결 대기 중...\n");
    g_socket_service_start(socket_service); // 소켓 서비스 시작
}

// 문제 입력 팝업 창 표시
void show_length_popup(GtkApplication *app) {
    global_app = app;
    GtkWidget *popup = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(popup), "Set Solution");
    gtk_window_set_default_size(GTK_WINDOW(popup), 400, 150);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(popup), vbox);

    GtkWidget *label = gtk_label_new("Enter the solution:");
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "e.g., 64382");
    gtk_box_append(GTK_BOX(vbox), entry);

    GtkWidget *confirm_button = gtk_button_new_with_label("Confirm");
    gtk_box_append(GTK_BOX(vbox), confirm_button);

    g_signal_connect(confirm_button, "clicked", G_CALLBACK(on_popup_confirm_with_solution), entry);
    gtk_window_present(GTK_WINDOW(popup));
}

// 정답 입력 팝업에서 확인 버튼 클릭 처리
static void on_popup_confirm_with_solution(GtkWidget *button, gpointer entry) {
    const char *solution_input = gtk_editable_get_text(GTK_EDITABLE(entry));

    if (strlen(solution_input) > 0) {
        // 정답을 서버에 저장
        set_solution(solution_input);

        gtk_window_destroy(GTK_WINDOW(gtk_widget_get_root(button)));  // 팝업 창 닫기
        show_main_window(global_app);  // 메인 창 표시
        send_answer_length_to_client(current_connection, get_string_length());
    } else {
        g_print("정답을 입력하세요.\n");
    }
}

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

    // 로그를 표시할 ListBox 추가
    log_list = gtk_list_box_new();
    gtk_box_append(GTK_BOX(vbox), log_list);

    gtk_window_present(GTK_WINDOW(window));
}

// 서버 활성화 핸들러
void on_activate(GtkApplication *app, gpointer user_data) {
    show_waiting_popup(app); // 서버 대기 상태로 전환
}
