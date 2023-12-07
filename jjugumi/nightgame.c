#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
#include <stdlib.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void controlsituation(int i, ITEM item, bool changeitem);
void cleardialog(int t);
void changeitem(int a, int b);
void choosen(int n,int p,int p2);
void choose(int p,int p2);
void changeplayeritem(int p, int p2);
int distance(int x1, int y1, int x2, int y2);
int finditem(int x, int y);
void getitem(void);
void get_zero_item(void);
void itemidalog(int p);
void night_init(void);
void move_manualn(key_t key);
void move_randomn(int i);
void move_toward_target(int player, int target_x, int target_y);
void move_tailn(int i, int nx, int ny);
void playingdialog(int n, int p1, int p2);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기

void night_init(void) {
	map_init(15, 50);
	int x, y;

	for (int i = 0; i < n_item; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		item[i].x = x;
		item[i].y = y;
		back_buf[item[i].x][item[i].y] = 'I';  // 아이템 위치 생성
	}

	for (int i = 0; i < n_player; i++) {
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);
			y = randint(1, N_COL - 2);
		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);
		if (win[i] == TRUE) {
			win[i] = FALSE;
			player[i].is_alive = TRUE;
			back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
		}
	}
	
	tick = 0;
	tick2 = 0;
}

int distance(int x1, int y1, int x2, int y2) {
	return abs(x1 - x2) + abs(y1 - y2);
}

void move_manualn(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_tailn(0, nx, ny);
}

void move_toward_target(int player, int target_x, int target_y) {
	int p = player;
	int nx, ny;

	// 타겟 좌표로부터 각 방향에 대한 거리 계산
	int dx[4] = { -1, 1, 0, 0 };
	int dy[4] = { 0, 0, -1, 1 };

	// 현재 플레이어의 위치
	int current_x = px[p];
	int current_y = py[p];

	int min_distance = INT_MAX;  // 최소 거리 초기화
	int preferred_dir = -1;  // 선호하는 방향 초기화

	// 각 방향에 대해 타겟과의 거리 계산
	for (int dir = 0; dir < 4; dir++) {
		nx = current_x + dx[dir];
		ny = current_y + dy[dir];

		// 이동 가능한지 확인
		if (placable(nx, ny)) {
			int dist = distance(nx, ny, target_x, target_y);

			// 거리가 더 가까우면 갱신
			if (dist < min_distance) {
				min_distance = dist;
				preferred_dir = dir;
			}
		}
	}

	// 이동
	if (preferred_dir != -1) {
		nx = current_x + dx[preferred_dir];
		ny = current_y + dy[preferred_dir];
		move_tailn(p, nx, ny);
	}
}

void move_randomn(int p) {
	int di = -1,dp = -1;
	int distitem = INT_MAX, distplayer = INT_MAX;

	// 모든 아이템에 대해 가장 가까운 아이템 찾기
	for (int i = 0; i < n_item; i++) {
		if (item[i].x != 0 && item[i].y != 0) {
			int dist = distance(px[p], py[p], item[i].x, item[i].y);
			if (dist < distitem) {
				distitem = dist;
				di = i;
			}
		}
	}

	//move_toward_target(p, item[di].x, item[di].y);
	// 모든 플레이어에 대해 가장 가까운 플레이어 찾기
	for (int i = 0; i < n_player; i++) {
		if (player[i].hasitem && player[i].is_alive) {
			int dist = distance(px[p], py[p], px[i], py[i]);
			if (dist < distplayer) {
				distplayer = dist;
				dp = i;
			}
		}
	}
	if (dp != -1) {
		if (distitem > distplayer) {
			move_toward_target(p, px[dp], py[dp]);
			return;
		}
		else if (distitem < distplayer) {
			move_toward_target(p, item[di].x, item[di].y);
			return;
		}
	}
	else {
		move_toward_target(p, item[di].x, item[di].y);
		return;
	}

}

int finditem(int x, int y) {

	for (int i = 0; i < n_item; i++) {
		if (item[i].x == x && item[i].y == y) {
			return i;
		}
	}
	return -1; //못찾으면 -1 retrun
}
// back_buf[][]에 기록
void move_tailn(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}
//인근 아이템 줍거나 패스하기
void get_zero_item(void) {

	if (player[0].hasitem == FALSE) {
		int new_x = px[0];
		int new_y = py[0];
		int item_num;
		if (back_buf[new_x][new_y - 1] == 'I' || back_buf[new_x][new_y + 1] == 'I' ||
			back_buf[new_x - 1][new_y] == 'I' || back_buf[new_x + 1][new_y] == 'I') {

			if (back_buf[new_x][new_y - 1] == 'I') {
				item_num = finditem(new_x, new_y - 1);
				if (item_num != -1) {
					player[0].item = item[item_num];
					strcpy_s(item[item_num].name, sizeof(item[item_num].name), " ");
					item[item_num].x = 0;
					item[item_num].y = 0;
					player[0].hasitem = TRUE;
					back_buf[new_x][new_y - 1] = ' ';
					controlsituation(0,item[item_num],FALSE);
					itemidalog(0);
				}
			}
			else if (back_buf[new_x][new_y + 1] == 'I') {
				item_num = finditem(new_x, new_y + 1);
				if (item_num != -1) {
					player[0].item = item[item_num];
					strcpy_s(item[item_num].name, sizeof(item[item_num].name), " ");
					item[item_num].x = 0;
					item[item_num].y = 0;
					player[0].hasitem = TRUE;
					back_buf[new_x][new_y + 1] = ' ';
					controlsituation(0, item[item_num], FALSE);
					itemidalog(0);
				}
			}
			else if (back_buf[new_x - 1][new_y] == 'I') {
				item_num = finditem(new_x - 1, new_y);
				if (item_num != -1) {
					player[0].item = item[item_num];
					strcpy_s(item[item_num].name, sizeof(item[item_num].name), " ");
					item[item_num].x = 0;
					item[item_num].y = 0;
					player[0].hasitem = TRUE;
					back_buf[new_x - 1][new_y] = ' ';
					controlsituation(0, item[item_num], FALSE);
					itemidalog(0);
				}
			}
			else {
				item_num = finditem(new_x + 1, new_y);
				if (item_num != -1) {
					player[0].item = item[item_num];
					strcpy_s(item[item_num].name, sizeof(item[item_num].name), " ");
					item[item_num].x = 0;
					item[item_num].y = 0;
					player[0].hasitem = TRUE;
					back_buf[new_x + 1][new_y] = ' ';
					controlsituation(0, item[item_num], FALSE);
					itemidalog(0);
				}
			}

		}
	}
	if (player[0].hasitem == TRUE) {
		int new_x = px[0];
		int new_y = py[0];
		int item_num;
		if (back_buf[new_x][new_y - 1] == 'I' || back_buf[new_x][new_y + 1] == 'I' ||
			back_buf[new_x - 1][new_y] == 'I' || back_buf[new_x + 1][new_y] == 'I') {
			char yesorno;
			gotoxy(17, 0);
			printf("                                                              ");
			gotoxy(17, 0);
			printf("아이템을 주우시겠습니까?(y/n) : ");
			scanf_s(" %c", &yesorno);
			gotoxy(17, 0);
			printf("                                                              ");
			if (yesorno == 'y') {

				if (back_buf[new_x][new_y - 1] == 'I') {
					item_num = finditem(new_x, new_y - 1);
					if (item_num != -1) {
						changeitem(0,item_num);
						back_buf[new_x][new_y - 1] = 'I';
						controlsituation(0, item[item_num], TRUE);
						itemidalog(0);
					}
				}
				else if (back_buf[new_x][new_y + 1] == 'I') {
					item_num = finditem(new_x, new_y + 1);
					if (item_num != -1) {
						changeitem(0,item_num);
						back_buf[new_x][new_y + 1] = 'I';
						controlsituation(0, item[item_num], TRUE);
						itemidalog(0);
					}
				}
				else if (back_buf[new_x - 1][new_y] == 'I') {
					item_num = finditem(new_x - 1, new_y);
					if (item_num != -1) {
						changeitem(0,item_num);
						back_buf[new_x - 1][new_y] = 'I';
						controlsituation(0, item[item_num], TRUE);
						itemidalog(0);
					}
				}
				else {
					item_num = finditem(new_x + 1, new_y);
					if (item_num != -1) {
						changeitem(0,item_num);
						back_buf[new_x + 1][new_y] = 'I';
						controlsituation(0, item[item_num], TRUE);
						itemidalog(0);
					}
				}

			}
		}
	}
}

void getitem(void) {
	for (int i = 1; i < n_player; i++) {
		if (player[i].is_alive == TRUE) {
			if (player[i].hasitem == FALSE) {
					int new_x = px[i];
					int new_y = py[i];
					int item_num;
					if (back_buf[new_x][new_y - 1] == 'I' || back_buf[new_x][new_y + 1] == 'I' ||
						back_buf[new_x - 1][new_y] == 'I' || back_buf[new_x + 1][new_y] == 'I') {

						if (back_buf[new_x][new_y - 1] == 'I') {
							item_num = finditem(new_x, new_y - 1);
							if (item_num != -1) {
								player[i].item = item[item_num];
								player[i].hasitem = TRUE;
								back_buf[new_x][new_y - 1] = ' ';
								controlsituation(i, item[item_num], FALSE);

							}
						}
						else if (back_buf[new_x][new_y + 1] == 'I') {
							item_num = finditem(new_x, new_y + 1);
							if (item_num != -1) {
								player[i].item = item[item_num];
								player[i].hasitem = TRUE;
								back_buf[new_x][new_y + 1] = ' ';
								controlsituation(i, item[item_num], FALSE);
								itemidalog(i);
							}
						}
						else if (back_buf[new_x - 1][new_y] == 'I') {
							item_num = finditem(new_x - 1, new_y);
							if (item_num != -1) {
								player[i].item = item[item_num];
								player[i].hasitem = TRUE;
								back_buf[new_x - 1][new_y] = ' ';
								controlsituation(i, item[item_num], FALSE);
								itemidalog(i);
							}
						}
						else {
							item_num = finditem(new_x + 1, new_y);
							if (item_num != -1) {
								player[i].item = item[item_num];
								player[i].hasitem = TRUE;
								back_buf[new_x + 1][new_y] = ' ';
								controlsituation(i, item[item_num], FALSE);
								itemidalog(i);
							}
						}

					}
				}
			else {
					int rand = randint(0, 1);
					if (rand == 1) {
						int new_x = px[i];
						int new_y = py[i];
						int item_num;

						if (back_buf[new_x][new_y - 1] == 'I' || back_buf[new_x][new_y + 1] == 'I' ||
							back_buf[new_x - 1][new_y] == 'I' || back_buf[new_x + 1][new_y] == 'I') {

							if (back_buf[new_x][new_y - 1] == 'I') {
								item_num = finditem(new_x, new_y - 1);
								if (item_num != -1) {
									changeitem(i, item_num);
									back_buf[new_x][new_y - 1] = 'I';
									controlsituation(i, item[item_num], TRUE);
									itemidalog(i);
								}
							}
							else if (back_buf[new_x][new_y + 1] == 'I') {
								item_num = finditem(new_x, new_y + 1);
								if (item_num != -1) {
									changeitem(i, item_num);
									back_buf[new_x][new_y + 1] = 'I';
									controlsituation(i, item[item_num], TRUE);
									itemidalog(i);
								}
							}
							else if (back_buf[new_x - 1][new_y] == 'I') {
								item_num = finditem(new_x - 1, new_y);
								if (item_num != -1) {
									changeitem(i, item_num);
									back_buf[new_x - 1][new_y] = 'I';
									controlsituation(i, item[item_num], TRUE);
									itemidalog(i);
								}
							}
							else {
								item_num = finditem(new_x + 1, new_y);
								if (item_num != -1) {
									changeitem(i, item_num);
									back_buf[new_x + 1][new_y] = 'I';
									controlsituation(i, item[item_num], TRUE);
									itemidalog(i);
								}
							}
						}
					}
				}
		}
	}
}

void changeitem(int p, int i) {
	ITEM tempitem;

	strcpy_s(tempitem.name, sizeof(tempitem.name), player[p].item.name);
	tempitem.intel_buf = player[p].item.intel_buf;
	tempitem.stamina_buf = player[p].item.stamina_buf;
	tempitem.str_buf = player[p].item.str_buf;

	strcpy_s(player[p].item.name, sizeof(player[p].item.name), item[i].name);
	player[p].item.intel_buf = item[i].intel_buf;
	player[p].item.stamina_buf = item[i].stamina_buf;
	player[p].item.str_buf = item[i].str_buf;

	strcpy_s(item[i].name, sizeof(item[i].name), tempitem.name);
	item[i].intel_buf = tempitem.intel_buf;
	item[i].stamina_buf = tempitem.stamina_buf;
	item[i].str_buf = tempitem.str_buf;
}

void changeplayeritem(int p, int p2) {
	ITEM tempitem;

	strcpy_s(tempitem.name, sizeof(tempitem.name), player[p].item.name);
	tempitem.intel_buf = player[p].item.intel_buf;
	tempitem.stamina_buf = player[p].item.stamina_buf;
	tempitem.str_buf = player[p].item.str_buf;

	strcpy_s(player[p].item.name, sizeof(player[p].item.name), player[p2].item.name);
	player[p].item.intel_buf = player[p2].item.intel_buf;
	player[p].item.stamina_buf = player[p2].item.stamina_buf;
	player[p].item.str_buf = player[p2].item.str_buf;

	strcpy_s(player[p2].item.name, sizeof(player[p2].item.name), tempitem.name);
	player[p2].item.intel_buf = tempitem.intel_buf;
	player[p2].item.stamina_buf = tempitem.stamina_buf;
	player[p2].item.str_buf = tempitem.str_buf;
}

void choosen(int n,int p,int p2){
	double stemina=0;
;	if (n == 1) {
		if (player[p].stamina > 0) {
			if (player[p].str > player[p2].str) {
				changeplayeritem(p, p2);
				controlsituation(p, player[p2].item, TRUE);
				controlsituation(p2, player[p].item, TRUE);
				stemina = player[p].stamina;
				stemina = stemina * 0.6;
				player[p].stamina = (int)stemina;
				playingdialog(1, p, p2);
			}
			else {
				stemina = player[p].stamina;
				stemina = stemina * 0.4;
				player[p].stamina = (int)stemina;
				playingdialog(3, p, p2);
			}
		}
	}
	else if (n == 2) {
	if (player[p].stamina > 0) {
		if (player[p].intel > player[p2].intel) {
			changeplayeritem(p, p2);
			controlsituation(p, player[p2].item, TRUE);
			controlsituation(p2, player[p].item, TRUE);
			stemina = player[p].stamina;
			stemina = stemina * 0.8;
			player[p].stamina = (int)stemina;
			playingdialog(2, p, p2);
		}
		else {
			stemina = player[p].stamina;
			stemina = stemina * 0.6;
			player[p].stamina = (int)stemina;
			playingdialog(4, p, p2);
		}
	}
	}
	else {
		return;
	}
}

void choose(int p,int p2) {
	int num;
	while (1) {
		gotoxy(17, 0);
		printf("                                                               ");
		gotoxy(17, 0);
		printf("1.강탈시도 2.회유시도 3.무시하기 : ");
		scanf_s("%d", &num);
		gotoxy(17, 0);
		printf("                                                               ");
		if (num == 1) {
			choosen(1,p,p2);
			return;
		}
		else if (num == 2) {
			choosen(2,p,p2);
			return;
		}
		else if (num == 3) {
			choosen(3,p,p2);
			return;
		}
	}
}

void playerfight(int p) {
	for (int i = 0; i < n_player; i++) {
		int new_x = px[p];
		int new_y = py[p];
		if (back_buf[new_x][new_y - 1] == '0' + i || back_buf[new_x][new_y + 1] == '0' + i ||
			back_buf[new_x - 1][new_y] == '0' + i || back_buf[new_x + 1][new_y] == '0' + i) {

			if (back_buf[new_x][new_y - 1] == '0' + i) {
				if (p != 0) {
					if (player[p].hasitem == TRUE && player[i].hasitem == TRUE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}

					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == TRUE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == TRUE && player[i].hasitem == FALSE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == FALSE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}
					}
				}
				else {
					choose(p, i);
					return;
				}
			}
			else if (back_buf[new_x][new_y + 1] == '0' + i) {
				if (p != 0) {
					if (player[p].hasitem == TRUE && player[i].hasitem == TRUE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}

					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == TRUE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == TRUE && player[i].hasitem == FALSE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == FALSE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}
					}
				}
				else {
					choose(p, i);
					return;
				}
			}
			else if (back_buf[new_x - 1][new_y] == '0' + i) {
				if (p != 0) {
					if (player[p].hasitem == TRUE && player[i].hasitem == TRUE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}

					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == TRUE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == TRUE && player[i].hasitem == FALSE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == FALSE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}
					}
				}
				else {
					choose(p, i);
					return;
				}
			}
			else {
				if (p != 0) {
					if (player[p].hasitem == TRUE && player[i].hasitem == TRUE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}

					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == TRUE) {
						choosen(randint(1, 3), p, i);
						return;
					}
					else if (player[p].hasitem == TRUE && player[i].hasitem == FALSE) {
						choosen(randint(1, 3), i, p);
						return;
					}
					else if (player[p].hasitem == FALSE && player[i].hasitem == FALSE) {
						int t = randint(0, 1);
						if (t == 0) {
							choosen(randint(1, 3), p, i);
							return;
						}
						else {
							choosen(randint(1, 3), i, p);
							return;
						}
					}
				}
				else {
					choose(p, i);
					return;
				}
			}
		}
	}
}

void itemidalog(int p){
	gotoxy(17, 0);
	printf("                                                               ");
	gotoxy(17, 0);
	printf("%d번 플레이어가 %s 아이템을 주웠습니다.", p, player[p].item.name);
	tick2 = 0;
}

void playingdialog(int n,int p1, int p2) {
	gotoxy(17, 0);
	printf("                                                               ");
	gotoxy(17, 0);
	if (n == 1) {
		printf("%d번 플레이어가 %d번 아이템을 강탈(교환)했습니다.", p1, p2);
	}
	if (n == 2) {
		printf("%d번 플레이어가 %d번 아이템을 회유(교환)했습니다.", p1, p2); 
	}
	if (n == 3) {
		printf("%d번 플레이어가 %d번 아이템을 강탈(교환)에 실패했습니다.", p1, p2);
	}
	if (n == 4) {
		printf("%d번 플레이어가 %d번 아이템을 회유(교환)에 실패했습니다.", p1, p2);
	}
	
	tick2 = 0;
}

void cleardialog(int t){
	if (t >= 2000) {
		gotoxy(17, 0);
		printf("                                                                                    ");
		tick2 = 0;
	}
}

void controlsituation(int i,ITEM item,bool changeitem) {
	if (changeitem == FALSE) {
		if (player[i].is_alive == TRUE) {
			if (player[i].hasitem == TRUE) {
				player[i].intel = player[i].intel + item.intel_buf;
				player[i].str = player[i].str + item.str_buf;
				player[i].stamina = player[i].stamina + item.stamina_buf;
				if (player[i].stamina > 100) {
					player[i].stamina = 100;
				}
			}
		}
	}
	else {
		if (player[i].is_alive == TRUE) {
			if (player[i].hasitem == TRUE) {
				player[i].intel = player[i].intel - player[i].item.intel_buf;
				player[i].str = player[i].str - player[i].item.str_buf;
				player[i].stamina = player[i].stamina - player[i].item.stamina_buf;

				if (player[i].stamina > 100) {
					player[i].stamina = 100;
				}

				player[i].intel = player[i].intel + item.intel_buf;
				player[i].str = player[i].str + item.str_buf;
				player[i].stamina = player[i].stamina + item.stamina_buf;

				if (player[i].stamina > 100) {
					player[i].stamina = 100;
				}
			}
		}
	}
	
}

void nightgame(void) {
	night_init();
	dialog("야간운동을 시작합니다.");
	system("cls");
	display();
	while (1) {

		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (key == K_QUIT) {
			for (int i = 0; i < n_player; i++) {
				if (player[i].is_alive == TRUE) {
					double stemina = 0;
					stemina = player[i].stamina;
					stemina = stemina * 1.5;
					player[i].stamina = (int)stemina;
					if (player[i].stamina > 100) {
						player[i].stamina = 100;
					}
				}
			}
			break;
		}
		else if (key != K_UNDEFINED) {
			
			move_manualn(key);
			get_zero_item();
			playerfight(0);
		}

		// player 1 부터는 랜덤으로 움직임(8방향)
		for (int i = 1; i < n_player; i++) {
			if (tick % period[i] == 0) {
				move_randomn(i);
				getitem();
				playerfight(i);
			}
		}

		display2();
		Sleep(10);
		tick += 10;
		tick2 += 10;
		cleardialog(tick2);
		
	}
}