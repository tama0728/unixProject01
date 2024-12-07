#include "client.h"

#define PORTNUM 9000
#define MAX_INPUT_LENGTH 10

int sd;
char buf[256];
static GtkWidget *log_list = NULL; // 로그를 표시할 ListBox
static GtkWidget *window = NULL; // 메인 창

void clear_log() {
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

void restart() {
    // 정답 길이 read
    if (read(sd, buf, sizeof(buf)) == -1) {
        perror("read");
        exit(1);
    }

    buf[strcspn(buf, "\n")] = '\0'; // 입력 끝의 개행 문자 제거
    printf("Solution length: %c\n", buf[0]);

    set_string_length(atoi(buf));
    char length[2];
    sprintf(length, "%d", get_string_length());
    clear_log();
    add_log("length: ", length);
}

// 비동기적으로 실행할 함수
void *game_restart_thread(void *data) {
    restart(); // 이 함수는 게임 시작 로직을 처리
    return NULL;
}

// 게임 로직 초기화
void reset_game() {
    pthread_t game_thread;
    if (pthread_create(&game_thread, NULL, game_restart_thread, NULL) != 0) {
        g_printerr("Game thread creation failed\n");
        return;
    }
}

// 버튼 클릭 이벤트
static void on_button_clicked(GtkWidget *button, gpointer entry) {
    const char *label = gtk_button_get_label(GTK_BUTTON(button));
    const char *current_text = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (buf[0] == 'Y') {
        buf[0] = 'N';
        reset_game();
    }

    add_input(current_text, label);
    gtk_editable_set_text(GTK_EDITABLE(entry), get_current_input()); // 입력 창 초기화
    if (is_game_complete()) {
//     서버로 입력 전송
        strcpy(buf, gtk_editable_get_text(GTK_EDITABLE(entry)));
        if (write(sd, buf, strlen(buf)) == -1) {
            perror("write");
            exit(1);
        }

        // 서버로부터 응답 받기
        int bytes_read = read(sd, buf, sizeof(buf));
        if (bytes_read == -1) {
            perror("read");
            exit(1);
        }

        buf[bytes_read] = '\0'; // 응답 데이터 처리
        printf("Server: %s\n", buf);
        add_log(gtk_editable_get_text(GTK_EDITABLE(entry)), buf);
    }
}

// Clear 버튼 클릭 이벤트 핸들러
static void on_clear_button_clicked(GtkWidget *button, gpointer data) {
    gtk_editable_set_text(GTK_EDITABLE(data), ""); // 입력 창 초기화
    reset_input(); // 게임 로직 초기화
    g_print("입력이 초기화되었습니다.\n");
}

// Quit 버튼 클릭 이벤트 핸들러
static void on_quit_button_clicked(GtkWidget *button, gpointer data) {
    gtk_window_destroy(GTK_WINDOW(window)); // 메인 창 닫기
    write(sd, "Q", 2); // 서버에 종료 메시지 전송
    close(sd); // 소켓 닫기
    exit(0);
}

// 메인 창 표시
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

    // 숫자 버튼 생성
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

    GtkWidget *quit_button = gtk_button_new_with_label("Quit");
    gtk_box_append(GTK_BOX(vbox), quit_button);
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit_button_clicked), entry);

    // 로그를 표시할 ListBox 추가
    log_list = gtk_list_box_new();
    gtk_box_append(GTK_BOX(vbox), log_list);

    // 정답 길이 출력
    char length[2];
    sprintf(length, "%d", get_string_length());
    add_log("length: ", length);

    gtk_window_present(GTK_WINDOW(window));
}

// 활성화 이벤트 핸들러
void on_activate(GtkApplication *app, gpointer user_data) {
    show_main_window(app);
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    struct sockaddr_in server;

    // 서버 소켓 생성
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset((char *)&server, '\0', sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORTNUM);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 서버와 연결
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        exit(1);
    }

    printf("Connected to server.\n");

    // 정답 길이 read
    if (read(sd, buf, sizeof(buf)) == -1) {
        perror("read");
        exit(1);
    }

    buf[strcspn(buf, "\n")] = '\0'; // 입력 끝의 개행 문자 제거
    printf("Solution length: %s\n", buf);
    set_string_length(atoi(buf));

    GtkApplication *app = gtk_application_new("com.example.numberbaseballclient", 0);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return 0;
}
