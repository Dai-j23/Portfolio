#include "casino.h"

 // 役の定義
#define YAKU_PINZORO 10 // ピンゾロ (1,1,1) (即勝ち)
#define YAKU_ARASHI   9 // アラシ (ゾロ目) (即勝ち)
#define YAKU_SHIGORO  8 // シゴロ (4,5,6) (即勝ち)
#define YAKU_ME       7 // 目（数字） (1〜6)
#define YAKU_HIFUMI   6 // ヒフミ (1,2,3) (即負け)
#define YAKU_MENASHI  0 // 目なし

// 役判定結果を格納する構造体
typedef struct {
    int yakuType;
    int yakuValue; // 役の強さ (アラシなら 2〜6, 目なら 1〜6)
} RollResult;


// プロトタイプ宣言
static void sortDice(int dice[3]);
static RollResult checkRoll(int dice[3]);
static void printYakuName(RollResult result);
static void printDice(int dice[3]);
static RollResult playerTurn(UserData* user);
static RollResult aiTurn(RollResult playerYaku);
static long calculatePayout(RollResult playerYaku, RollResult aiYaku, long bet);
static void rollDice(int dice[3]);

// サイコロの目をソートする (役判定のため)
static void sortDice(int dice[3]) {
    int i, j, temp;
    for (i = 0; i < 2; i++) {
        for (j = i + 1; j < 3; j++) {
            if (dice[i] > dice[j]) {
                temp = dice[i];
                dice[i] = dice[j];
                dice[j] = temp;
            }
        }
    }
}

// 役を判定する
static RollResult checkRoll(int dice[3]) {
    RollResult result;
    int d[3]; // ソート用にコピー
    memcpy(d, dice, sizeof(int) * 3);
    sortDice(d);

    if (d[0] == 1 && d[1] == 1 && d[2] == 1) { // ピンゾロ
        result.yakuType = YAKU_PINZORO;
        result.yakuValue = 1;
    }
    else if (d[0] == d[1] && d[1] == d[2]) { // アラシ
        result.yakuType = YAKU_ARASHI;
        result.yakuValue = d[0]; // 2〜6
    }
    else if (d[0] == 4 && d[1] == 5 && d[2] == 6) { // シゴロ
        result.yakuType = YAKU_SHIGORO;
        result.yakuValue = 0;
    }
    else if (d[0] == 1 && d[1] == 2 && d[2] == 3) { // ヒフミ
        result.yakuType = YAKU_HIFUMI;
        result.yakuValue = 0;
    }
    else if (d[0] == d[1]) { // 目 (例: 1,1,5 -> 5)
        result.yakuType = YAKU_ME;
        result.yakuValue = d[2];
    }
    else if (d[1] == d[2]) { // 目 (例: 1,5,5 -> 1)
        result.yakuType = YAKU_ME;
        result.yakuValue = d[0];
    }
    else { // 目なし
        result.yakuType = YAKU_MENASHI;
        result.yakuValue = 0;
    }
    return result;
}

// 役の名前の表示
static void printYakuName(RollResult result) {
    switch (result.yakuType) {
    case YAKU_PINZORO: printf(" >> 役: ピンゾロ (1,1,1)！\n"); break;
    case YAKU_ARASHI: printf(" >> 役: アラシ (%d, %d, %d)！\n", result.yakuValue, result.yakuValue, result.yakuValue); break;
    case YAKU_SHIGORO: printf(" >> 役: シゴロ (4,5,6)！\n"); break;
    case YAKU_HIFUMI: printf(" >> 役: ヒフミ (1,2,3)...\n"); break;
    case YAKU_ME: printf(" >> 役: [ %d ] の目\n", result.yakuValue); break;
    case YAKU_MENASHI: printf(" >> 役: 目なし\n"); break;
    }
}

// サイコロの目を表示する
static void printDice(int dice[3]) {
    printf("[ %d ] [ %d ] [ %d ]\n", dice[0], dice[1], dice[2]);
}

// サイコロを3つ振る
static void rollDice(int dice[3]) {
    dice[0] = (rand() % 6) + 1;
    dice[1] = (rand() % 6) + 1;
    dice[2] = (rand() % 6) + 1;
}

// プレイヤーのターン
static RollResult playerTurn(UserData* user) {
    int dice[3];
    RollResult yaku = { YAKU_MENASHI, 0 };
    int rollCount = 0;
    char choice;

    printf("\n--- %s のターン (子) ---\n", user->name);

    while (rollCount < 3) {
        rollCount++;
        printf("【%d 回目】サイコロを振ります... (エンター)", rollCount);
        while (getchar() != '\n');

        rollDice(dice);
        printDice(dice);
        yaku = checkRoll(dice);
        printYakuName(yaku);

        // 強い役 (ピンゾロ、アラシ、シゴロ) または 負け役 (ヒフミ) が出たら即確定
        if (yaku.yakuType == YAKU_PINZORO || yaku.yakuType == YAKU_ARASHI ||
            yaku.yakuType == YAKU_SHIGORO || yaku.yakuType == YAKU_HIFUMI) {
            printf(" >> 役が確定しました。\n");
            return yaku;
        }

        // 3回目でなければ、振り直すか聞く
        if (rollCount < 3) {
            if (yaku.yakuType == YAKU_ME) {
                // 「目」が出た場合
                printf("この目 [ %d ] で勝負しますか？ (y/n): ", yaku.yakuValue);
                if (scanf(" %c", &choice) != 1) choice = 'n';
                while (getchar() != '\n');
                if (choice == 'y' || choice == 'Y') {
                    printf(" >> [ %d ] の目で勝負します。\n", yaku.yakuValue);
                    return yaku; // この目で確定
                }
            }
            else {
                // 「目なし」の場合
                printf(" >> 目なし。振り直します。\n");
            }
        }
    }

    // 3回振った場合
    printf(" >> 3回振り終わりました。\n");
    return yaku;
}

// AIのターン
static RollResult aiTurn(RollResult playerYaku) {
    int dice[3];
    RollResult aiYaku = { YAKU_MENASHI, 0 };
    int rollCount = 0;

    printf("\n--- AI のターン (親) ---\n");

    while (rollCount < 3) {
        rollCount++;
        printf("【%d 回目】AIがサイコロを振ります...\n", rollCount);
        Sleep(500); // AIが考えてる風の待機

        rollDice(dice);
        printDice(dice);
        aiYaku = checkRoll(dice);
        printYakuName(aiYaku);

        // 「目」が出た場合も即確定
        if (aiYaku.yakuType != YAKU_MENASHI) {

            // 役名を分かりやすく表示
            if (aiYaku.yakuType == YAKU_ME) {
                printf(" >> AIの役が [ %d の目 ] に確定しました。\n", aiYaku.yakuValue);
            }
            else {
                printf(" >> AIの役が確定しました。\n");
            }
            return aiYaku; // 役または目が出たので終了
        }

        // 目なしの場合 (振り直し)
        if (rollCount < 3) {
            printf("AI: (目なし... 振り直します)\n");
        }
        Sleep(500); // 待機
    }

    // 3回振った場合
    printf(" >> AIは3回振り終わりました。\n");
    return aiYaku; // 3回目が目なしでも、それが結果
}

// プレイヤーとAIの役を比較し、払い戻し額を計算する
static long calculatePayout(RollResult playerYaku, RollResult aiYaku, long bet) {
    long payout = 0;

    printf("\n--- 勝敗判定 ---\n");
    printf("プレイヤー: "); printYakuName(playerYaku);
    printf("AI (親)   : "); printYakuName(aiYaku);
    printf("----------------\n");

    // プレイヤーの即勝ち/即負け判定
    if (playerYaku.yakuType == YAKU_PINZORO) {
        printf("ピンゾロ！ プレイヤーの5倍勝ちです！\n");
        return bet * 5;
    }
    if (playerYaku.yakuType == YAKU_HIFUMI) {
        printf("ヒフミ... プレイヤーの2倍負けです。\n");
        return -(bet * 2);
    }

    // --- AIの即勝ち/即負け判定 ---
    if (aiYaku.yakuType == YAKU_PINZORO) {
        printf("AIがピンゾロ！ プレイヤーの5倍負けです！\n");
        return -(bet * 5);
    }
    if (aiYaku.yakuType == YAKU_HIFUMI) {
        printf("AIがヒフミ！ プレイヤーの2倍勝ちです！\n");
        return bet * 2;
    }

    // --- 役の強さ比較 (アラシ、シゴロ、目、目なし) ---

    // プレイヤーがアラシ
    if (playerYaku.yakuType == YAKU_ARASHI) {
        if (aiYaku.yakuType == YAKU_ARASHI && aiYaku.yakuValue > playerYaku.yakuValue) {
            printf("AIのアラシが強い... プレイヤーの3倍負けです。\n");
            return -(bet * 3);
        }
        else {
            printf("アラシ！ プレイヤーの3倍勝ちです！\n");
            return bet * 3;
        }
    }
    // AIがアラシ
    if (aiYaku.yakuType == YAKU_ARASHI) {
        printf("AIがアラシ！ プレイヤーの3倍負けです。\n");
        return -(bet * 3);
    }

    // プレイヤーがシゴロ
    if (playerYaku.yakuType == YAKU_SHIGORO) {
        if (aiYaku.yakuType == YAKU_SHIGORO) {
            printf("シゴロ同士！ 引き分けです。\n");
            return 0;
        }
        else {
            printf("シゴロ！ プレイヤーの2倍勝ちです！\n");
            return bet * 2;
        }
    }
    // AIがシゴロ
    if (aiYaku.yakuType == YAKU_SHIGORO) {
        printf("AIがシゴロ！ プレイヤーの2倍負けです。\n");
        return -(bet * 2);
    }

    // プレイヤーが「目」
    if (playerYaku.yakuType == YAKU_ME) {
        if (aiYaku.yakuType == YAKU_ME) {
            // お互い「目」
            if (playerYaku.yakuValue > aiYaku.yakuValue) {
                printf("プレイヤーの目 [ %d ] の勝ち！ (1倍勝ち)\n", playerYaku.yakuValue);
                return bet;
            }
            else if (playerYaku.yakuValue < aiYaku.yakuValue) {
                printf("AIの目 [ %d ] の勝ち... (1倍負け)\n", aiYaku.yakuValue);
                return -bet;
            }
            else {
                printf("目が同じ！ 引き分けです。\n");
                return 0;
            }
        }
        else if (aiYaku.yakuType == YAKU_MENASHI) {
            // AIが「目なし」
            printf("プレイヤーの目 [ %d ] の勝ち！ (1倍勝ち)\n", playerYaku.yakuValue);
            return bet;
        }
    }

    // プレイヤーが「目なし」
    if (playerYaku.yakuType == YAKU_MENASHI) {
        if (aiYaku.yakuType == YAKU_ME) {
            // AIが「目」
            printf("AIの目 [ %d ] の勝ち... (1倍負け)\n", aiYaku.yakuValue);
            return -bet;
        }
        else if (aiYaku.yakuType == YAKU_MENASHI) {
            // お互い「目なし」
            printf("お互い目なし！ 引き分けです。\n");
            return 0;
        }
    }

    printf("判定エラー: 不明な勝敗です。引き分けとします。\n");
    return 0;
}


// チンチロのゲームロジック
void playGame_Chinchiro(UserData* user) {
    long bet;
    long payout;
    char continueGame;
    RollResult playerYaku;
    // AIが振らない場合に備えて、デフォルトの「目なし」で初期化
    RollResult aiYaku = { YAKU_MENASHI, 0 };

    printf("--- チンチロへようこそ ---\n");
    printf("親(AI)と3回までのサイコロ勝負をします。\n");
    printf("プレイヤーがピンゾロ・シゴロ・ヒフミを出した場合、即座に勝敗が決定します。\n");


    while (1) {
        printf("\n現在の所持金: %ld 円\n", user->money);

        if (user->money <= 0) {
            printf("所持金がありません！\n");
            break;
        }

        // 1. 掛け金を決める
        bet = getBet(user->money);
        if (bet == 0) {
            break; // ゲーム選択に戻る
        }
        printf("%ld 円を賭けました。\n", bet);

        // 2. プレイヤーのターン
        playerYaku = playerTurn(user);

        // プレイヤーの役に応じてAIのターンをスキップ
        if (playerYaku.yakuType == YAKU_PINZORO ||
            playerYaku.yakuType == YAKU_SHIGORO ||
            playerYaku.yakuType == YAKU_HIFUMI) {

            printf("\n--- プレイヤーの役が即時決着のため、AIは振りません ---\n");
            aiYaku.yakuType = YAKU_MENASHI;
            aiYaku.yakuValue = 0;

        }
        else {
            // プレイヤーが「アラシ」「目」「目なし」の場合のみAIが振る
            aiYaku = aiTurn(playerYaku);
        }

        // 4. 払い戻し
        payout = calculatePayout(playerYaku, aiYaku, bet);
        user->money += payout;

        // 結果表示
        if (payout > 0) {
            printf("\n結果: %ld 円獲得しました！\n", payout);
        }
        else if (payout < 0) {
            printf("\n結果: %ld 円失いました...\n", -payout);
        }
        else {
            printf("\n結果: 引き分け (所持金変動なし)\n");
        }
        printf("現在の所持金: %ld 円\n", user->money);

        printf("\nもう一度プレイしますか？ (y/n): ");
        if (scanf(" %c", &continueGame) != 1) {
            continueGame = 'n';
        }
        while (getchar() != '\n');

        if (continueGame != 'y' && continueGame != 'Y') {
            printf("チンチロを終了します。\n");
            break;
        }
    }
}