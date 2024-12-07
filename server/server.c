#include "server.h"

#define PORTNUM 9000
#define MAX_INPUT_LENGTH 10

char buf[256];
int sd, ns;
char solution[11];
static GtkApplication *global_app = NULL; // 전역 앱 객체
static GtkWidget *log_list = NULL; // 로그를 표시할 ListBox
static GtkWidget *popup = NULL; // 팝업 창
static GtkWidget *global_window = NULL; // 메인 창

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

// 게임 시작 함수
void game_start() {
    while (1) {
        int bytes_read = read(ns, buf, sizeof(buf));
        if (bytes_read == -1) {
            return;
        } else {

            buf[bytes_read] = '\0'; // 받은 데이터를 문자열로 처리
            // 종료 메시지 처리
            if (buf[0] == 'Q') {
                close(ns);
                close(sd);
                exit(0);
            }

            // 게임 로직 처리
            set_input(buf);
            const char *result = numBaseball();
            if (result != NULL) {
                add_log(buf, result);
                printf("Result: %s\n", result);
                write(ns, result, strlen(result));
            }

            // 게임 종료 조건
            if (get_game_status()) {
                reset_game();
                clear_game_status();
            }
        }
    }
}

// 비동기적으로 실행할 함수
void *game_start_thread(void *data) {
    game_start(); // 이 함수는 게임 시작 로직을 처리
    return NULL;
}

// Restart 버튼 클릭 이벤트 핸들러
static void on_restart_button_clicked(GtkWidget *button, gpointer data) {
    gtk_editable_set_text(GTK_EDITABLE(data), ""); // 입력 창 초기화
    reset_game(); // 게임 로직 초기화
    g_print("입력이 초기화되었습니다.\n");
    gtk_window_destroy(GTK_WINDOW(global_window)); // 메인 창 닫기
    show_popup(global_app); // 팝업 창 표시
    gtk_window_present(GTK_WINDOW(popup));
}

// 메인 창 표시
void show_main_window(GtkApplication *app) {
    global_app = app;
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "숫자 야구 게임");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    GtkWidget *label = gtk_label_new("숫자 야구 게임");
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *entry = gtk_entry_new();
    GtkWidget *restart_button = gtk_button_new_with_label("Restart");
    gtk_box_append(GTK_BOX(vbox), restart_button);
    g_signal_connect(restart_button, "clicked", G_CALLBACK(on_restart_button_clicked), entry);

    // 로그를 표시할 ListBox 추가
    log_list = gtk_list_box_new();
    gtk_box_append(GTK_BOX(vbox), log_list);

    gtk_window_present(GTK_WINDOW(window));
    global_window = window;
    // 게임 시작을 비동기적으로 실행
    pthread_t game_thread;
    if (pthread_create(&game_thread, NULL, game_start_thread, NULL) != 0) {
        g_printerr("Game thread creation failed\n");
        return;
    }
    show_popup(global_app);
}

// 팝업 창에서 확인 버튼 이벤트
static void on_popup_confirm(GtkWidget *button, gpointer entry) {
    const char *input = gtk_editable_get_text(GTK_EDITABLE(entry));
    if (!validate_input(input)) {
        g_print("유효하지 않은 입력입니다. %s\n", input);
        return;
    } else {
        set_solution(input); // 게임 로직에 문자열 길이 설정
        char length[2];
        sprintf(length, "%ld", strlen(input));
        write(ns, length, strlen(input));
        gtk_window_destroy(GTK_WINDOW(gtk_widget_get_root(button))); // 팝업 창 닫기
        show_main_window(global_app); // 메인 창 표시
        return;
    }
}

// 팝업 창 표시
void show_popup(GtkApplication *app) {
    global_app = app;
    popup = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(popup), "Type the solution");
    gtk_window_set_default_size(GTK_WINDOW(popup), 300, 150);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(popup), vbox);

    GtkWidget *label = gtk_label_new("Enter the solution:");
    gtk_box_append(GTK_BOX(vbox), label);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "e.g., 64382");
    gtk_box_append(GTK_BOX(vbox), entry);

    GtkWidget *button = gtk_button_new_with_label("Confirm");
    gtk_box_append(GTK_BOX(vbox), button);

    g_signal_connect(button, "clicked", G_CALLBACK(on_popup_confirm), entry);
}

// 활성화 이벤트 핸들러
void on_activate(GtkApplication *app, gpointer user_data) {
    show_popup(app);
    gtk_window_present(GTK_WINDOW(popup));
}

int main(int argc, char *argv[]) {
    struct sockaddr_in sin, cli;
    int clientlen = sizeof(cli);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    memset((char *)&sin, '\0', sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORTNUM);
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(sd, (struct sockaddr *)&sin, sizeof(sin))) {
        perror("bind");
        exit(1);
    }

    printf("Waiting for client...\n");

    if (listen(sd, 5)) {
        perror("listen");
        exit(1);
    }

    if ((ns = accept(sd, (struct sockaddr *)&cli, &clientlen)) == -1) {
        perror("accept");
        exit(1);
    }

    printf("[Client Connected]\n");

    GtkApplication *app = gtk_application_new("com.example.numberbaseballserver", 0);

    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return 0;
}
