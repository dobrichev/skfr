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

#include "stdafx.h"    //system includes and most general dats and includes
#include "skfrtype.h"
#include "finout.h" //declarations files for the program
#include "opsudocmd.h"
#include "ratingengine.h"
#include "utilities.h"
// global variables
FINPUT finput;
FOUTPUT foutput,se_refus;
OPSUDOCMD Op;
#include "_00_Batch_Start.cpp" // main routine initial and command line parsing
#include "_00_Batch_Go.cpp"    // main routine processing puzzles

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<main file
////! Main : lauch initialization and if ok the rating of the puzzle batch
//int main(array<System::String ^> ^args)
//{
//	if(Batch_Start(args)) 
//		Batch_Go();
//	return 0;
//}

int main(int argc, char *argv[]) {
	if(Batch_Start(argc, argv)) 
		Batch_Go();
	return 0;
}

