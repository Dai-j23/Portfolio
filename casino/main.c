#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 定義
#define DATA_FILE_NAME "userdata.txt" // データファイル名
#define INITIAL_MONEY 1000            // 初期所持金
#define NAME_LENGTH 10                // ユーザー名の最大長
#define MAX_USERS 10                 // 最大ユーザー数

// ユーザーデータを管理する構造体
typedef struct {
    char name[NAME_LENGTH];
    long money;
} UserData;

// プロトタイプ宣言
void saveData(UserData users[], int userCount);
int loadData(UserData users[]);
UserData* selectUser(UserData users[], int* userCount);
void showRanking(UserData users[], int userCount);
int compareUsersByMoney(const void* a, const void* b); // qsort用比較関数

void playGame_Baccarat(UserData* user);
void playGame_Blackjack(UserData* user);
void playGame_Poker(UserData* user);
void playGame_Roulette(UserData* user);
void playGame_Chinchiro(UserData* user);


int main(void) {
    UserData users[MAX_USERS]; // 全ユーザーデータ配列
    int userCount = 0;         // 現在のユーザー数
    UserData* currentUser = NULL; // 現在ログイン中のユーザー

    int choice = 0;

    // 全ユーザーデータの読み込み
    userCount = loadData(users);
    printf("現在 %d 名のユーザーが登録されています。\n\n", userCount);

    // ユーザー選択 (ログイン または 新規登録)
    currentUser = selectUser(users, &userCount);
    if (currentUser == NULL) {
        printf("ユーザー選択がキャンセルされました。プログラムを終了します。\n");
        return 1;
    }

    printf("\nようこそ、%sさん！ ゲームをお楽しみください。\n", currentUser->name);

    // 3. メインループ (ゲーム選択)
    while (1) {
        printf("\n--- カジノへようこそ (%s) ---\n", currentUser->name);
        printf("現在の所持金: %ld 円\n", currentUser->money);
        printf("プレイするゲームを選んでください:\n");
        printf("1: バカラ\n");
        printf("2: ブラックジャック\n");
        printf("3: ポーカー\n");
        printf("4: ルーレット\n");
        printf("5: チンチロ\n");
        printf("6: ランキング表示\n");
        printf("0: ゲームを終了してデータを保存\n");
        printf("----------------------------------\n");
        printf("番号を入力してください: ");

        if (scanf("%d", &choice) != 1) {
            printf("無効な入力です。数字を入力してください。\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        printf("\n");

        switch (choice) {
        case 1:
            playGame_Baccarat(currentUser);
            break;
        case 2:
            playGame_Blackjack(currentUser);
            break;
        case 3:
            playGame_Poker(currentUser);
            break;
        case 4:
            playGame_Roulette(currentUser);
            break;
        case 5:
            playGame_Chinchiro(currentUser);
            break;
        case 6:
            showRanking(users, userCount);
            break;
        case 0:
            printf("データを保存して終了します。\n");
            saveData(users, userCount); // 全ユーザーデータを保存
            printf("ありがとうございました！またのお越しをお待ちしております。\n");
            exit(0);
        default:
            printf("無効な番号です。0から6の数字を入力してください。\n");
            break;
        }

        // 所持金がマイナスになった場合の処理 (破産)
        if (currentUser->money < 0) {
            printf("%s さんは破産しました！\n", currentUser->name);
            printf("所持金を %ld 円にリセットします。\n", INITIAL_MONEY);
            currentUser->money = INITIAL_MONEY;
        }

        printf("\n----------------------------------\n"); // 区切り線
    }

    return 0;
}


// 全ユーザーデータをファイルに保存する
// users 保存するデータが入ったUserData配列
// userCount 保存するユーザー数

void saveData(UserData users[], int userCount) {
    FILE* fp;

    fp = fopen(DATA_FILE_NAME, "w");
    if (fp == NULL) {
        printf("エラー: ファイル %s を開けませんでした。\n", DATA_FILE_NAME);
        return;
    }

    // 全ユーザーのデータを書き込む
    for (int i = 0; i < userCount; i++) {
        fprintf(fp, "%s\n", users[i].name);
        fprintf(fp, "%ld\n", users[i].money);
    }

    fclose(fp);
}

// 全ユーザーデータをファイルから読み込む
// users 読み込んだデータを入れるUserData配列
int loadData(UserData users[]) {
    FILE* fp;
    int i = 0; // ユーザー数カウンタ

    fp = fopen(DATA_FILE_NAME, "r");

    if (fp == NULL) {
        // ファイルが存在しない (初回起動時など)
        printf("データファイル %s が見つかりません。新規に作成します。\n", DATA_FILE_NAME);
        return 0; // ユーザー数 0 を返す
    }

    // ファイルの終端に達するまで、または最大ユーザー数に達するまで読み込む
    while (i < MAX_USERS) {
        // 1行目 (名前) を読み込む
        if (fscanf(fp, "%s", users[i].name) != 1) {
            break; // 読み込めなければループ終了 (ファイル終端)
        }

        // 2行目 (所持金) を読み込む
        if (fscanf(fp, "%ld", &(users[i].money)) != 1) {
            printf("エラー: %s さんの所持金データが不正です。\n", users[i].name);
            break; // データが不正ならループ終了
        }

        i++; // ユーザー数をインクリメント
    }

    fclose(fp);
    return i;
}

// ユーザーにログインまたは新規登録を選択させる
// users 全ユーザーデータ配列
// userCount 現在のユーザー数へのポインタ (新規登録時に更新するため)
// UserData* ログイン/新規登録したユーザーへのポインタ
UserData* selectUser(UserData users[], int* userCount) {
    int choice = 0;
    char tempName[NAME_LENGTH];

    while (1) {
        printf("--- ユーザー選択 ---\n");
        printf("1: 既存ユーザーでログイン\n");
        printf("2: 新規ユーザー登録\n");
        printf("0: 終了\n");
        printf("番号を入力してください: ");

        if (scanf("%d", &choice) != 1) {
            printf("無効な入力です。\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n'); // バッファクリア

        switch (choice) {
        case 1: // ログイン
            if (*userCount == 0) {
                printf("まだ誰も登録されていません。新規登録を選んでください。\n\n");
                continue;
            }

            printf("あなたの名前を入力してください (英数字): ");
            if (scanf("%9s", tempName) != 1) { // 9文字までに制限 (バッファオーバーフロー対策)
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');

            // ユーザー検索
            for (int i = 0; i < *userCount; i++) {
                if (strcmp(users[i].name, tempName) == 0) {
                    printf("ログイン成功: %sさん\n", users[i].name);
                    return &users[i]; // 見つかったユーザーのポインタを返す
                }
            }
            printf("ユーザー '%s' が見つかりません。\n\n", tempName);
            break;

        case 2: // 新規登録
            if (*userCount >= MAX_USERS) {
                printf("登録ユーザー数が上限 (%d人) に達しています。\n\n", MAX_USERS);
                continue;
            }

            printf("新しい名前を入力してください (英数字・スペース不可): ");
            if (scanf("%49s", tempName) != 1) {
                while (getchar() != '\n');
                continue;
            }
            while (getchar() != '\n');

            // 名前重複チェック
            int nameExists = 0;
            for (int i = 0; i < *userCount; i++) {
                if (strcmp(users[i].name, tempName) == 0) {
                    nameExists = 1;
                    break;
                }
            }
            if (nameExists) {
                printf("その名前 '%s' は既に使用されています。\n\n", tempName);
                continue;
            }

            // 新規ユーザーを配列の末尾に追加
            strcpy(users[*userCount].name, tempName);
            users[*userCount].money = INITIAL_MONEY;

            printf("新規ユーザー '%s' さんを登録しました。(所持金: %ld)\n", users[*userCount].name, users[*userCount].money);

            UserData* newUser = &users[*userCount]; // 新しいユーザーのポインタ
            (*userCount)++; // ユーザー数を増やす

            // 新規登録後、すぐにファイルに保存する
            saveData(users, *userCount);
            printf("データを保存しました。\n");

            return newUser; // 新規ユーザーのポインタを返す

        case 0: // 終了
            return NULL; // NULLを返してメイン関数側で終了処理

        default:
            printf("無効な選択です。\n\n");
            break;
        }
    }
}

// 所持金ランキングを表示する
// users 全ユーザーデータ配列
// userCount 現在のユーザー数
void showRanking(UserData users[], int userCount) {
    if (userCount == 0) {
        printf("まだ誰も登録されていません。\n");
        return;
    }

    // 元の配列を壊さないよう、ランキング表示用の配列にコピーする
    UserData rankingUsers[MAX_USERS];
    memcpy(rankingUsers, users, sizeof(UserData) * userCount);

    // qsortを使って所持金の降順にソートする
    // `qsort(ソート対象配列, 要素数, 要素1つのサイズ, 比較関数)`
    qsort(rankingUsers, userCount, sizeof(UserData), compareUsersByMoney);

    // 結果の表示
    printf("--- 所持金ランキング ---\n");
    for (int i = 0; i < userCount; i++) {
        printf("%2d位: %-20s ( %ld 円 )\n",
            i + 1,
            rankingUsers[i].name,
            rankingUsers[i].money);
        // 左詰め表示
    }
    printf("------------------------\n");
}

// qsortのための比較関数 (所持金の降順)
int compareUsersByMoney(const void* a, const void* b) {
    UserData* userA = (UserData*)a;
    UserData* userB = (UserData*)b;

    if (userB->money > userA->money) {
        return 1;
    }
    else if (userB->money < userA->money) {
        return -1;
    }
    else {
        return 0;
    }
}


// 各ゲームのダミー関数
void playGame_Baccarat(UserData* user) {
    printf("[バカラをプレイします (現在は準備中です)]\n");
    // テスト用: 所持金をランダムに増減させる
    int bet = 100;
    if (user->money < bet) {
        printf("所持金が足りません！\n"); return;
    }
    if (rand() % 2 == 0) {
        user->money += bet;
        printf("%s は %d 円勝ちました！ (所持金: %ld)\n", user->name, bet, user->money);
    }
    else {
        user->money -= bet;
        printf("%s は %d 円負けました... (所持金: %ld)\n", user->name, bet, user->money);
    }
}
void playGame_Blackjack(UserData* user) { printf("[ブラックジャックをプレイします (現在は準備中です)]\n"); }
void playGame_Poker(UserData* user) { printf("[ポーカーをプレイします (現在は準備中です)]\n"); }
void playGame_Roulette(UserData* user) { printf("[ルーレットをプレイします (現在は準備中です)]\n"); }
void playGame_Chinchiro(UserData* user) { printf("[チンチロをプレイします (現在は準備中です)]\n"); }