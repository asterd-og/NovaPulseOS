#pragma once

#include <types.h>

#define darkBlue     0x000080
#define darkYellow   0x808000
#define gray         0x808080
#define darkRed      0x800000
#define magenta      0xaa00aa
#define orange       0xff8000
#define lightRed     0xff8080
#define darkGreen    0x008000
#define cyan         0x00aaaa
#define lightGreen   0x80ff00
#define lightBlue    0x8080ff
#define lightYellow  0xffff80
#define white        0xffffff

typedef enum {
    Good,
    Info,
    Bad
} LogStatus;

void LogWrite(LogStatus status, char* pMsg, ...);