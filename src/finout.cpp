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
#include "stdafx.h"
#include "global.h"
#include "finout.h"



FINPUT::FINPUT(){open=endf=0;}
void FINPUT::SetName(char * nn){namex=nn;}


char * FINPUT::namex="puz";

int FINPUT::OpenFI()
{
	if(open) return 0; 
	open=1;
	char zn[200]; 
	strcpy_s(zn,198,namex); 
	int ll=strlen(zn);
	strcpy_s(&zn[ll],30,".txt"); // add file type
	ifstream::open(zn); 
	if(is_open()) return 0 ;
	//Console::WriteLine("problem in open input");
	cerr<<"problem in open input\n";
	endf=1;
	return 1; 
}

void FINPUT::CloseFI()
{
	if(endf) return; 
	ifstream::close();
	endf=1;
}

int FINPUT::GetPuzzle(char * zp)
{
	if(endf) return 0;
	char ze[400]; // big buffer to avoid problems linked to getline
	while(!eof() )
	{
		getline(ze,300); 
		ze[301]=0;  
		int i= strlen(ze); 
		if(i>250)
		{
			endf=1;return 0;
		}
		if(i<81)continue; 
		zp[81]=0; // file truncated to 81
		// standard file  clues or "." minimum 17 clues
		int n=0;
		for(int i=0;i<81;i++) 
		{
			char c=ze[i]; 
			if(c<'0' || c>'9') zp[i]='.'; 
			else {n++;zp[i]=ze[i];}
		}
		if(n<17) continue; 
		return 1;
	}// end while
	endf=1; 
	return 0;
}

char* FOUTPUT::zpuzx;
 
int FOUTPUT::OpenFO1()  
	{
		char zn[200]; 
		strcpy_s(zn,198,FINPUT::namex); 
		int ll=strlen(zn);
        strcpy_s(&zn[ll],30,"_rat.txt");
	    return OpenFO(zn); 
	}

int FOUTPUT::OpenFO2() 
{
	char zn[200]; 
	strcpy_s(zn,198,FINPUT::namex); 
	int ll=strlen(zn);
    strcpy_s(&zn[ll],30,"_N_rat.txt");
	return OpenFO(zn); 
}

int FOUTPUT::OpenFO(char * nam)
{
	if(open) return 0; 
	open=1;
	endf=0;
	ofstream::open(nam); 
	if(is_open()) 
		return 0;
	//Console::WriteLine("problem in open output");
	cerr<<"problem in open output\n";
	open =0;
	endf=1;
	return 1;
}

void FOUTPUT::PrintErEpEd(int er,int ep,int ed)
    {(*this)<<zpuzx<<" ";
		int era=er/10,erb=er%10,epa=ep/10,epb=ep%10,eda=ed/10,edb=ed%10;
		(*this)	 <<"ED="<<era<<"."<<erb<<"/"<<epa<<"."<<epb<<"/"<<eda<<"."<<edb; 
	} 

