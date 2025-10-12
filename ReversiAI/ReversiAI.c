#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

// --- 定数の定義 ---
#define BOARD_SIZE 8 // 盤面の一辺のサイズ

// --- 盤面の状態を表す定数 ---
#define EMPTY 0 // 0: 空き
#define BLACK 1 // 1: 黒石
#define WHITE 2 // 2: 白石

// --- プレイヤーの種類 ---
#define PLAYER_TYPE_HUMAN 0
#define PLAYER_TYPE_AI 1

// --- AIレベルの定義 ---
#define AI_LEVEL_RANDOM 1 // 初級: ランダム
#define AI_LEVEL_GREEDY 2 // 中級: 貪欲法

// --- グローバル変数 ---
int board[BOARD_SIZE][BOARD_SIZE]; // 盤面
int ai_level_black; // 黒番AIのレベル
int ai_level_white; // 白番AIのレベル

// --- 関数のプロトタイプ宣言 ---
void initializeBoard();
void printBoard();
bool isValidMove(int player, int row, int col);
void flipPieces(int player, int row, int col);
int countFlippablePieces(int player, int row, int col);
bool hasValidMove(int player);
void getHumanMove(int player, int* r, int* c);
void getRandomAIMove(int player, int* r, int* c);
void getGreedyAIMove(int player, int* r, int* c);
void printResult();
void clearInputBuffer();

// ゲーム全体の流れの管理
int main() {
    // 変数の宣言
    int player_black_type, player_white_type;
    int game_mode, human_color_choice;
    int currentPlayer = BLACK;

    // 乱数の初期化
    srand((unsigned int)time(NULL));

    // ゲーム設定
    printf("=== リバーシゲーム ===\n");
    printf("モードを選択してください (1: 人間 vs AI, 2: 人間 vs 人間, 3: AI vs AI): ");
    scanf("%d", &game_mode);
    while (game_mode < 1 || game_mode > 3) {
        printf("無効な選択です。(1, 2, 3): ");
        clearInputBuffer();
        scanf("%d", &game_mode);
    }

    if (game_mode == 1) { // 人間 vs AI
        printf("あなたの石の色を選択してください (1: 黒(先手), 2: 白(後手)): ");
        scanf("%d", &human_color_choice);
        while (human_color_choice != 1 && human_color_choice != 2) {
            printf("無効な選択です。(1 or 2): ");
            clearInputBuffer();
            scanf("%d", &human_color_choice);
        }

        printf("AIのレベルを選択してください (1: 初級, 2: 中級): ");
        int ai_level_choice;
        scanf("%d", &ai_level_choice);
        while (ai_level_choice != AI_LEVEL_RANDOM && ai_level_choice != AI_LEVEL_GREEDY) {
            printf("無効な選択です。(1 or 2): ");
            clearInputBuffer();
            scanf("%d", &ai_level_choice);
        }

        if (human_color_choice == 1) {
            player_black_type = PLAYER_TYPE_HUMAN;
            player_white_type = PLAYER_TYPE_AI;
            ai_level_white = ai_level_choice;
        }
        else {
            player_black_type = PLAYER_TYPE_AI;
            player_white_type = PLAYER_TYPE_HUMAN;
            ai_level_black = ai_level_choice;
        }
    }
    else if (game_mode == 2) { // 人間 vs 人間
        player_black_type = PLAYER_TYPE_HUMAN;
        player_white_type = PLAYER_TYPE_HUMAN;
    }
    else { // AI vs AI
        player_black_type = PLAYER_TYPE_AI;
        player_white_type = PLAYER_TYPE_AI;
        printf("黒(先手)AIのレベルを選択してください (1: 初級, 2: 中級): ");
        scanf("%d", &ai_level_black);
        while (ai_level_black != AI_LEVEL_RANDOM && ai_level_black != AI_LEVEL_GREEDY) {
            printf("無効な選択です。(1 or 2): ");
            clearInputBuffer();
            scanf("%d", &ai_level_black);
        }
        printf("白(後手)AIのレベルを選択してください (1: 初級, 2: 中級): ");
        scanf("%d", &ai_level_white);
        while (ai_level_white != AI_LEVEL_RANDOM && ai_level_white != AI_LEVEL_GREEDY) {
            printf("無効な選択です。(1 or 2): ");
            clearInputBuffer();
            scanf("%d", &ai_level_white);
        }
    }


    // ゲーム開始
    initializeBoard();

    while (true) {
        printBoard();

        // ゲーム終了判定
        if (!hasValidMove(BLACK) && !hasValidMove(WHITE)) {
            printf("両者とも置ける場所がありません。ゲーム終了です。\n");
            break;
        }

        // ターン進行
        int r = -1, c = -1;
        int current_player_type = (currentPlayer == BLACK) ? player_black_type : player_white_type;

        if (!hasValidMove(currentPlayer)) {
            printf("プレイヤー %s はパスします。\n", (currentPlayer == BLACK) ? "●" : "○");
        }
        else {
            if (current_player_type == PLAYER_TYPE_HUMAN) {
                getHumanMove(currentPlayer, &r, &c);
            }
            else { // AIのターン
                printf("AI (%s) の番です...\n", (currentPlayer == BLACK) ? "●" : "○");
                int current_ai_level = (currentPlayer == BLACK) ? ai_level_black : ai_level_white;

                if (current_ai_level == AI_LEVEL_RANDOM) {
                    getRandomAIMove(currentPlayer, &r, &c);
                }
                else {
                    getGreedyAIMove(currentPlayer, &r, &c);
                }
                printf("AIは %c%d に置きました。\n", 'a' + c, r + 1);
            }
            flipPieces(currentPlayer, r, c);
        }

        // AI vs AI モードの場合、進行を一時停止する
        if (game_mode == 3 && (hasValidMove(BLACK) || hasValidMove(WHITE))) {
            printf("Enterキーを押して次のターンへ進む...");
            clearInputBuffer();
            getchar();
        }

        // ターン交代
        currentPlayer = (currentPlayer == BLACK) ? WHITE : BLACK;
        printf("\n");
    }

    // 結果表示
    printResult();
    printf("続行するには何かキーを押してください...");
    clearInputBuffer();
    getchar();

    return 0;
}

// 盤面の初期化
void initializeBoard() {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            board[r][c] = EMPTY;
        }
    }
    board[3][3] = WHITE; board[3][4] = BLACK;
    board[4][3] = BLACK; board[4][4] = WHITE;
}

// 盤面の表示
void printBoard() {
    printf("\n  a b c d e f g h\n");
    for (int r = 0; r < BOARD_SIZE; r++) {
        printf("%d ", r + 1);
        for (int c = 0; c < BOARD_SIZE; c++) {
            switch (board[r][c]) {
            case EMPTY: printf(". "); break;
            case BLACK: printf("● "); break;
            case WHITE: printf("○ "); break;
            }
        }
        printf("\n");
    }
}

// 指定したマス(row, col)に石を置けるか判定
bool isValidMove(int player, int row, int col) {
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE || board[row][col] != EMPTY) {
        return false;
    }
    int opponent = (player == BLACK) ? WHITE : BLACK;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int r = row + dr; int c = col + dc;
            if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE || board[r][c] != opponent) {
                continue;
            }
            r += dr; c += dc;
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                if (board[r][c] == EMPTY) break;
                if (board[r][c] == player) return true;
                r += dr; c += dc;
            }
        }
    }
    return false;
}

// (row, col)に石を置き、相手の石をひっくり返す
void flipPieces(int player, int row, int col) {
    int opponent = (player == BLACK) ? WHITE : BLACK;

    // 指定された位置に石を置く
    board[row][col] = player;

    // 8方向をチェックし、ひっくり返す
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;

            int r = row + dr;
            int c = col + dc;
            bool canFlip = false;

            // 盤面内で相手の石が続いているかチェック
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                if (board[r][c] == EMPTY) break; // 空白ならその方向は終わり
                if (board[r][c] == player) { // 自分の石を見つけたら
                    canFlip = true; // ひっくり返せると確定
                    break;
                }
                r += dr; c += dc;
            }

            // ひっくり返せることが確定した場合、実際に処理を行う
            if (canFlip) {
                r = row + dr;
                c = col + dc;
                // 相手の石である限り、自分の石に変えていく
                while (board[r][c] == opponent) {
                    board[r][c] = player;
                    r += dr;
                    c += dc;
                }
            }
        }
    }
}

// 指定したマス(row, col)に置いた場合にひっくり返せる石の数を数える
int countFlippablePieces(int player, int row, int col) {
    if (!isValidMove(player, row, col)) return 0;
    int opponent = (player == BLACK) ? WHITE : BLACK;
    int totalFlips = 0;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int r = row + dr; int c = col + dc;
            int flipsInDir = 0;
            while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                if (board[r][c] == opponent) {
                    flipsInDir++;
                }
                else if (board[r][c] == player) {
                    totalFlips += flipsInDir;
                    break;
                }
                else { // EMPTY
                    break;
                }
                r += dr; c += dc;
            }
        }
    }
    return totalFlips;
}

// 指定したplayerが置ける場所があるかどうかの判定
bool hasValidMove(int player) {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (isValidMove(player, r, c)) return true;
        }
    }
    return false;
}

// プレイヤーから入力を受け付け、座標(r, c)に変換
void getHumanMove(int player, int* r, int* c) {
    printf("プレイヤー %s の番です。手を入力してください (例: d3): ", (player == BLACK) ? "●" : "○");
    while (true) {
        char col_char;
        int row_num;
        if (scanf(" %c%d", &col_char, &row_num) != 2) {
            clearInputBuffer();
            printf("入力形式が不正です。もう一度入力してください: ");
            continue;
        }
        *r = row_num - 1;
        *c = col_char - 'a';
        if (isValidMove(player, *r, *c)) {
            break;
        }
        else {
            printf("その場所には置けません。もう一度入力してください: ");
        }
    }
}

// AIの手の決定 (初級: ランダム)
void getRandomAIMove(int player, int* r, int* c) {
    int validMoves[BOARD_SIZE * BOARD_SIZE][2];
    int count = 0;
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (isValidMove(player, row, col)) {
                validMoves[count][0] = row;
                validMoves[count][1] = col;
                count++;
            }
        }
    }
    if (count > 0) {
        int choice = rand() % count;
        *r = validMoves[choice][0];
        *c = validMoves[choice][1];
    }
}

// AIの手の決定 (中級: 貪欲法)
void getGreedyAIMove(int player, int* r, int* c) {
    int maxFlips = -1;
    *r = -1;
    *c = -1;

    // 優先順位の高いマス（角）を先にチェック
    int corners[4][2] = { {0,0}, {0,7}, {7,0}, {7,7} };
    for (int i = 0; i < 4; i++) {
        if (isValidMove(player, corners[i][0], corners[i][1])) {
            *r = corners[i][0];
            *c = corners[i][1];
            return;
        }
    }

    // 全てのマスをチェックして最も多く返せる手を探す
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (isValidMove(player, row, col)) {
                int flips = countFlippablePieces(player, row, col);
                if (flips > maxFlips) {
                    maxFlips = flips;
                    *r = row;
                    *c = col;
                }
            }
        }
    }
}

// ゲーム終了時に結果を表示
void printResult() {
    int black_score = 0;
    int white_score = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] == BLACK) black_score++;
            else if (board[r][c] == WHITE) white_score++;
        }
    }
    printf("\n--- 結果発表 ---\n");
    printf("黒 ●: %d\n", black_score);
    printf("白 ○: %d\n", white_score);
    if (black_score > white_score) {
        printf("黒 ● の勝ちです！\n");
    }
    else if (white_score > black_score) {
        printf("白 ○ の勝ちです！\n");
    }
    else {
        printf("引き分けです。\n");
    }
}

// 標準入力バッファをクリア
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}