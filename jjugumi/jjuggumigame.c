// 2023-2 고급프로그래밍 과제: 쭈꾸미 게임
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include "jjuggumi.h"
#include <errno.h>

#define	DATA_FILE "C:\\Users\\sojaeyeong\\source\\repos\\advprograming2\\jjugumi\\jjuggumi.dat" 
//일반 파일에 적으면 데이터가 잡히지 않아서 임의로 설정해두었습니다 
//파일 내 링크로 수정해주시면 됩니다.

char messeage[100];
int jjuggumi_init(void);

void ending(char mes[]);
// low 이상 high 이하 난수를 발생시키는 함수
int randint(int low, int high) {
	int rnum = rand() % (high - low + 1) + low;
	return rnum;
}

int jjuggumi_init(void) {
	srand((unsigned int)time(NULL));
	extern int errno;
	FILE* fp;
	fopen_s(&fp, DATA_FILE, "r");
	if (fp == NULL) {
		printf("Error opening the file: %d \n",errno);
		return -1;
	}
	// 플레이어데이터load
	fscanf_s(fp, "%d", &n_player);
	n_alive = n_player;
	for (int i= 0; i< n_player; i++) {
		PLAYER* p = &player[i];// 파일에서각스탯최댓값읽기
		fscanf_s(fp, "%s%d%d%d",p->name, (unsigned int)sizeof(p->name),&(p->intel), &(p->str),&(p->stamina));
		//printf("%s\n",player[i].name);
		p->stamina = 100; // 100%
		
		// 현재상태
		p->is_alive= true;
		p->hasitem= false;
	}
	
	// 아이템데이터load
	fscanf_s(fp, "%d", &n_item);
	printf("%d",n_item);
	for (int i= 0; i< n_item; i++) {
		fscanf_s(fp, "%s%d%d%d",item[i].name, (unsigned int)sizeof(item[i].name),&(item[i].intel_buf),&(item[i].str_buf),&(item[i].stamina_buf));
		//printf("%s,%d,%d,%d\n", item[i].name, item[i].intel_buf, item[i].str_buf, item[i].stamina_buf);
	}

	fclose(fp);
	return 0;
}

void intro() {
	system("cls");
	printf("                        ___\n");
	printf("                     .-'   `'.\n");
	printf("                    /         \\\n");
	printf("                    |         ;\n");
	printf("                    |         |           ___.--,\n");
	printf("           _.._     |0) ~ (0) |    _.---'`__.-( (_.\n");
	printf("    __.--'`_.. '.__.\\    '--. \\_.-' ,.--'`     `\"\"\"\n");
	printf("   ( ,.--'`   ',__ /./;  ;, '.__.'`    __\n");
	printf("  _`) )  .---.__.' / |  |\\   \\__..--\"\"  \"\"\"--.,_\n");
	printf("  `---' .'.''-._.-'`_./  /\\ '.  \\ _.-~~~````~~~-._`-.__.'\n");
	printf("        | |  .' _.-' |  |  \\  \\  '.               `~---`\n");
	printf("         \\ \\/ .'     \\  \\   '. '-._)\n");
	printf("          \\/ /        \\  \\    `=__`~-.\n");
	printf("          / /\\         `) )    / / `\"\".`\\\n");
	printf("    , _.-'.'\\ \\        / /    ( (     / /\n");
	printf("     `--~`   ) )    .-'.'      '.'.  | (\n");
	printf("            (/`    ( (`          ) )  '-;\n");
	printf("             `      '-;         (-'\n");

	
	Sleep(2000); 
	
	printf("          *********************************\n");
	printf("          쭈꾸미 게임에 오신 걸 환영합니다.\n");
	printf("          *********************************\n");
	Sleep(3000);

	system("cls");
}
void ending(char messeage[]) {
	system("cls");
	printf("          *********************************\n");
	printf("           %s 번 플레이어가 승리하였습니다.\n",messeage);
	printf("          *********************************\n");
	Sleep(3000);

}
int main(void) {
	jjuggumi_init();
	intro();
	mugunghwa();
	nightgame();
	//juldarigi();
	//jebi();
	ending(messeage);
	return 0;
}
