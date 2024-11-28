/*
* raw(?) I/O
*/
#include "io.h"
#include "common.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// set_color 함수에서 배경색을 설정하도록 수정
void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 배경색만 설정하는 printc 함수 수정
void printc(POSITION pos, char ch, int color) {
	set_color(color);  // 색상 설정
	gotoxy(pos);       // 위치로 커서 이동
	printf("%c", ch);  // 문자 출력
}

KEY get_key(void) {
    if (!_kbhit()) {
        return k_none;
    }

    int byte = _getch();
    switch (byte) {
    case 'q': return k_quit;
    case ' ': return k_space;   // 스페이스 키 추가
    case 27:  return k_esc;     // ESC 키 추가
    case 224:
        byte = _getch();
        switch (byte) {
        case 72: return k_up;
        case 75: return k_left;
        case 77: return k_right;
        case 80: return k_down;
        default: return k_undef;
        }
    default: return k_undef;
    }
}