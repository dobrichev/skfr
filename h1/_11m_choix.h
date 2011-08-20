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
/* new design for the "sets" table
   all entries in candidates mode + 'event' if any
   no duplicate check
   limit to size ZCHOIX_max
*/


#define zcxb_lim 100000
class ZCXB   // buffer for candidates + "events"
{public: USHORT zs[zcxb_lim],izs,izs_one;
 inline void Init(){izs=0;}
 inline void LockNestedOne() {izs_one=izs;}
 inline void StartNestedOne() {izs=izs_one;}
 void GetSpace(USHORT *(& ps),int n)
    {ps=&zs[izs];izs+=n; if(izs>=zcxb_lim){ps=0;EE.Elimite("ZCXB");}}
 
}zcxb;   



/* in SE we keep sets in candidate mode (index in TZPTLN)
   identical sets are sorted out before 
   types identified (enum list) are
   CH_base  cell or region list of candidates
   CH_set   event set, last "candidate" is the event 
*/
enum CHOIX_TYPE{CH_base,CH_set=4};
class ZCHOIX
{public:      
 USHORT *tcd,  // "set" table pointer to TCXI
        ix,   // index in TCHOIX 
		ncd,   // number of candidates + "event"
		type; // as of CHOIX_TYPE 

 int Prepare (USHORT * mi,USHORT nmi,CHOIX_TYPE ty,USHORT ixe)
	  {zcxb.GetSpace(tcd,nmi); if(tcd==0) return 0;// limite atteinte
       for(int i=0;i<nmi;i++)  tcd[i] = mi[i]; 
       ncd=nmi;ix=ixe;type=ty;return 1;}

 void GetCand(BFCAND & ms)// init à la charge de l'appelant
           {for(int i=0;i<ncd;i++) ms.Set(tcd[i]);}

 inline int ChoixBase(){return (!type);}
 void PrintInterditSimple(USHORT mi,USHORT derive){}//;
 void Image();
/* int ImageSimple();
 int Interdit(int nmx,int ftyp,CB1024 &wi);
 void ImageBasexx(int saut=0); 
 void ImageBase(){if(!ImageSimple()) ImageBasexx();}
 void ImageRemonte2(USHORT * m); // classique deux marques conflit
 
*/
 } ;
