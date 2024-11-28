/*
*  display.c:
* 화면에 게임 정보를 출력
* 맵, 커서, 시스템 메시지, 정보창, 자원 상태 등등
* io.c에 있는 함수들을 사용함
*/

#include "display.h"
#include "io.h"
#include "common.h"
// 색상 정의 추가
#define COLOR_PLAYER_BLUE      9   // 아트레이디스 (파란색)
#define COLOR_AI_RED          12   // 하코넨 (빨간색)
#define COLOR_SANDWORM       14   // 샌드웜 (황토색)
#define COLOR_PLATE          8    // 장판 (검은색)
#define COLOR_SPICE          6    // 스파이스 (주황색)
#define COLOR_TERRAIN        7    // 기타 지형 (회색)
// 출력할 내용들의 좌상단(topleft) 좌표
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

const POSITION unit_status_pos = { 0, MAP_WIDTH + 2 };  // 오른쪽 위
const POSITION system_message_pos = { MAP_HEIGHT + 2, 0 };  // 왼쪽 아래
const POSITION command_pos = { MAP_HEIGHT + 2, MAP_WIDTH + 2 };  // 오른쪽 아래

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);

void display_system_message() {
	set_color(COLOR_DEFAULT);  // 기본 색상으로 출력
	gotoxy(system_message_pos);  // 왼쪽 아래로 커서를 이동
	printf("시스템 메시지: \n");
}

void display_object_info(CURSOR cursor) {
	set_color(COLOR_DEFAULT);  // 기본 색상으로 출력
	gotoxy(unit_status_pos);  // 오른쪽 위로 커서를 이동

	// 예시로 커서 위치를 기준으로 유닛 정보를 출력
	printf("상태창:\n");


}

void display_commands() {
	set_color(COLOR_DEFAULT);  // 기본 색상으로 출력
	gotoxy(command_pos);  // 오른쪽 아래로 커서를 이동

	// 사용 가능한 명령어 목록을 출력
	printf("명령창:\n");
	printf("								↑: Move Up\n");
	printf("								↓: Move Down\n");
	printf("								←: Move Left\n");
	printf("								→: Move Right\n");
	printf("								Q: Quit Game\n");
}

void display(
	RESOURCE resource,
	char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH],
	CURSOR cursor)
{
	display_resource(resource);
	display_map(map);
	display_cursor(cursor);
	display_system_message();
	display_object_info(cursor);
	display_commands();
	// ...
}

void display_resource(RESOURCE resource) {
	set_color(COLOR_RESOURCE);
	gotoxy(resource_pos);
	printf("spice = %d/%d, population=%d/%d\n",
		resource.spice, resource.spice_max,
		resource.population, resource.population_max
	);
}

// subfunction of draw_map()
void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]) {
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			for (int k = 0; k < N_LAYER; k++) {
				if (src[k][i][j] >= 0) {
					dest[i][j] = src[k][i][j];
				}
			}
		}
	}
}

void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]) {
	project(map, backbuf);

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			int color = COLOR_DEFAULT;
			switch (backbuf[i][j]) {
			case 'B': color = COLOR_PLAYER_BLUE; break;
			case 'W': color = COLOR_SANDWORM; break;
			case 'H': color = (map[1][i][j] == 'H' ? COLOR_PLAYER_BLUE : COLOR_AI_RED); break;
			case 'S': color = COLOR_SPICE; break;
			case 'P': color = COLOR_PLATE; break;
			case 'R': color = COLOR_TERRAIN; break;
			default: color = COLOR_DEFAULT;
			}

			if (frontbuf[i][j] != backbuf[i][j]) {
				POSITION pos = { i, j };
				printc(padd(map_pos, pos), backbuf[i][j], color);
			}
			frontbuf[i][j] = backbuf[i][j];
		}
	}
}




// frontbuf[][]에서 커서 위치의 문자를 색만 바꿔서 그대로 다시 출력
void display_cursor(CURSOR cursor) {
	POSITION prev = cursor.previous;
	POSITION curr = cursor.current;

	char ch = frontbuf[prev.row][prev.column];
	printc(padd(map_pos, prev), ch, COLOR_DEFAULT);

	ch = frontbuf[curr.row][curr.column];
	printc(padd(map_pos, curr), ch, COLOR_CURSOR);
}

bool is_tile_selected = false;

void handle_spacebar(CURSOR* cursor) {
	POSITION curr = cursor->current;  // 커서의 현재 위치
	char building = backbuf[curr.row][curr.column];  // 해당 위치의 건물 정보 가져오기

	// 해당 위치에 따라 다른 메시지를 출력
	if (building == 'B') {
		printf("본진입니다\n");
		is_tile_selected = true;
	}
	else if (building == 'P') {
		printf("장판입니다\n");
	}
	else {
		printf("사막입니다\n");
	}
}