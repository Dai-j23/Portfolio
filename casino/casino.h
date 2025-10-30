#pragma once
#ifndef CASINO_H
#define CASINO_H

// 共通ヘッダファイル
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <windows.h>

// 定義
#define DATA_FILE_NAME "userdata.txt"
#define INITIAL_MONEY 1000
#define NAME_LENGTH 10
#define MAX_USERS 10

// 構造体定義
typedef struct {
    char name[NAME_LENGTH];
    long money;
    int bankruptCount;
} UserData;

// プロトタイプ宣言
// main.c に実装されている関数
void saveData(UserData users[], int userCount);
int loadData(UserData users[]);
UserData* selectUser(UserData users[], int* userCount);
void showRanking(UserData users[], int userCount);
int compareUsersByMoney(const void* a, const void* b);
long getBet(long maxBet); // 共通の掛け金入力関数

// 各ゲームの関数 (cファイルがないものはmain.cにダミーを配置)
void playGame_Baccarat(UserData* user);
void playGame_Blackjack(UserData* user);
void playGame_Poker(UserData* user);
void playGame_Roulette(UserData* user);
void playGame_Chinchiro(UserData* user);//OK

#endif // CASINO_H