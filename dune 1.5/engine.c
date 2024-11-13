// 1) �Ϻ� �ʱ��ġ ����
// 2) ����Ű ����Ŭ�������� �����̵��� �����̽��ٸ� �������� ����ǥ�� ����

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
void handle_spacebar(CURSOR cursor);
POSITION sample_obj_next_position(void);


/* ================= control =================== */
int sys_clock = 0;		// system-wide clock(ms)
CURSOR cursor = { { 1, 1 }, {1, 1} };


/* ================= game data =================== */
char map[N_LAYER][MAP_HEIGHT][MAP_WIDTH] = { 0 };

RESOURCE resource = {
	.spice = 0,
	.spice_max = 0,
	.population = 0,
	.population_max = 0
};

OBJECT_SAMPLE obj = {
	.pos = {1, 1},
	.dest = {MAP_HEIGHT - 2, MAP_WIDTH - 2},
	.repr = 'o',
	.speed = 300,
	.next_move_time = 300

};


/* ================= main() =================== */
int main(void) {
	srand((unsigned int)time(NULL));

	init();
	intro();
	display(resource, map, cursor);

	while (1) {
		// loop �� ������(��, TICK==10ms����) Ű �Է� Ȯ��
		KEY key = get_key();

		// Ű �Է��� ������ ó��
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// ����Ű ���� �Է�
			switch (key) {
			case k_space:
				// �����̽��ٸ� ������ �� ����� �޽��� ó��
				handle_spacebar(cursor);  // ��: Ŀ�� ��ġ�� ���� �޽��� ���	
				break;
			case k_quit: outro();
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
	map[1][obj.pos.row][obj.pos.column] = 'o';

	map[0][MAP_HEIGHT - 3][1] = 'B';
	map[0][MAP_HEIGHT - 3][2] = 'B';
	map[0][MAP_HEIGHT - 2][1] = 'B';
	map[0][MAP_HEIGHT - 2][2] = 'B';

	map[0][1][MAP_WIDTH - 3] = 'B';
	map[0][1][MAP_WIDTH - 2] = 'B';
	map[0][2][MAP_WIDTH - 3] = 'B';
	map[0][2][MAP_WIDTH - 2] = 'B';

	map[0][MAP_HEIGHT - 3][3] = 'P';
	map[0][MAP_HEIGHT - 3][4] = 'P';
	map[0][MAP_HEIGHT - 2][3] = 'P';
	map[0][MAP_HEIGHT - 2][4] = 'P';

	map[0][1][MAP_WIDTH - 5] = 'P';
	map[0][1][MAP_WIDTH - 4] = 'P';
	map[0][2][MAP_WIDTH - 5] = 'P';
	map[0][2][MAP_WIDTH - 4] = 'P';

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

	map[1][MAP_HEIGHT - 4][1] = 'H'; // �Ʒ� ���� ���� H
	map[1][3][MAP_WIDTH - 2] = 'H'; // �� ���� �Ʒ��� H

	map[0][5][7] = 'W';
	map[0][MAP_HEIGHT - 9][MAP_WIDTH - 11] = 'W';
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

