// 1) �Ϻ� �ʱ��ġ ����
// 2) ����Ű ����Ŭ�������� �����̵��� �����̽��ٸ� �������� ����ǥ�� ����
// �Ϻ����� ����ü ����

#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "io.h"
#include "display.h"
#include <windows.h>
#include <time.h>

void init(void);
void intro(void);
void outro(void);
void cursor_move(DIRECTION dir);
void sample_obj_move(void);
BASE_SELECT_STATE base_select_state = BS_NONE;
SYSTEM_MESSAGE sys_messages = { 0 };
//void handle_spacebar(CURSOR cursor);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };

OBJECT objects[MAX_OBJECTS];
int object_count = 0;

/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300

};

void add_system_message(const char* message) {
	if (sys_messages.message_count >= MAX_SYSTEM_MESSAGES) {
		for (int i = 0; i < MAX_SYSTEM_MESSAGES - 1; i++) {
			strcpy_s(sys_messages.messages[i], sizeof(sys_messages.messages[i]), sys_messages.messages[i + 1]);
		}
		sys_messages.current_index = MAX_SYSTEM_MESSAGES - 1;
	}
	else {
		sys_messages.current_index = sys_messages.message_count;
		sys_messages.message_count++;
	}

	// �� �޽��� �߰�
	strcpy_s(sys_messages.messages[sys_messages.current_index], sizeof(sys_messages.messages[0]), message);
}

void init_game_objects() {
	// ��Ʈ���̵� ���� (���ϴ�)
	map[0][MAP_HEIGHT - 3][1] = 'B';
	map[0][MAP_HEIGHT - 3][2] = 'B';
	map[0][MAP_HEIGHT - 2][1] = 'B';
	map[0][MAP_HEIGHT - 2][2] = 'B';

	// ���ڳ� ���� (����)
	map[0][1][MAP_WIDTH - 3] = 'B';
	map[0][1][MAP_WIDTH - 2] = 'B';
	map[0][2][MAP_WIDTH - 3] = 'B';
	map[0][2][MAP_WIDTH - 2] = 'B';

	// ���� ��ġ
	map[0][MAP_HEIGHT - 3][3] = 'P';
	map[0][MAP_HEIGHT - 3][4] = 'P';
	map[0][MAP_HEIGHT - 2][3] = 'P';
	map[0][MAP_HEIGHT - 2][4] = 'P';

	map[0][1][MAP_WIDTH - 5] = 'P';
	map[0][1][MAP_WIDTH - 4] = 'P';
	map[0][2][MAP_WIDTH - 5] = 'P';
	map[0][2][MAP_WIDTH - 4] = 'P';

	// �����̽� ������ ��ġ
	map[0][MAP_HEIGHT - 6][5] = 'S';
	map[0][4][MAP_WIDTH - 6] = 'S';

	// �߸� ����� ��ġ
	map[0][MAP_HEIGHT / 2][MAP_WIDTH / 2] = 'W';
	map[0][MAP_HEIGHT / 2 - 1][MAP_WIDTH / 2 + 1] = 'W';

	// ���� ��ġ
	map[0][9][38] = 'R';
	map[0][6][14] = 'R';
	map[0][14][36] = 'R';
	// 2x2 ������
	map[0][10][25] = 'R';
	map[0][10][26] = 'R';
	map[0][11][25] = 'R';
	map[0][11][26] = 'R';

	map[0][3][21] = 'R';
	map[0][3][22] = 'R';
	map[0][4][21] = 'R';
	map[0][4][22] = 'R';
	
}

void spawn_initial_units() {
	// ��Ʈ���̵� �Ϻ�����
	OBJECT harvester1 = {
		.pos = {MAP_HEIGHT - 4, 1},
		.repr = 'H',
		.team = 1,
		.hp = 100,
		.max_hp = 100
	};
	objects[object_count++] = harvester1;
	map[1][harvester1.pos.row][harvester1.pos.column] = harvester1.repr;

	// ���ڳ� �Ϻ�����
	OBJECT harvester2 = {
		.pos = {5, MAP_WIDTH - 4},
		.repr = 'H',
		.team = 2,
		.hp = 100,
		.max_hp = 100
	};
	objects[object_count++] = harvester2;
	map[1][harvester2.pos.row][harvester2.pos.column] = harvester2.repr;
}

/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			switch (key) {
			case k_quit: outro();
			case k_space: // �����̽� Ű�� �ǹ� ����
				if (map[0][cursor.current.row][cursor.current.column] == 'B') {
					base_select_state = BS_BASE_SELECTED;
					add_system_message("Base selected");
				}
				break;
			case k_esc: // ESC Ű�� ���� ���
				if (base_select_state != BS_NONE) {
					base_select_state = BS_NONE;
					add_system_message("Base selection canceled");
				}
				break;
			case 'h': // 'H' Ű�� �Ϻ����� ����
				if (base_select_state == BS_BASE_SELECTED) {
					if (resource.spice >= 50) { // ���� ��� ����
						// �Ϻ����� ���� ����
						OBJECT new_harvester = {
							.pos = {cursor.current.row, cursor.current.column},
							.repr = 'H',
							.team = 1,
							.hp = 100,
							.max_hp = 100
						};

						// �ֺ� �� Ÿ�Ͽ� �Ϻ����� ��ġ
						if (map[1][new_harvester.pos.row][new_harvester.pos.column] == -1) {
							objects[object_count++] = new_harvester;
							map[1][new_harvester.pos.row][new_harvester.pos.column] = new_harvester.repr;

							resource.spice -= 50; // ���� ��� ����
							add_system_message("A new harvester ready");
						}
						else {
							add_system_message("Cannot place harvester here");
						}
					}
					else {
						add_system_message("Not enough spice");
					}
				}
				break;
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// ���� ������Ʈ ����
		sample_obj_move();

		// ȭ�� ���
		display(resource, map, cursor);
		Sleep(TICK);
		sys_clock += 10;
	}
}

/* ================= subfunctions =================== */
void intro(void) {
	printf(" DDDD   U   U   N   N   EEEEE	\n");
	printf(" D   D  U   U   NN  N   E		\n");
	printf(" D   D  U   U   N N N   EEEE	\n");
	printf(" D   D  U   U   N  NN   E		\n");
	printf(" DDDD   UUUUU   N   N   EEEEE	\n");
	Sleep(1000);
	system("cls");
}

void outro(void) {
	printf("exiting...\n");
	exit(0);
}

void init(void) {
	// layer 0(map[0])�� ���� ����
	for (int j = 0; j < MAP_WIDTH; j++) {
		map[0][0][j] = '#';
		map[0][MAP_HEIGHT - 1][j] = '#';
	}

	for (int i = 1; i < MAP_HEIGHT - 1; i++) {
		map[0][i][0] = '#';
		map[0][i][MAP_WIDTH - 1] = '#';
		for (int j = 1; j < MAP_WIDTH - 1; j++) {
			map[0][i][j] = ' ';
		}
	}

	// layer 1(map[1])�� ��� �α�(-1�� ä��)
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			map[1][i][j] = -1;
		}
	}

	// object sample
	init_game_objects();
	spawn_initial_units();
}



DIRECTION last_direction = d_stay;   // ������ ���� �Է�
int consecutive_moves = 0;           // ���ӵ� ���� �Է� Ƚ��
clock_t last_move_time = 0;          // ������ �Է� �ð�

void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	clock_t current_time = clock();

	// ���� �Է� �ð� ���� ���
	double time_diff = (double)(current_time - last_move_time) / CLOCKS_PER_SEC;

	// 0.3�� �̳��� ���� ���� �Է��� ������ ���� �̵� ����
	if (last_direction == dir && time_diff <= 0.3) {
		consecutive_moves++;
	}
	else {
		consecutive_moves = 1;  // ���ǿ� ���� ������ ���� �̵� �ʱ�ȭ
	}

	last_direction = dir;
	last_move_time = current_time;

	// �̵� �Ÿ�: ���� �Է��� �� �� �̻��̸� 4ĭ, �ƴϸ� 1ĭ �̵�
	int move_distance = (consecutive_moves >= 2) ? 4 : 1;

	POSITION new_pos = curr;  // ���� ��ġ�� ����� ����

	// �̵� �Ÿ���ŭ �ݺ��Ͽ� �̵�
	for (int i = 0; i < move_distance; i++) {
		POSITION next_pos = pmove(new_pos, dir);

		// �� ��踦 ����� �ʵ��� validation check ����
		if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
			1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
			new_pos = next_pos; // ���� ��ġ�� ����
		}
		else {
			break; // �� ��踦 ����� �̵� �ߴ�
		}
	}

	// �̵��� �Ϸ�� �Ŀ� �� ���� ���� ��ġ�� ���� ��ġ�� ����
	cursor.previous = cursor.current;
	cursor.current = new_pos;
}








/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// ���� ��ġ�� �������� ���ؼ� �̵� ���� ����	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// ������ ����. ������ �ܼ��� ���� �ڸ��� �պ�
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright�� ������ ����
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft�� ������ ����
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// ������, ������ �Ÿ��� ���ؼ� �� �� �� ������ �̵�
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos�� ���� ����� �ʰ�, (������ ������)��ֹ��� �ε����� ������ ���� ��ġ�� �̵�
	// ������ �浹 �� �ƹ��͵� �� �ϴµ�, ���߿��� ��ֹ��� ���ذ��ų� ���� ������ �ϰų�... ���
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // ���ڸ�
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// ���� �ð��� �� ����
		return;
	}

	// ������Ʈ(�ǹ�, ���� ��)�� layer1(map[1])�� ����
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}