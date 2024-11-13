// 1) 일부 초기배치 구현
// 2) 방향키 더블클릭했을때 연속이동과 스페이스바를 눌렀을때 상태표시 구현

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
		// loop 돌 때마다(즉, TICK==10ms마다) 키 입력 확인
		KEY key = get_key();

		// 키 입력이 있으면 처리
		if (is_arrow_key(key)) {
			cursor_move(ktod(key));
		}
		else {
			// 방향키 외의 입력
			switch (key) {
			case k_space:
				// 스페이스바를 눌렀을 때 출력할 메시지 처리
				handle_spacebar(cursor);  // 예: 커서 위치에 따라 메시지 출력	
				break;
			case k_quit: outro();
			case k_none:
			case k_undef:
			default: break;
			}
		}

		// 샘플 오브젝트 동작
		sample_obj_move();

		// 화면 출력
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
	// layer 0(map[0])에 지형 생성
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

	// layer 1(map[1])은 비워 두기(-1로 채움)
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

	// 2x2 바위들
	map[0][10][25] = 'R';
	map[0][10][26] = 'R';
	map[0][11][25] = 'R';
	map[0][11][26] = 'R';

	map[0][3][21] = 'R';
	map[0][3][22] = 'R';
	map[0][4][21] = 'R';
	map[0][4][22] = 'R';

	map[1][MAP_HEIGHT - 4][1] = 'H'; // 아래 본진 위에 H
	map[1][3][MAP_WIDTH - 2] = 'H'; // 위 본진 아래에 H

	map[0][5][7] = 'W';
	map[0][MAP_HEIGHT - 9][MAP_WIDTH - 11] = 'W';
}




DIRECTION last_direction = d_stay;   // 마지막 방향 입력
int consecutive_moves = 0;           // 연속된 방향 입력 횟수
clock_t last_move_time = 0;          // 마지막 입력 시간

void cursor_move(DIRECTION dir) {
	POSITION curr = cursor.current;
	clock_t current_time = clock();

	// 연속 입력 시간 차이 계산
	double time_diff = (double)(current_time - last_move_time) / CLOCKS_PER_SEC;

	// 0.3초 이내에 같은 방향 입력이 들어오면 연속 이동 증가
	if (last_direction == dir && time_diff <= 0.3) {
		consecutive_moves++;
	}
	else {
		consecutive_moves = 1;  // 조건에 맞지 않으면 연속 이동 초기화
	}

	last_direction = dir;
	last_move_time = current_time;

	// 이동 거리: 연속 입력이 두 번 이상이면 4칸, 아니면 1칸 이동
	int move_distance = (consecutive_moves >= 2) ? 4 : 1;

	POSITION new_pos = curr;  // 최종 위치를 계산할 변수

	// 이동 거리만큼 반복하여 이동
	for (int i = 0; i < move_distance; i++) {
		POSITION next_pos = pmove(new_pos, dir);

		// 맵 경계를 벗어나지 않도록 validation check 유지
		if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 &&
			1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2) {
			new_pos = next_pos; // 최종 위치만 갱신
		}
		else {
			break; // 맵 경계를 벗어나면 이동 중단
		}
	}

	// 이동이 완료된 후에 한 번만 이전 위치와 현재 위치를 갱신
	cursor.previous = cursor.current;
	cursor.current = new_pos;
}








/* ================= sample object movement =================== */
POSITION sample_obj_next_position(void) {
	// 현재 위치와 목적지를 비교해서 이동 방향 결정	
	POSITION diff = psub(obj.dest, obj.pos);
	DIRECTION dir;

	// 목적지 도착. 지금은 단순히 원래 자리로 왕복
	if (diff.row == 0 && diff.column == 0) {
		if (obj.dest.row == 1 && obj.dest.column == 1) {
			// topleft --> bottomright로 목적지 설정
			POSITION new_dest = { MAP_HEIGHT - 2, MAP_WIDTH - 2 };
			obj.dest = new_dest;
		}
		else {
			// bottomright --> topleft로 목적지 설정
			POSITION new_dest = { 1, 1 };
			obj.dest = new_dest;
		}
		return obj.pos;
	}

	// 가로축, 세로축 거리를 비교해서 더 먼 쪽 축으로 이동
	if (abs(diff.row) >= abs(diff.column)) {
		dir = (diff.row >= 0) ? d_down : d_up;
	}
	else {
		dir = (diff.column >= 0) ? d_right : d_left;
	}

	// validation check
	// next_pos가 맵을 벗어나지 않고, (지금은 없지만)장애물에 부딪히지 않으면 다음 위치로 이동
	// 지금은 충돌 시 아무것도 안 하는데, 나중에는 장애물을 피해가거나 적과 전투를 하거나... 등등
	POSITION next_pos = pmove(obj.pos, dir);
	if (1 <= next_pos.row && next_pos.row <= MAP_HEIGHT - 2 && \
		1 <= next_pos.column && next_pos.column <= MAP_WIDTH - 2 && \
		map[1][next_pos.row][next_pos.column] < 0) {

		return next_pos;
	}
	else {
		return obj.pos;  // 제자리
	}
}

void sample_obj_move(void) {
	if (sys_clock <= obj.next_move_time) {
		// 아직 시간이 안 됐음
		return;
	}

	// 오브젝트(건물, 유닛 등)은 layer1(map[1])에 저장
	map[1][obj.pos.row][obj.pos.column] = -1;
	obj.pos = sample_obj_next_position();
	map[1][obj.pos.row][obj.pos.column] = obj.repr;

	obj.next_move_time = sys_clock + obj.speed;
}

