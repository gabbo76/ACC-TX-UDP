#ifndef READDATA_H
#define READDATA_H

#include "../include/SharedFileOut.h"
#include <windows.h>
#include <tchar.h>
#include <iostream>


int InitSM();
void DismissSM();
void ReadPhysics(SPageFilePhysics * outData);
void ReadGraphics(SPageFileGraphic * outData);
void ReadStatic(SPageFileStatic * outData);

#endif