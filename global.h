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
#pragma once

#include "skfrtype.h"

// global variables used in .h files
extern USHORT 
	aigstop, ///<set to 1 will stop the current rating
	op0,	///<Used for debugging
	op1,	///<Used for debugging
	col;	///<GP Last used tag | PP size of last used tag

class FLOG;
extern FLOG EE;		///< file to output traces for debug

class OPSUDO;
extern OPSUDO Op;	///< option compiled from arguments

class TZPTLN;
extern TZPTLN zpln; ///< ???
class TP81;

class MMS;
extern MMS mms;
class UN_JEU;
extern UN_JEU un_jeu;
class TP81F;
extern TP81F tp81f;
class P81F;
extern P81F * t81f;	
class DIVF;
extern DIVF divf;
extern TP81 T81dep;
extern TP81 * T81,*T81C;		//standard names for main objects of the class
class P81;
extern P81 * T81t,*T81tc;		//and corresponding tables of cells  
                      // the corresponding tables are located in class JDK
class ZTOB;
extern ZTOB aztob;
class JDK;
extern JDK jdk;
class ULT;
extern ULT tult;
class TPAIRES;
extern TPAIRES zpaires;
class TIR;
extern TIR yt;
class CRIN;
extern CRIN ur;
class CRINT;
extern CRINT urt;
