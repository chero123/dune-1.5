/*
*  display.c:
* ȭ�鿡 ���� ������ ���
* ��, Ŀ��, �ý��� �޽���, ����â, �ڿ� ���� ���
* io.c�� �ִ� �Լ����� �����
*/

#include "display.h"
#include "io.h"
#include "common.h"
// ���� ���� �߰�
#define COLOR_PLAYER_BLUE      9   // ��Ʈ���̵� (�Ķ���)
#define COLOR_AI_RED          12   // ���ڳ� (������)
#define COLOR_SANDWORM       14   // ����� (Ȳ���)
#define COLOR_PLATE          8    // ���� (������)
#define COLOR_SPICE          6    // �����̽� (��Ȳ��)
#define COLOR_TERRAIN        7    // ��Ÿ ���� (ȸ��)
// ����� ������� �»��(topleft) ��ǥ
const POSITION resource_pos = { 0, 0 };
const POSITION map_pos = { 1, 0 };

const POSITION unit_status_pos = { 0, MAP_WIDTH + 2 };  // ������ ��
const POSITION system_message_pos = { MAP_HEIGHT + 2, 0 };  // ���� �Ʒ�
const POSITION command_pos = { MAP_HEIGHT + 2, MAP_WIDTH + 2 };  // ������ �Ʒ�

char backbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };
char frontbuf[MAP_HEIGHT][MAP_WIDTH] = { 0 };

void project(char src[N_LAYER][MAP_HEIGHT][MAP_WIDTH], char dest[MAP_HEIGHT][MAP_WIDTH]);
void display_resource(RESOURCE resource);
void display_map(char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH]);
void display_cursor(CURSOR cursor);

void display_system_message() {
	set_color(COLOR_DEFAULT);  // �⺻ �������� ���
	gotoxy(system_message_pos);  // ���� �Ʒ��� Ŀ���� �̵�
	printf("�ý��� �޽���: \n");
}

void display_object_info(CURSOR cursor) {
	set_color(COLOR_DEFAULT);  // �⺻ �������� ���
	gotoxy(unit_status_pos);  // ������ ���� Ŀ���� �̵�

	// ���÷� Ŀ�� ��ġ�� �������� ���� ������ ���
	printf("����â:\n");


}

void display_commands() {
	set_color(COLOR_DEFAULT);  // �⺻ �������� ���
	gotoxy(command_pos);  // ������ �Ʒ��� Ŀ���� �̵�

	// ��� ������ ��ɾ� ����� ���
	printf("���â:\n");
	printf("								��: Move Up\n");
	printf("								��: Move Down\n");
	printf("								��: Move Left\n");
	printf("								��: Move Right\n");
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




// frontbuf[][]���� Ŀ�� ��ġ�� ���ڸ� ���� �ٲ㼭 �״�� �ٽ� ���
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
	POSITION curr = cursor->current;  // Ŀ���� ���� ��ġ
	char building = backbuf[curr.row][curr.column];  // �ش� ��ġ�� �ǹ� ���� ��������

	// �ش� ��ġ�� ���� �ٸ� �޽����� ���
	if (building == 'B') {
		printf("�����Դϴ�\n");
		is_tile_selected = true;
	}
	else if (building == 'P') {
		printf("�����Դϴ�\n");
	}
	else {
		printf("�縷�Դϴ�\n");
	}
}