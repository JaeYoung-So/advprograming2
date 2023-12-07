#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3
#define DIR_SPACE	4

void jebi_init(void);
void move_manualjebi(key_t key);
void move_tailjebi(int player, int nx, int ny);
bool placable2(int row, int col);
void checkpass(int p);
void killlog(int i);
void passlog(int i);
void live_init(void);

int px[PLAYER_MAX], py[PLAYER_MAX], now=0,death;  // 각 플레이어 위치
bool pass[PLAYER_MAX];
int diedplayer[PLAYER_MAX];
int pp=0; //playingplayer
int round=1;
int passjebi;

void jebi_init(void) {
	map_init(7, (n_alive * 3) + 4);
	int x, y;
	passjebi = n_alive;
	death = randint(0,n_alive-1);
	for (int i = 0; i < n_alive; i++) {
		if (i == death) {
			pass[i] = FALSE;
		}
		else {
			pass[i] = TRUE;
		}
	}
	for (int i = 0; i < n_alive; i++) {
		
		x = 3;
		y = 3 + (i * 3);
		if (i == 0) {
			px[0] = x;
			py[0] = y;
			back_buf[px[0]][py[0]] = '@';
		}
		else {
			px[i] = x;
			py[i] = y;
			back_buf[px[i]][py[i]] = '?';
		}
	}
	
	tick = 0;
}
void live_init(void) {
	map_init(7, (n_alive * 3) + 4);
	int x, y;
	for (int i = 0; i < passjebi; i++) {

		x = 3;
		y = 3 + (i * 3);
		if (i == 0) {
			px[0] = x;
			py[0] = y;
			back_buf[px[0]][py[0]] = '@';
		}
		else {
			px[i] = x;
			py[i] = y;
			back_buf[px[i]][py[i]] = '?';
		}
	}
}
void death_init(void) {
	map_init(7, (n_alive * 3) + 4);
	int x, y;
	if (n_alive > 1) {
		death = randint(0, n_alive - 1);
	}
	for (int i = 0; i < n_alive; i++) {
		if (i == death) {
			pass[i] = FALSE;
		}
		else {
			pass[i] = TRUE;
		}
	}
	for (int i = 0; i < n_alive; i++) {

		x = 3;
		y = 3 + (i * 3);
		if (i == 0) {
			px[0] = x;
			py[0] = y;
			back_buf[px[0]][py[0]] = '@';
		}
		else {
			px[i] = x;
			py[i] = y;
			back_buf[px[i]][py[i]] = '?';
		}
	}

	tick = 0;
}
void move_manualjebi(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dy[4] = { 0, 0, -3, 3 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_SPACE: dir = DIR_SPACE; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	if (dir != 4) {
		// 움직여서 놓일 자리
		int nx, ny;
		nx = px[0];
		ny = py[0] + dy[dir];
		if (!placable2(nx, ny)) {
			return;
		}
		if (dir == 2) {
			now--;
		}
		if (dir == 3) {
			now++;
		}
		move_tailjebi(0, nx, ny);
	}
	else {
		checkpass(pp);
	}
	
}
// back_buf[row][col]이 이동할 수 있는 자리인지 확인하는 함수
bool placable2(int row, int col) {
	if (row < 0 || row >= N_ROW ||
		col < 0 || col >= N_COL ||
		back_buf[row][col] != '?') {
		return false;
	}
	return true;
}
// back_buf[][]에 기록
void move_tailjebi(int player, int nx, int ny) {
	int p = player;
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = '?';
	px[p] = nx;
	py[p] = ny;
}

void checkpass(int p) {
	if (pass[now] == FALSE) {
		killlog(p);
		player[p].is_alive = FALSE;
		now = 0;
		n_alive--;
		if (player[0].is_alive == TRUE) {
			pp = 0;
		}
		else {
			for (int i = 0; i < n_player; i++) {
				if (player[i].is_alive == TRUE) {
					pp = i;
					break;
				}
			}
		}
		round++;
		passjebi = n_alive;
		system("cls");
		death_init();
	}
	else {
		passlog(p);
		now = 0;
		passjebi--;
		if (passjebi <= death) {
			pass[death] = TRUE;
			death--;
			pass[death] = FALSE;
		}
		do {
			pp++;
		} while (player[pp].is_alive != TRUE);
		system("cls");
		live_init();
	}
}

void killlog(int i){

	int message_row = 2;
	int message_col = 0;

	// 화면 출력
	gotoxy(message_row, message_col);
	printf("*****************************");
	gotoxy(message_row + 1, message_col);
	printf("    %d player %d fail!", n_alive, i);
	gotoxy(message_row + 2, message_col);
	printf("*****************************");

	Sleep(3000);

	gotoxy(message_row, message_col);
	printf("                                        ");
	gotoxy(message_row + 1, message_col);
	printf("                                        ");
	gotoxy(message_row + 2, message_col);
	printf("                                        ");

}

void passlog(int i){
	int message_row = 2;
	int message_col = 0;

	// 화면 출력
	gotoxy(message_row, message_col);
	printf("*****************************");
	gotoxy(message_row + 1, message_col);
	printf("    %d player %d pass!", n_alive, i);
	gotoxy(message_row + 2, message_col);
	printf("*****************************");

	Sleep(3000);

	gotoxy(message_row, message_col);
	printf("                                        ");
	gotoxy(message_row + 1, message_col);
	printf("                                        ");
	gotoxy(message_row + 2, message_col);
	printf("                                        ");
}

void jebi(void) {

	jebi_init();
	system("cls");
	display();
	while (1) {
		int count=0;
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manualjebi(key);
		}

		display();
		gotoxy(N_ROW+1, 0);
		printf("%d round, turn : player %d", round, pp);
		
		for (int i = 0; i < n_player; i++) {
			if (player[i].is_alive == TRUE) {
				count++;
			}
		}
		if (count == 1) {
			return;
		}
		Sleep(10);
		tick += 10;
	}
}