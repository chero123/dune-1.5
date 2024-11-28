/*
* raw(?) I/O
*/
#include "io.h"
#include "common.h"

void gotoxy(POSITION pos) {
	COORD coord = { pos.column, pos.row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// set_color �Լ����� ������ �����ϵ��� ����
void set_color(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// ������ �����ϴ� printc �Լ� ����
void printc(POSITION pos, char ch, int color) {
	set_color(color);  // ���� ����
	gotoxy(pos);       // ��ġ�� Ŀ�� �̵�
	printf("%c", ch);  // ���� ���
}

KEY get_key(void) {
    if (!_kbhit()) {
        return k_none;
    }

    int byte = _getch();
    switch (byte) {
    case 'q': return k_quit;
    case ' ': return k_space;   // �����̽� Ű �߰�
    case 27:  return k_esc;     // ESC Ű �߰�
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