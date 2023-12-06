#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void jebi_init(void);
//void move_manualjebi(key_t key);
//void move_tailjebi(int player, int nx, int ny);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기

void jebi_init(void) {
	map_init(7, (n_alive * 3) + 4);
	int x, y;
	for (int i = 0; i < n_alive; i++) {

		x = 3;
		y = 3 + (i * 3);
		if (i == 0) {
			back_buf[x][y] = '@';
		}
		else {
			back_buf[x][y] = '?';  // (0 .. n_player-1)
		}
	}
	tick = 0;
}
/*
void move_manualjebi(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;	
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_tailjebi(0, nx, ny);
}

// back_buf[][]에 기록
void move_tailjebi(int player, int nx, int ny) {
	int p = player;
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = '?';
	px[p] = nx;
	py[p] = ny;
}
*/
void jebi(void) {
	//testcode
	for (int i = 0; i < n_player; i++) {
		player[i].is_alive = TRUE;
	}
	jebi_init();
	system("cls");
	display();
	while (1) {
		key_t key = get_key();
		if (key == K_QUIT) {
			break;
		}
		else if (key != K_UNDEFINED) {
			//move_manualjebi(key);
		}
		display();
		Sleep(10);
		tick += 10;
	}
}