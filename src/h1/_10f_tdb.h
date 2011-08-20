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

class TDB{
public:  
static BFTAG done[50]; // already in the path
static USHORT ich,mode,length,mmfin;
static PATH path, pathr;
static USHORT   maxsearch,  // external limitation for path search
	            maxlength,  // search limit as of the existing rating
				maxichain,  // ongoing search value
	            parsediag,  // debugging only
				parsecount; // length in parsing mode
static BFCAND tbf_end,tbf_endok;// one for each way
 

BFTAG t[BFTAG_BitSize],parents;	


void Init(){t[0].SetAll_0();
            for(int i=1;i<BFTAG_BitSize;i++) {t[i]=t[0];} }
void ExpandAll(TDB & from);
void ExpandShort(TDB & from,int npas);
void AllParents(TDB & from);
int SearchEliminations(TDB & from,BFTAG & elims);
inline void Set(int i, int m) {t[i].Set(m);};
inline int Is(int i,int m){return t[i].On(m);};
inline int IsConflit(int m1, int m2) {return Is(m1,m2^1);}
inline int IsOu (int m1, int m2) {return Is(m1^1,m2);}

static inline void InitParsing(){maxsearch=maxlength;}
static inline void SetParsingMini(USHORT x)
           {if(x<maxlength) maxsearch=x;else maxsearch=maxlength;}
inline void StoreTbf(){tbf_endok=tbf_end;}
inline void GetBackTbf() {tbf_end=tbf_endok;}


void Parents(USHORT x){parents.SetAll_0();
                  for(int i=2;i<col;i++) if(t[i].On(x)) parents.Set(i);}
//void Copy_Path(){pathr=path;}
//void Copy_Back){path=pathr;}                 
                  

//void Direct_Path(int m1, int m2, BFTAG & parents);


int Plus (int m1,int m2)  {if (!t[m1].On(m2))
		  { t[m1].Set(m2);  return 1; }else return 0;};
int  Entre(int m1,int m2){ return (Plus(m1,m2^1)+ Plus(m2,m1^1));};
void Image();
private:
//void Direct_Step();

//int Length();

};

 BFTAG TDB::done[50]; 
USHORT  TDB::ich, TDB::mode, TDB::length, TDB::mmfin, TDB::maxlength;
PATH   TDB::path,TDB::pathr;
USHORT TDB::maxichain,TDB::maxsearch,TDB:: parsediag,TDB:: parsecount;
BFCAND TDB::tbf_end,TDB::tbf_endok; 

