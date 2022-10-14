#include "syscall.h"
char s[256];
#define MAXLENGTH 256
int main() {
    char ch;
    int x;
    int i = 0;

    //Đọc - In ra 1 ký tự nhập từ bàn phím
    PrintString("\nNhap ki tu can in ra:\t");

    ch = ReadChar();
    PrintString("Ki tu vua nhap la:\t");
    PrintChar(ch);

    // Đọc - In ra 1 chuỗi ký tự nhập từ bàn phím
    PrintString("\nNhap chuoi can in ra:\t");
    ReadString(s, MAXLENGTH);
    PrintString("Chuoi vua nhap la:\t");
    PrintString(s);

    //Đọc - In ra 1 dãy số nguyên từ bàn phím
    PrintString("\nNhap so nguyen:\t");
    x = ReadNum();
    PrintString("So vua nhap:\t");
    PrintNum(x);

    // In ra 1 số ngẫu nhiên

    for (; i < 30; ++i) {
        PrintString("\nIn ra 1 so ngau nhien\t");
        x = RandomNum();
        PrintNum(x);
    }

    Halt();
}