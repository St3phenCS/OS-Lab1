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
char *User2System(int virtAddr, int limit) {
    int i; // index
    int oneChar;
    char *kernelBuf = NULL;
    kernelBuf = new char[limit + 1]; // need for terminal string

    if (kernelBuf == NULL)
        return kernelBuf;
    memset(kernelBuf, 0, limit + 1);
    // printf("\n Filename u2s: ");

    for (i = 0; i < limit; i++) {
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
int System2User(int virtAddr, char *buffer, int len) {
    if (len <= 0)
        return len;
    int i = 0;
    int oneChar = 0;
    do {
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

void SysHalt() {
    kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) {
    return op1 + op2;
}

int SysSub(int op1, int op2) {
    return op1 - op2;
}

void SysReadChar() {
    char firstChar;
    firstChar = kernel->synchConsoleIn->GetChar();
    while (firstChar != '\n' && kernel->synchConsoleIn->GetChar() != '\n')
        ;
    kernel->machine->WriteRegister(2, firstChar);
}
void SysPrintChar() {
    char _char = (char)kernel->machine->ReadRegister(4);
    kernel->synchConsole_Write(&_char, 1);
}

void SysReadString() {
    char *buffer = new char[MAXLENGTH + 1];
    if (!buffer)
        return;
    int i = 0;
    char ch;
    while (i < MAXLENGTH) {
        do {
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
    if (buffer) {
        System2User(ptr, buffer, MAXLENGTH);
        delete[] buffer;
    }
}
void SysPrintString() {
    int i = 0;
    int ptr = kernel->machine->ReadRegister(4);
    char *buffer = User2System(ptr, MAXLENGTH);
    while (buffer[i] != '\0') {
        kernel->synchConsoleOut->PutChar(buffer[i++]);
    }
}

// https://en.wikipedia.org/wiki/Linear_congruential_generator

void SysRandomNum() {
    int seed = kernel->stats->totalTicks;
    RAND_NUM = (RAND_NUM * seed + CONST_C) % MODULUS;
    kernel->machine->WriteRegister(2, RAND_NUM);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
