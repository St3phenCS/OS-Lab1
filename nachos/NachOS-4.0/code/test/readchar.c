#include "syscall.h"
char s[256];

int main()
{

    char ch;
    int x;
    // PrintString("\nNhap ki tu can in ra:\t");
    // ch = ReadChar();
    // PrintString("\nKi tu vua nhap la:\t");
    // PrintChar(ch);
    // PrintString("\nNhap chuoi can in ra:\t");
    // ReadString(s);
    // PrintString("\nChuoi vua nhap la:\t");
    // PrintString(s);
    // PrintString("\nIn ra 1 so ngau nhien\t");
    // x = RandomNum();
    // PrintNum(x);
    PrintString("\nNhap so nguyen:\t");
    x = ReadNum();
    PrintString("So vua nhap:\t");
    PrintNum(x);
    Halt();
}