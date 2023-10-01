#pragma once

#include <types.h>

#define COM1 0x3F8

int SeInit();
void SeSend(char* pStr);