#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "jjuggumi.h"
#include "canvas.h"

#define DIALOG_DURATION_SEC		4

void draw(void);
void print_status(void);
void print_status2(void);

// (zero-base) row행, col열로 커서 이동
void gotoxy(int row, int col) {
	COORD pos = { col,row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

// row행, col열에 ch 출력
void printxy(char ch, int row, int col) {
	gotoxy(row, col);
	printf("%c", ch);
}

void map_init(int n_row, int n_col) {
	// 두 버퍼를를 완전히 비우기
	for (int i = 0; i < ROW_MAX; i++) {
		for (int j = 0; j < COL_MAX; j++) {
			back_buf[i][j] = front_buf[i][j] = ' ';
		}
	}

	N_ROW = n_row;
	N_COL = n_col;
	for (int i = 0; i < N_ROW; i++) {
		// 대입문 이렇게 쓸 수 있는데 일부러 안 가르쳐줬음
		back_buf[i][0] = back_buf[i][N_COL - 1] = '#';

		for (int j = 1; j < N_COL - 1; j++) {
			back_buf[i][j] = (i == 0 || i == N_ROW - 1) ? '#' : ' ';
		}
	}
}

// back_buf[row][col]이 이동할 수 있는 자리인지 확인하는 함수
bool placable(int row, int col) {
	if (row < 0 || row >= N_ROW ||
		col < 0 || col >= N_COL ||
		back_buf[row][col] != ' ') {
		return false;
	}
	return true;
}

// 상단에 맵을, 하단에는 현재 상태를 출력
void display(void) {
	draw();
	gotoxy(N_ROW + 4, 0); // 추가로 표시할 정보가 있으면 맵과 상태창 사이의 빈 공간에 출력
	print_status();
}

void display2(void) {
	draw();
	gotoxy(N_ROW + 4, 0);
	print_status2();
	gotoxy(N_ROW + 4, 0); // 추가로 표시할 정보가 있으면 맵과 상태창 사이의 빈 공간에 출력
	print_status();
}

void draw(void) {
	for (int row = 0; row < N_ROW; row++) {
		for (int col = 0; col < N_COL; col++) {
			if (front_buf[row][col] != back_buf[row][col]) {
				front_buf[row][col] = back_buf[row][col];
				printxy(front_buf[row][col], row, col);
			}
		}
	}
}

void print_status(void) {
	printf("no. of players left: %d\n", n_alive);
	printf("\t\t\tintl\tstr\tstm\n");
	for (int p = 0; p < n_player; p++) {
		if (win[p] == true) {
			printf("player %2d: %5s\t%d\t%d\t%d%\t\n", p, win[p] ? "Win" : "false",player[p].intel,player[p].str,player[p].stamina);
		}
		else {
			printf("player %2d: %5s\t%d\t%d\t%d%\t\n", p, player[p].is_alive ? "alive" : "DEAD", player[p].intel, player[p].str, player[p].stamina);
		}
	}
}
void print_status2(void) {
	printf("no. of players left: %d\n", n_alive);
	printf("\t\t\tintl\tstr\tstm\n");
	for (int p = 0; p < n_player; p++) {
		printf("                                                                                 \n");
	}
}

void dialog(char message[]) {
	int remaining_time = DIALOG_DURATION_SEC;

	while (remaining_time >= 0) {

		int message_row = 5;
		int message_col = 5;

		// 화면 출력
		gotoxy(message_row, message_col);
		printf("****************************************");
		gotoxy(message_row + 1, message_col);
		printf("% s", message);
		printf(" %d ", remaining_time);
		gotoxy(message_row + 2, message_col);
		printf("****************************************");

		Sleep(1000); 
		remaining_time--;

	}
}
//킬로그표시
void killdialog(char message[]) {
	
	int message_row = 5;
	int message_col = 5;

	// 화면 출력
	gotoxy(message_row, message_col);
	printf("****************************************");
	gotoxy(message_row + 1, message_col);
	printf("     % s",message);
	gotoxy(message_row + 2, message_col);
	printf("****************************************");

	Sleep(3000);

	gotoxy(message_row, message_col);
	printf("                                        ");
	gotoxy(message_row + 1, message_col);
	printf("                                        ");
	gotoxy(message_row + 2, message_col);
	printf("                                        ");

}
