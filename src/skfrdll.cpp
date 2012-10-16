/*
Copyright (c) 2011, OWNER: Gérard Penet
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

Neither the name of the OWNER nor the names of its contributors 
may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 

IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
// skfrdll.cpp : définit les fonctions exportées pour l'application DLL.
//

#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#include "ratingengine.h"

#define DLLEXPORT __declspec(dllexport)

extern "C" DLLEXPORT int __stdcall ratePuzzle(char *ze, int * er, int * ep, int * ed, int * aig);
extern "C" DLLEXPORT void __stdcall setMinMax(int mined,int maxed, int minep, int maxep, int miner, int maxer, UINT filt);
extern "C" DLLEXPORT void __stdcall setParam(int o1, int delta, int os, int oq, int ot, int oexclude, int edcycles);
extern "C" DLLEXPORT int __stdcall setTestMode(int ot, char * logFileName);
extern "C" DLLEXPORT void __stdcall ratePuzzles(int nPuzzles, char *ze, int *er, int *ep, int *ed, int *aig, int *ir);

int __stdcall ratePuzzle(char *ze, int * er, int * ep, int * ed, int * aig) {
	return ratePuzzleC(ze, er, ep, ed, aig);
}
void __stdcall setMinMax(int mined,int maxed, int minep, int maxep, int miner, int maxer, UINT filt) {
	setMinMaxC(mined, maxed,  minep,  maxep,  miner,  maxer, filt);
}
void __stdcall setParam(int o1, int delta, int os, int oq, int ot, int oexclude, int edcycles) {
	setParamC(o1, delta, os, oq, ot, oexclude, edcycles);
}
int __stdcall setTestMode(int ot, char *logFileName) {
	return setTestModeC(ot, logFileName);
}
void __stdcall ratePuzzles(int nPuzzles, char *ze, int *er, int *ep, int *ed, int *aig, int *ir) {
	ratePuzzlesC(nPuzzles, ze, er, ep, ed, aig, ir); //parallel rating
}