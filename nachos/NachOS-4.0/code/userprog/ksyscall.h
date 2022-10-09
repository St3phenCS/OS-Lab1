/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include "synchconsole.h"

#define MAXLENGTH 256
#define MODULUS 2147483647
#define CONST_C 16807
int RAND_NUM = 0;

// Input: - User space address (int)
// - Limit of buffer (int)
// Output:- Buffer (char*)
// Purpose: Copy buffer from User memory space to System memory space
char *User2System(int virtAddr, int limit)
{
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    // printf("\n Filename u2s: ");

    for (i = 0; i < limit; i++)
    {
        kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
        kernelBuf[i] = (char)oneChar;
        // printf("%c", kernelBuf[i])
        if (oneChar == 0)
            break;
    }
    return kernelBuf;
}

// Input: - User space address (int)
// - Limit of buffer (int)
// - Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr, char *buffer, int len)
{
    if (len <= 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do
    {
        oneChar = (int)buffer[i];
        kernel->machine->WriteMem(virtAddr + i, 1, oneChar);
        i++;
    } while (i < len && oneChar != 0);
    return i;
}

void IncreasePC()
/* Modify return point */
{
    /* set previous programm counter (debugging only)*/
    kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

    /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
    kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

    /* set next programm counter for brach execution */
    kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}
// -------------------------------------------------------------------

void SysHalt()
{
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

int SysSub(int op1, int op2)
{
    return op1 - op2;
}

void SysReadChar()
{
    char firstChar;
    firstChar = kernel->synchConsoleIn->GetChar();
    while (firstChar != '\n' && kernel->synchConsoleIn->GetChar() != '\n')
        ;
    kernel->machine->WriteRegister(2, firstChar);
}
void SysPrintChar()
{
    char _char = (char)kernel->machine->ReadRegister(4);
    kernel->synchConsole_Write(&_char, 1);
}

void SysReadString()
{
    char *buffer = new char[MAXLENGTH + 1];
    if (!buffer)
        return;
    int i = 0;
    char ch;
    while (i < MAXLENGTH)
    {
        do
        {
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        if (ch == '\0' || ch == '\n') // enter -> ket thuc nhap
            break;
        buffer[i++] = ch;
    }
    // if (i >= MAXLENGTH) {
    //     while (kernel->synchConsoleIn->GetChar() != '\0')
    //         ;
    // }
    int ptr = kernel->machine->ReadRegister(4);
    if (buffer)
    {
        System2User(ptr, buffer, MAXLENGTH);
        delete[] buffer;
    }
}
void SysPrintString()
{
    int i = 0;
    int ptr = kernel->machine->ReadRegister(4);
    char *buffer = User2System(ptr, MAXLENGTH);
    while (buffer[i] != '\0')
    {
        kernel->synchConsoleOut->PutChar(buffer[i++]);
    }
}

// https://en.wikipedia.org/wiki/Linear_congruential_generator

void SysRandomNum()
{
    int seed = kernel->stats->totalTicks;
    RAND_NUM = (RAND_NUM * seed + CONST_C) % MODULUS;
    kernel->machine->WriteRegister(2, RAND_NUM);
}

void SysReadNum()
{
    /*Input: NULL
    Output: số nguyên Int
    CN: Đọc số nguyên từ bàn phím
    */
    char *buffer = new char[MAXLENGTH + 1];
    if (!buffer)
        return;
    int i = 0;
    char ch;
    while (i < MAXLENGTH)
    {
        do
        {
            ch = kernel->synchConsoleIn->GetChar();
        } while (ch == EOF);
        if (ch == '\0' || ch == '\n') // enter -> ket thuc nhap
            break;
        buffer[i++] = ch;
    }
    bool isInt = true;
    bool isNegative = false;
    int result = 0;
    // Kiểm tra số nhập vào có phải là số âm hay không     
    // Kiểm tra tràn số
    if (buffer[0] == '-')
    {
        isNegative = true;
        i = 1;
        if (strlen(buffer) > 11)
        {
            isInt = false;
        }
        if (strcmp(buffer,"-2147483647")>0)
        {
            isInt=false;
        }
    }
    else
    {
        isNegative = false;
        i = 0;
        if (strlen(buffer) > 10)
        {
            isInt = false;
        }
        if (strcmp(buffer,"2147483647")>0)
        {
            isInt=false;
        }
    }
    // Kiểm tra các kí tự nhập vào có phải số hay không
    while (buffer[i] != '\0')
    {
        if (buffer[i] < 48 || buffer[i] > 57)
        {
            isInt = false;
            break;
        }
        i++;
    }
    int num = 0;
    // chuyển chuỗi thành số
    if (isInt)
    {
        i = 0;
        if (isNegative)
        {
            i = 1;
        }
        while (buffer[i] != '\0')
        {
            num = num * 10 + int(buffer[i]) - '0';
            i++;
        }
        if (isNegative)
        {
            num = -num;
        }
        result = num;
    }
    else
    {
        result = 0;
    }
    kernel->machine->WriteRegister(2, result);
}

void SysPrintNum()
{
    /*Input: số nguyên Int
    Output: NULL
    CN: In một số nguyên ra màn hình
    */
    bool isNegative = false;
    int so = kernel->machine->ReadRegister(4);
    int i = 0;
    char *buffer = new char[MAXLENGTH + 1];
    if (so != 0)
    {
        if (so < 0)
        {
            isNegative = true;
            so = -so;
        }
        int chuso = 0;
        while (so != 0)
        {
            chuso = so % 10;
            buffer[i] = chuso + '0';
            so = so / 10;
            i++;
        }
        if (isNegative)
        {
            buffer[i] = '-';
            i++;
        }
    }
    else
    {
        buffer[i] = '0';
        i++;
    }
    buffer[i] = '\0';
    while (i >= 0)
    {
        kernel->synchConsoleOut->PutChar(buffer[i--]);
    }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
