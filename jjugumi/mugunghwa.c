#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>

#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void mugung_init(void);
void move_manual(key_t key, int t);
void move_random(int i, int dir);
void move_tail(int i, int nx, int ny);
void kill_tail(int i, int nx, int ny);
void kill_false(int i);
bool getlife(int i);
bool get_safezone(int i,int nx, int ny);
void safe_tail(int player, int nx, int ny);
void safe_win();

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // �� �÷��̾� ��ġ, �̵� �ֱ�
int diedplayer[PLAYER_MAX];
int safeplayer[PLAYER_MAX];
int winplayer[PLAYER_MAX];
int s_num = 0;
int livecount;


void say_mugung(int time) {

	if (time == 500) {
		gotoxy(17, 0);
		printf("��                                                        ");
	}
	if (time == 600) {
		gotoxy(17, 0);
		printf("�� ��                                                     ");
	}
	if (time == 750) {
		gotoxy(17, 0);
		printf("�� �� ȭ                                                  ");
	}
	if (time == 950) {
		gotoxy(17, 0);
		printf("�� �� ȭ ��                                               ");
	}
	if (time == 1200) {
		gotoxy(17, 0);
		printf("�� �� ȭ �� ��                                            ");
	}
	if (time == 1500) {
		gotoxy(17, 0);
		printf("�� �� ȭ �� ��  ��                                        ");
	}
	if (time == 1800) {
		gotoxy(17, 0);
		printf("�� �� ȭ �� ��  �� ��                                     ");
	}
	if (time == 2050) {
		gotoxy(17, 0);
		printf("�� �� ȭ �� ��  �� �� ��                                  ");
	}
	if (time == 2250) {
		gotoxy(17, 0);
		printf("�� �� ȭ �� ��  �� �� �� ��                               ");
	}
	if (time == 2450) {
		gotoxy(17, 0);
		printf("�� �� ȭ �� ��  �� �� �� �� ��!                           ");
		back_buf[(N_ROW / 2) - 1][1] = '#';
		back_buf[(N_ROW / 2)][1] = '#'; 
		back_buf[(N_ROW / 2) + 1][1] = '#';
	}
	if (time == 4000) {
		back_buf[(N_ROW / 2) - 1][1] = '@';
		back_buf[(N_ROW / 2)][1] = '@';
		back_buf[(N_ROW / 2) + 1][1] = '@';
		gotoxy(17, 0);
		printf("                                                          ");
	}
}

void mugung_init(void) {
	map_init(15, 50);
	int x, y;

	back_buf[(N_ROW / 2) - 1][1] = '@';
	back_buf[(N_ROW / 2)][1] = '@';
	back_buf[(N_ROW / 2) + 1][1] = '@';

	
	printf("%d", n_player);
	for (int i = 0; i < n_player; i++) {
		// ���� �ڸ��� ������ �ٽ� ����
		do {
			x = randint(1, N_ROW - 2);
			y = 48;
		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 500);

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
		x++;
	}

	tick = 0;
	
}

void safe_win() {
	for (int i = 0; i < n_player; i++) {
		if (getlife(i) == true) {
			int new_x = px[i];
			int new_y = py[i];

			if (back_buf[new_x][new_y - 1] == '@' || back_buf[new_x][new_y + 1] == '@' ||
				back_buf[new_x - 1][new_y] == '@' || back_buf[new_x + 1][new_y] == '@') {

				safeplayer[s_num] = i;
				s_num++;
				safe_tail(i, px[i], py[i]);
			}
		}
	}	
}
void mugung_set(void) {
	map_init(15, 50);

	back_buf[(N_ROW / 2) - 1][1] = '@';
	back_buf[(N_ROW / 2)][1] = '@';
	back_buf[(N_ROW / 2) + 1][1] = '@';
}

void move_manual(key_t key,int tick) {
	// �� �������� ������ �� x, y�� delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	bool life;
	life = getlife(0);

	int dir;  // ������ ����(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// �������� ���� �ڸ�
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];

	//���������Ǹ� �����̰� ����
	bool safezone;
	safezone = get_safezone(0, nx, ny);

	if (!placable(nx, ny)) {
		return;
	}
	if (life == true) {
		if (tick < 2450 && tick >= 500) {
			move_tail(0, nx, ny);
		}
		else if (tick >= 2450) {
			
			if (safezone == false) {
				kill_tail(0, nx, ny);
				char fullMessage[100] = "";
				sprintf_s(fullMessage, sizeof(fullMessage), " %d player, 0 dead! ", n_alive);
				killdialog(fullMessage);
			}
			else {
				move_tail(0, nx, ny);
			}
		}
	}
}

bool getlife(int p) {
	if (player[p].is_alive == true) {
		return true;
	}
	else {
		return false;
	}
}

bool get_safezone(int p,int nx,int ny) {
	int safe_buf[ROW_MAX][COL_MAX] = { 0 }; // �迭 �ʱ�ȭ
	int move_buf[ROW_MAX][COL_MAX] = { 0 }; // �迭 �ʱ�ȭ
	move_buf[nx][ny] = 1;

	for (int i = 0; i < n_player; i++) {
		if (player[i].is_alive == true) {
			for (int j = 1; j < 15; j++) {
				for (int k = py[i]; k < 50; k++) {
					if (j == px[i]) {
						safe_buf[j][k] = 1;
					}
				}
			}
		}
		
	}

	if (move_buf[nx][ny] == safe_buf[nx][ny]){
		return true;
	}
	return false;
}

// 0 <= dir < 4�� �ƴϸ� ����
void move_random(int player, int dir) {
	int p = player;  // �̸��� ��...
	int nx, ny;  // �������� ������ ���� �ڸ�
	bool life;
	life = getlife(p);

	// ������ ������ ���� ���� ���ٰ� ����(���� ������ ����)	
	if (life == true) {
		do {
			int random_num = randint(1, 10);
			
			if (random_num == 1) {
				// 10% Ȯ���� ������ ����
				nx = px[p];
				ny = py[p];
			}
			else if (random_num == 2) {
				// 10% Ȯ���� ���� �̵�
				nx = px[p] - 1;
				ny = py[p];
			}
			else if (random_num == 3) {
				// 10% Ȯ���� �Ʒ��� �̵�
				nx = px[p] + 1;
				ny = py[p];
			}
			else {
				// ������ ��쿡�� �������� �̵�
				nx = px[p];
				ny = py[p] - 1;
			}
		} while (!placable(nx, ny));

		move_tail(p, nx, ny);
	}
	
}

//10���� Ȯ���θ� ������
void move_stop(int player, int dir) {
	int p = player;
	int nx, ny;
	int kill = 0;
	bool life = true;
	life = getlife(p);

	if (life == true) {
		int random_num = randint(1, 10);
		if (random_num == 1) {
			int random_num2 = randint(1, 10);
			if (random_num2 == 1) {
				// 10% Ȯ���� ���� �̵�
				nx = px[p] - 1;
				ny = py[p];
			}
			else if (random_num2 == 2) {
				// 10% Ȯ���� �Ʒ��� �̵�
				nx = px[p] + 1;
				ny = py[p];
			}
			else {
				// ������ ��쿡�� �������� �̵�
				nx = px[p];
				ny = py[p] - 1;
			}
			kill = 1;
		}
		else {
			// ������ ��쿡�� ����
			nx = px[p];
			ny = py[p];
		}

		bool safezone;
		safezone = get_safezone(p, nx, ny);

		if (placable(nx, ny)) {
			if (kill == 1) {
				if (safezone == true) {
					move_tail(p, nx, ny);
				}
				else {
					kill_tail(p, nx, ny);
				}
			}
			else {
				move_tail(p, nx, ny);
			}
		}
	}
	
}

// back_buf[][]�� �������
void kill_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = ' ';
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
	kill_false(p);
}

// back_buf[][]�� ���
void move_tail(int player, int nx, int ny) {
	int p = player;  // �̸��� ��...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}
//������ false�� �ٲ��ִ� �Լ�
void kill_false(int p) {
	player[p].is_alive = false;
	n_alive--;
	diedplayer[p] = p;
}
void safe_tail(int p, int nx, int ny) {
	back_buf[nx][ny] = ' ';
	player[p].is_alive = false;
	win[p] = true;
	livecount++;
}

void mugunghwa() {
	mugung_set();
	display();
	dialog("����ȭ���� �Ǿ����ϴٸ� �����մϴ�.");
	system("cls");
	mugung_init();
	display();
	//test 
	while (1) {
		if (n_alive == 1) {
			win[0] = true;
			break;
		}
		if (n_alive == livecount) {
			break;
		}
		// player 0�� ������ ������(4����)
		key_t key = get_key();
		if (key == K_QUIT) {
			for (int i = 0; i < n_player; i++) {
				if (player[i].is_alive == true) {
					win[i] = true;
				}
			}
			break;
		}
		else if (key != K_UNDEFINED) {
			move_manual(key, tick);
		}
		
		// player 1 ���ʹ� �������� ������(8����)
		if (tick < 2450 && tick >= 500) {
			for (int i = 1; i < n_player; i++) {
				if (safeplayer[i] != i) {
					if (period[i] != 0) {
						if (tick % period[i] == 0) {
							move_random(i, -1);
						}
						else if (tick % period[i] == 1) {
							move_random(i, -1);
						}
						else if (tick % period[i] == 2) {
								move_random(i, -1);
						}
						else if (tick % period[i] == 3) {
							move_random(i, -1);
						}
						else if (tick % period[i] == 4) {
							move_random(i, -1);
						}
					}
				}
			}
		}
			
			
		else if (tick >= 2460) {

			char deadPlayersMessage[100] = "";
			int numDeadPlayers = 0;

			for (int i = 1; i < n_player; i++) {
				if (diedplayer[i] != 0) {
					if (numDeadPlayers > 0) {
						strcat_s(deadPlayersMessage, sizeof(deadPlayersMessage), ", ");
					}

					char playerNumber[10];
					if (_itoa_s(diedplayer[i], playerNumber, sizeof(playerNumber), 10) != 0) {
						// ���� ó�� (��ȯ ����)
					}

					strcat_s(deadPlayersMessage, sizeof(deadPlayersMessage), playerNumber);
					numDeadPlayers++;

					diedplayer[i] = 0; // ��������� �ʱ�ȭ
				}
			}

			if (numDeadPlayers > 0) {
				char fullMessage[100] = "";
				sprintf_s(fullMessage, sizeof(fullMessage), "%d player, %s dead! ", n_alive, deadPlayersMessage);
				killdialog(fullMessage);
			}
		}

		else if (tick == 2450) {
			for (int i = 0; i < n_player; i++) {
				if (safeplayer[i] != i) {
					move_stop(i, 0);
				}
			}
		}

		safe_win();

		say_mugung(tick);
		display();
		Sleep(10);
		tick += 10;

		if (tick > 4000) {
			tick = 500;
		}
	}

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

	/*for (int i = 0; i < n_player; i++) {
		if (win[i] == true) {
			char playerNumber[10];
			sprintf_s(playerNumber, sizeof(playerNumber), "%d", i); // ������ ���ڿ��� ��ȯ
			strcat_s(messeage, sizeof(messeage), playerNumber);
			strcat_s(messeage, sizeof(messeage), ", "); // ��ǥ�� ������ �߰�
		}
	}
	if (strlen(messeage) > 0) {
		messeage[strlen(messeage) - 2] = '\0';
	}*/
}