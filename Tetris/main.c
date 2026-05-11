#include<stdio.h>
#include<Windows.h>
#include<stdlib.h>
#include<time.h>

#define Top 0
#define Bottom Top+ 25
#define left 3
#define right left + 12

#define next_block_left right + 5
#define next_block_right next_block_left + 6
#define next_block_top Top + 8
#define next_block_bottom next_block_top + 4

#define stored_block_left next_block_right + 5  
#define stored_block_right stored_block_left + 6
#define stored_block_top Top + 8
#define stored_block_bottom stored_block_top + 4

#define start_speed 500
#define guide_delay 200
#define left_right_delay 100
#define down_delay 50
#define fast_drop_delay 300
#define last_dance_delay 70
#define block_shape_change_delay 50
#define block_swap_delay 500
int normal_drop_delay = 500;

clock_t last_left_time = 0; clock_t last_right_time = 0; clock_t last_down_time = 0;
clock_t normal_drop_time = 0; clock_t fast_drop_time = 0; 
clock_t guide_time = 0; clock_t block_shape_change_start_time = 0; clock_t erase_time = 0; clock_t last_dance_time = 0;
clock_t block_swap_time = 0;

int block_shape_change_flag = 0;
int score_sum = 0; int stage_level = 1; 
int guide_cnt = 0; int guide_flag = 0; 
int game_over_flag = 1;
int stored_block = -1;
int block_swap_flag = 0;
int dif = 0;

int map[Bottom - Top + 1][stored_block_right - left + 2]; // 맵 배열 map[바닥 좌표(Bottom) - 천장 좌표(Top) + 바닥 벽(1)][오른쪽 끝 좌표(stored_block_right, 다음 블럭 표시) - 왼쪽 끝 좌표(left) + 좌우 벽(2)]

int blockcolor[10] = { 11,14,13,12,10,9,6 };
char guide[2] = { '.', ' ' };

// 블럭[종류][모양][세로][가로]
int block[7][4][4][4] = {
	{ {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}, {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}, {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}} }, // I
	{ {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}}, {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}}, {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}}, {{0,0,0,0},{0,2,2,0},{0,2,2,0},{0,0,0,0}} }, // O
	{ {{0,0,0,0},{0,3,3,3},{0,0,3,0},{0,0,0,0}}, {{0,0,3,0},{0,0,3,3},{0,0,3,0},{0,0,0,0}}, {{0,0,3,0},{0,3,3,3},{0,0,0,0},{0,0,0,0}}, {{0,0,3,0},{0,3,3,0},{0,0,3,0},{0,0,0,0}} }, // T
	{ {{0,0,0,0},{0,4,4,0},{0,0,4,4},{0,0,0,0}}, {{0,0,0,4},{0,0,4,4},{0,0,4,0},{0,0,0,0}}, {{0,0,0,0},{0,4,4,0},{0,0,4,4},{0,0,0,0}}, {{0,0,0,4},{0,0,4,4},{0,0,4,0},{0,0,0,0}} }, // Z
	{ {{0,0,0,0},{0,0,5,5},{0,5,5,0},{0,0,0,0}}, {{0,0,5,0},{0,0,5,5},{0,0,0,5},{0,0,0,0}}, {{0,0,0,0},{0,0,5,5},{0,5,5,0},{0,0,0,0}}, {{0,0,5,0},{0,0,5,5},{0,0,0,5},{0,0,0,0}} }, // S
	{ {{0,0,0,0},{0,6,6,6},{0,0,0,6},{0,0,0,0}}, {{0,0,6,6},{0,0,6,0},{0,0,6,0},{0,0,0,0}}, {{0,6,0,0},{0,6,6,6},{0,0,0,0},{0,0,0,0}}, {{0,0,6,0},{0,0,6,0},{0,6,6,0},{0,0,0,0}} }, // J
	{ {{0,0,0,0},{0,7,7,7},{0,7,0,0},{0,0,0,0}}, {{0,0,7,0},{0,0,7,0},{0,0,7,7},{0,0,0,0}}, {{0,0,0,7},{0,7,7,7},{0,0,0,0},{0,0,0,0}}, {{0,7,7,0},{0,0,7,0},{0,0,7,0},{0,0,0,0}} }  // L
};

// 색 설정
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 해당 좌표로 가기
void gotoxy(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

// 커서 안보이게 하기
void CursorView()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// T
void printT(int x, int y)		//T 출력
{
	gotoxy(x, y);   printf("▣▣▣▣▣▣▣▣");
	gotoxy(x, y + 1); printf("   ▣▣");
	gotoxy(x, y + 2); printf("   ▣▣");
	gotoxy(x, y + 3); printf("   ▣▣");
	gotoxy(x, y + 4); printf("   ▣▣");
	gotoxy(x, y + 5); printf("   ▣▣");
	gotoxy(x, y + 6); printf("   ▣▣");
	gotoxy(x, y + 7); printf("   ▣▣");
	gotoxy(x, y + 8); printf("   ▣▣");
}
// E
void printE(int x, int y)		//E 출력
{
	gotoxy(x, y);     printf("▣▣▣▣▣▣");
	gotoxy(x, y + 1); printf("▣▣        ");
	gotoxy(x, y + 2); printf("▣▣        ");
	gotoxy(x, y + 3); printf("▣▣        ");
	gotoxy(x, y + 4); printf("▣▣▣▣▣▣");
	gotoxy(x, y + 5); printf("▣▣        ");
	gotoxy(x, y + 6); printf("▣▣        ");
	gotoxy(x, y + 7); printf("▣▣        ");
	gotoxy(x, y + 8); printf("▣▣▣▣▣▣");
}
// R
void printR(int x, int y)		//R 출력
{
	gotoxy(x, y);     printf("▣▣▣▣    ");
	gotoxy(x, y + 1); printf("▣▣  ▣▣  ");
	gotoxy(x, y + 2); printf("▣▣   ▣▣ ");
	gotoxy(x, y + 3); printf("▣▣  ▣▣  ");
	gotoxy(x, y + 4); printf("▣▣▣▣    ");
	gotoxy(x, y + 5); printf("▣▣ ▣▣   ");
	gotoxy(x, y + 6); printf("▣▣  ▣▣  ");
	gotoxy(x, y + 7); printf("▣▣   ▣▣ ");
	gotoxy(x, y + 8); printf("▣▣    ▣▣");
}
// I
void printI(int x, int y)		//I 출력
{
	gotoxy(x, y);     printf("▣▣▣▣▣▣");
	gotoxy(x, y + 1); printf("  ▣▣");
	gotoxy(x, y + 2); printf("  ▣▣");
	gotoxy(x, y + 3); printf("  ▣▣");
	gotoxy(x, y + 4); printf("  ▣▣");
	gotoxy(x, y + 5); printf("  ▣▣");
	gotoxy(x, y + 6); printf("  ▣▣");
	gotoxy(x, y + 7); printf("  ▣▣");
	gotoxy(x, y + 8); printf("▣▣▣▣▣▣");
}
// S
void printS(int x, int y)		//S 출력
{
	gotoxy(x, y);     printf("  ▣▣▣▣▣");
	gotoxy(x, y + 1); printf(" ▣▣▣▣   ");
	gotoxy(x, y + 2); printf("▣▣▣      ");
	gotoxy(x, y + 3); printf(" ▣▣▣     ");
	gotoxy(x, y + 4); printf("   ▣▣▣   ");
	gotoxy(x, y + 5); printf("     ▣▣▣ ");
	gotoxy(x, y + 6); printf("      ▣▣▣");
	gotoxy(x, y + 7); printf("   ▣▣▣▣ ");
	gotoxy(x, y + 8); printf("▣▣▣▣▣  ");
}

//스타트 화면 
int start(void)
{
	int i;
	int j;
	CursorView();		//커서 숨기기 
	system("cls");		//화면 초기화 

	for (int i = 0; i <= 29; i++) {
		for (int j = 0; j <= 90; j++) {
			if (j == 0 || j == 90) {
				gotoxy(j, i);
				printf("■");
			}
			if (i == 29) {
				gotoxy(j, i);
				printf("■");
			}
			if (i == 0) {
				gotoxy(j, i);
				printf("■");
			}
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);		//색상 지정
	printT(5, 6);
	Sleep(200);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	printE(19, 6);
	Sleep(200);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
	printT(31, 6);
	Sleep(200);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	printR(45, 6);
	Sleep(200);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	printI(59, 6);
	Sleep(200);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	printS(73, 6);
	Sleep(200);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	gotoxy(10, 16);
	printf("                  난이도 선택 : 스페이스 바");

	return(Select());
}

// 난이도 설정
int Select(void) {
	int i = 0;
	int y = 19;

	gotoxy(35, 19);
	printf("Beginner");
	gotoxy(35, 21);
	printf("Amateur");
	gotoxy(35, 23);
	printf("Expert");
	gotoxy(35, 25);
	printf("Exit");
	gotoxy(28, 19);
	printf(">>");



	while (1) {
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
			if (y == 19) {
				for (i = 0; i < 5; i++)
				{
					gotoxy(35, 19);
					printf("        ");
					Sleep(50);
					gotoxy(35, 19);
					printf("Beginner");
					Sleep(50);
				}
				system("cls");
				return 1;
			}

			else if (y == 21)
			{

				for (i = 0; i < 5; i++)
				{
					gotoxy(35, 21);
					printf("       ");
					Sleep(50);
					gotoxy(35, 21);
					printf("Amateur");
					Sleep(50);
				}
				system("cls");
				return 2;
			}

			else if (y == 23)
			{


				for (i = 0; i < 5; i++)		//선택시 반짝거림 p
				{
					gotoxy(35, 23);
					printf("      ");
					Sleep(50);
					gotoxy(35, 23);
					printf("Expert");
					Sleep(50);
				}
				system("cls");
				return 3;
			}
			else
			{
				return 0;
			}

		}

		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			gotoxy(28, y);
			printf("  ");

			if (y != 19)
				y = y - 2;
			else
				y = 25;

			gotoxy(28, y);
			printf(">>");
			Sleep(100);
		}

		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			gotoxy(28, y);
			printf("  ");
			if (y != 25)
				y = y + 2;
			else
				y = 19;
			gotoxy(28, y);
			printf(">>");
			Sleep(100);
		}
	}
}

// 벽 생성
void wall() {
	for (int i = Top; i <= Bottom; i++) {
		for (int j = left; j <= right; j++) {
			if (j == left || j == right) {
				map[i][j] = -1;
			}
			if (i == Bottom) {
				map[i][j] = -2;
			}
			if (i == Top + 1) {
				if (left == j || j == right) {
					map[i][j] = -4;
				}
			}
		}
	}
}

// 게임 조작법
void info() {
	int x = right * 3, y = 5;
	gotoxy(x, y + 2); printf("┌  다음 블럭  ┐ ");
	gotoxy(x, y + 7); printf("└             ┘ ");
	gotoxy(right * 5 - 2, y + 2); printf("┌  블럭 스왑  ┐ ");
	gotoxy(right * 5 - 2, y + 7); printf("└             ┘ ");
	gotoxy(x, y + 9); printf(" 레벨 : %6d ", stage_level);
	if (dif == 1) { //초보
		gotoxy(x, y + 10); printf(" 점수 : %6d ", score_sum);
	}
	else if (dif == 2) { //중수
		gotoxy(x, y + 10); printf(" 점수 : %6d ", score_sum - 2000);
	}
	else if (dif == 3) { // 고수
		gotoxy(x, y + 10); printf(" 점수 : %6d ", score_sum - 4000);
	}
	gotoxy(x, y + 15); printf("  △   :  가이드 블럭    Space : 빠르게 블럭 내리기   Tab : 블럭 스왑");
	gotoxy(x, y + 16); printf("◁   ▷ : 왼쪽 / 오른쪽   Shift : 모양 바꾸기");
	gotoxy(x, y + 17); printf("  ▽   : 천천히 블럭 내리기     Esc : 종료");
}

// 다음 블럭 보이기
void draw_next_block(int next_random) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[next_random][0][i][j] == next_random + 1) {
				map[next_block_top + i][next_block_right / 2 + j - 2] = next_random + 1;
			}
		}
	}
}

// 다음 블럭 구역 지우기
void erase_next_block() {
	for (int i = next_block_top; i <= next_block_bottom; i++) {
		for (int j = next_block_left; j <= next_block_right; j++) {
			map[i][j] = 0;
		}
	}
}

// 블럭 스왑 보이기
void draw_stored_block(int stored_random) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[stored_random][0][i][j] == stored_random + 1) {
				map[stored_block_top + i][stored_block_right / 2 + j - 2] = stored_random + 1;
			}
		}
	}
}

// 블럭 스왑 구역 지우기
void erase_stored_block() {
	for (int i = stored_block_top; i <= stored_block_bottom; i++) {
		for (int j = stored_block_left; j <= stored_block_right; j++) {
			map[i][j] = 0;
		}
	}
}

// 맵 그리기
void DrawMap() {
	for (int i = Top; i <= Bottom; i++) {
		for (int j = left; j <= right; j++) {
			if (map[i][j] == -1) {
				setColor(15);
				gotoxy(j * 2, i);
				printf("■");
			}
			else if (map[i][j] == -2) {
				setColor(15);
				gotoxy(j * 2, i);
				printf("■");
			}
			else if (map[i][j] == -3) {
				setColor(15);
				gotoxy(j * 2, i);
				printf("□");
			}
			else if (map[i][j] == -4) {
				setColor(12);
				gotoxy(j * 2, i);
				if (j == left) {
					printf("▶");
				}
				else {
					printf("◀");
				}
				setColor(15);
			}
			else if (map[i][j] == 0) {
				setColor(8);
				gotoxy(j * 2, i);
				printf("%c", guide[guide_cnt % 2]);
				setColor(15);
			}
			else if (1 <= map[i][j] && map[i][j] <= 7) {
				setColor(blockcolor[map[i][j] - 1]);
				gotoxy(j * 2, i);
				printf("□");
				setColor(15);
			}
		}
	}

	for (int i = next_block_top; i <= next_block_bottom; i++) {
		for (int j = next_block_left; j <= next_block_right; j++) {
			if (map[i][j] == 0) {
				gotoxy(j * 2, i);
				printf(" ");
			}
			else if (1 <= map[i][j] && map[i][j] <= 7) {
				setColor(blockcolor[map[i][j] - 1]);
				gotoxy(j * 2, i);
				printf("□");
				setColor(15);
			}
		}
	}

	for (int i = stored_block_top; i <= stored_block_bottom; i++) {
		for (int j = stored_block_left; j <= stored_block_right; j++) {
			if (map[i][j] == 0) {
				gotoxy(j * 2, i);
				printf(" ");
			}
			else if (1 <= map[i][j] && map[i][j] <= 7) {
				setColor(blockcolor[map[i][j] - 1]);
				gotoxy(j * 2, i);
				printf("□");
				setColor(15);
			}
		}
	}
}

// 블럭 만들기
void create_block(int random, int cnt, int x, int y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[random][cnt][i][j] == random + 1) {
				map[y + i][x / 2 + j] = random + 1;
			}
		}
	}
}

// 이전 블럭 지우기
void erase_prev_block(int random, int cnt, int x, int y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[random][cnt][i][j] == random + 1) {
				map[y + i][x / 2 + j] = 0;
			}
		}
	}
}

// 다른 블럭이나 옆 쪽 벽 감지
int is_detected_block_sidewall(int random, int cnt, int x, int y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[random][cnt][i][j] == random + 1) {
				if (map[y + i][x / 2 + j] == -1 || map[y + i][x / 2 + j] == -3) { // 함수 호출 때 받은 x값이 x - 2, x + 2 므로 바로 옆이 옆 쪽 벽이거나 다른 블럭이면
					return 1;
				}
			}
		}
	}
	return 0;
}

// 다른 블럭이나 바닥 감지
int is_detected_block_bottomwall(int random, int cnt, int x, int y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[random][cnt][i][j] == random + 1) {
				if (map[y + i][x / 2 + j] == -2 || map[y + i][x / 2 + j] == -3) { // 함수 호출 때 받은 y값이 y + 1 이므로 바로 아래 이 아래 벽이거나 다른 블럭이면
					return 1;
				}
			}
		}
	}
	return 0;
}

// 가이드 블럭 기능
void draw_guide(int random, int cnt, int x, int y) {
	while (!is_detected_block_bottomwall(random, cnt, x, y + 1)) {
		y++;
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		if (clock() - guide_time >= guide_delay) {
			guide_cnt++;
			guide_flag = (guide_flag) ? 0 : 1;
			guide_time = clock();
		}
	}

	if (guide_flag) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (block[random][cnt][i][j] == random + 1) {
					gotoxy((x / 2 + j) * 2, (y + i));
					printf("□");
				}
			}
		}
	}
}

// 쉬프트 인식 블럭 모양 바꾸기
int block_shape_change(int random, int cnt, int x, int y) {
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		block_shape_change_start_time = clock();
		block_shape_change_flag = 1;
	}
	if (clock() - block_shape_change_start_time > block_shape_change_delay && block_shape_change_flag) {
		erase_prev_block(random, cnt, x, y);
		cnt++;
		cnt %= 4;

		if (is_detected_block_sidewall(random, cnt, x, y) || is_detected_block_bottomwall(random, cnt, x, y)) {
			cnt--;
			cnt = (cnt + 4) % 4;
		}

		block_shape_change_flag = 0;
	}
	return cnt;
}

// 스페이스바 인식 하드드랍(빠르게)
int fast_drop(int random, int cnt, int x, int y) {
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
		if (clock() - fast_drop_time >= fast_drop_delay) {
			while (!is_detected_block_bottomwall(random, cnt, x, y + 2)) {
				y++;
			}
			fast_drop_time = clock();
		}
	}
	return y;
}

// 초당 블럭 내리기
int drop_per_second(int y) {
	if (clock() - normal_drop_time > normal_drop_delay) {
		y++;
		normal_drop_time = clock();
	}
	return y;
}

// 방향키 인식 좌 우 움직이기
int left_right(int random, int cnt, int x, int y) {
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if (!is_detected_block_sidewall(random, cnt, x - 2, y) && clock() - last_left_time > left_right_delay) {
			x -= 2;
			last_left_time = clock();
		}
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (!is_detected_block_sidewall(random, cnt, x + 2, y) && clock() - last_right_time > left_right_delay) {
			x += 2;
			last_right_time = clock();
		}
	}
	return x;
}

// 방향키 인식 아래 움직이기
int down(int random, int cnt, int x, int y) {
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		if (!is_detected_block_bottomwall(random, cnt, x, y + 2) && clock() - last_down_time > down_delay) {
			y += 1;
			last_down_time = clock();
		}
	}
	return y;
}

// 라스트 댄스
int last_dance(int random, int cnt, int x, int y) {
	last_dance_time = clock();
	if (clock() - last_dance_time < last_dance_delay) {
		erase_prev_block(random, cnt, x, y); // 이전 블럭 지우기
		x = left_right(random, cnt, x, y); // 좌우 움직이기
	}
	return x;
}

// 해당 좌표 고정된 블럭으로 변환
void fixed_block(int random, int cnt, int x, int y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[random][cnt][i][j] == random + 1) {
				map[y + i][x / 2 + j] = -3;
			}
		}
	}
}

// 완성한 줄 지우기
int remove_full_lines() {
	int full_lines = 0;
	for (int i = Bottom - 1; i > Top; i--) {
		int is_full_flag = 1;
		for (int j = left + 1; j < right; j++) {
			if (map[i][j] != -3) {
				is_full_flag = 0;
				break;
			}
		}

		if (is_full_flag) {
			full_lines++;
			for (int k = i; k > Top; k--) {
				for (int l = left + 1; l < right; l++) {
					map[k][l] = map[k - 1][l];
				}
			}
			i++;
		}
	}
	return full_lines;
}

// 스코어 레벨
void score_level(int full_lines) {
	score_sum += full_lines * 100;
	if (score_sum >= stage_level * 1000 && normal_drop_delay > 0) {
		normal_drop_delay = start_speed - (stage_level * 100);
		stage_level += 1;
	}
}

// 게임 오버
void is_game_over() {

	for (int i = left; i < right; i++) {
		if (map[(stage_level >= 5)? Top + 2 : Top + 1][i] == -3) { //천장(위에서 두번째 줄)에 inactive가 생성되면 게임 오버 
			system("cls");
			int x = 5;
			int y = 5;
			gotoxy(x, y + 0); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤"); //게임오버 메세지 
			gotoxy(x, y + 1); printf("▤                              ▤");
			gotoxy(x, y + 2); printf("▤  +-----------------------+   ▤");
			gotoxy(x, y + 3); printf("▤  |  G A M E  O V E R..   |   ▤");
			gotoxy(x, y + 4); printf("▤  +-----------------------+   ▤");
			if (dif == 1) { //초보
				gotoxy(x, y + 5); printf("▤    YOUR SCORE: %6d        ▤", score_sum);
			}
			else if (dif == 2) { //중수
				gotoxy(x, y + 5); printf("▤    YOUR SCORE: %6d        ▤", score_sum - 2000);
			}
			else if (dif == 3) { // 고수
				gotoxy(x, y + 5); printf("▤    YOUR SCORE: %6d        ▤", score_sum - 4000);
			}
			gotoxy(x, y + 6); printf("▤                              ▤");
			gotoxy(x, y + 7); printf("▤                              ▤");
			gotoxy(x, y + 8); printf("▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤▤");
			game_over_flag = 0;
			Sleep(5000);
		}
	}

}

int main() {
	srand((unsigned int)time(NULL));
	int x = (left + right * 2) / 2 + 1, y = Top;
	int random = rand() % 7, next_random = rand() % 7, cnt = 0;

	// 난이도 설정
	while (1) {
		dif = start();

		if (dif == 1) {	//초보
			break;
		}
		else if (dif == 2) { //중수
			score_sum = 2000;
			stage_level = 2;
		}
		else if (dif == 3) { // 고수
			score_sum = 4000;
			stage_level = 4;
		}
		else {        //종료
			game_over_flag = 0;
		}
		break;
	}

	while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000) && game_over_flag) {
		wall(); // 벽 생성
		info(); // 게임 조작법
		erase_next_block(); // 다음 블럭 지우기
		draw_next_block(next_random); // 다음 블럭 보이기

		DrawMap(); // 지형 그리기
		CursorView(); // 커서 안보이게 하기
		erase_prev_block(random, cnt, x, y); // 이전 블럭 지우기

		x = left_right(random, cnt, x, y); // 좌우 움직이기
		y = fast_drop(random, cnt, x, y); // 스페이스 바 누르면 빠르게(하드) 드랍
		y = down(random, cnt, x, y); // 아래 움직이기
		y = drop_per_second(y); // 초 마다 드랍
		cnt = block_shape_change(random, cnt, x, y); // 모양 바꾸기

		create_block(random, cnt, x, y); // 현재 블럭 그리기
		draw_guide(random, cnt, x, y); // 가이드 블럭 기능

		is_game_over();

		// 블럭 스왑 기능
		if (GetAsyncKeyState(VK_TAB) & 0x8000) {
			if (clock() - block_swap_time > block_swap_delay) {
				erase_prev_block(random, cnt, x, y);

				if ((is_detected_block_sidewall(random, cnt, x - 2, y) || is_detected_block_sidewall(random, cnt, x + 2, y) || is_detected_block_bottomwall(random, cnt, x, y + 2)) &&
					(is_detected_block_sidewall(stored_block, 0, x - 2, y) || is_detected_block_sidewall(stored_block, 0, x + 2, y) || is_detected_block_bottomwall(stored_block, 0, x, y + 2))) {
					continue;
				}
				else {
					if (stored_block == -1) {
						stored_block = random;
						random = next_random;
						next_random = rand() % 7;
						cnt = 0;
					}
					else {
						int temp = stored_block;
						stored_block = random;
						random = temp;
						cnt = 0;
					}
					erase_next_block();
					draw_next_block(next_random);
					erase_stored_block();
					draw_stored_block(stored_block);
				}

				block_swap_time = clock();
			}
		}

		if (is_detected_block_bottomwall(random, cnt, x, y + 1)) {
			x = last_dance(random, cnt, x, y);
			fixed_block(random, cnt, x, y); // 해당 위치 블럭 고정
			score_level(remove_full_lines()); // 한 줄 지우고 점수 매기기

			cnt = 0; // 모양 초기화
			x = (left + right * 2) / 2 + 1, y = Top; // 좌표 초기화
			random = next_random; // 다음 블럭이 이번에 나오는 블럭이 됨
			next_random = rand() % 7; // 종류 초기화
		}
	}
	system("cls");
	return 0;
}