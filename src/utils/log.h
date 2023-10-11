#pragma once

#include <types.h>

typedef enum {
    Good,
    Info,
    Bad
} LogStatus;

void LogWrite(LogStatus status, char* pMsg, ...);