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
// skfr.cpp : main file for sudoku fast rating
// processing exclusively a batch file 
// output = input valid + rating   default input="puz.txt"  
// default output1="puz_rat.txt"  default output1="puz_N_rat.txt"

typedef unsigned short int  USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;

using namespace System;
using namespace std;

extern USHORT
	aigstop, /// can be set to 1 anywhere to stop the process as soon as possible for one puzzle
	op0,	///<Used for debugging
	op1,	///<Used for debugging
	col;	///<GP Last used tag | PP size of last used tag
extern enum DeriveLength{dldirect=3,dlfirst=8,dlsecond=14,dlhigh=18};

#include "stdafx.h"    //system includes and most general dats and includes
//#include "..\skfr\skfr.h"
#include "ratEnum.h"            // enum list of solving techniques
#include "bitfields1.h"		// bitfields are grouped in this file
class BF_CONVERT;
 extern BF_CONVERT bfconv;
//#include "bitfields2.h"		// bitfields are grouped in this file
#include "opsudo.h"             // storing and managing options
class OPSUDO;
 extern OPSUDO Op;

#include "finout.h"                // relay for printing only in tests
class FINPUT ;
 extern FINPUT finput;
class FOUTPUT;
 extern FOUTPUT foutput, se_refus;
#include "fsr.h"                // relay for printing only in tests
class FSR;
 extern FSR EE;
class DIVF ;
 extern DIVF divf;
class JDK;
 extern JDK jdk;
class TZPTLN;
 extern TZPTLN zpln;
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



#include "c\_00_Batch_Start.cpp" // main routine initial and command line parsing
//#include "c\_00_Batch_Go.cpp"    // main routine processing puzzles


// loop to process the batch and puzzles verifications

// local functions
 long GetTimeMillis();				// get   standard time in millisec
 void PrintTime(long ts,long te,int cons=0);    // Print elapsed time
 int Puz_Go(char *ze);				// processing a puzzle
int Serate(char *ze, char * ED);

//! Evaluation loop of puzzle and print the result for each puzzle
void Batch_Go()
{
	long tstart=GetTimeMillis();// for the overall elapsed time
	char ze[82];
	foutput.zpuz=ze;		// prepare to output the puzzle string
	while(finput.GetPuzzle(ze)) 
	{   char ED[16];
		long tpuz_start=GetTimeMillis(); // for puzzle elapsed time
		int ir=Puz_Go(ze); 
//		int ir=Serate(ze,ED);
		if(Op.os)    // split option
        {
			if(ir<4) se_refus<< ze 
		//		<< " " << Op.ermax << " " << Op.epmax<< " " << Op.edmax
				<< endl; 
			else  foutput << ze 
		//		<< " " << Op.ermax << " " << Op.epmax<< " " << Op.edmax 
				<<endl; 
		}
		else
		{  
			Op.PrintErEpEd(); // result
			if(Op.ptime && Op.ermax>=10 && Op.ermax<=120 ) 
			{
				long tpuz_end=GetTimeMillis();
				PrintTime(tpuz_start,tpuz_end);
			}
		if(Op.ermax<10 || Op.ermax>120) se_refus << endl;
		else  foutput<<endl;
		}
	}
	// print global elapsed time
	long tend=GetTimeMillis();
	PrintTime(tstart,tend,1); // foutput<<endl;
}

extern long tdebut; // for debugging purpose start time in jdk traite base

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<main file
//! Main : lauch initialization and if ok the rating of the puzzle batch
int main(array<System::String ^> ^args)
{
	if(Batch_Start(args)) 
		Batch_Go();
	return 0;
}
