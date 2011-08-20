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
#define zcx_lim 20000
 class TZCHOIX     
 { public:    
    ZCHOIX zc[zcx_lim];
    USHORT izc,     //index to zc
		   izc_one,
		   direct,  // 0 except if derivation from direct weak links
		   nmmin,nmmax;    //min  max  size seen for a set
    BFTAG tce[20],  // storing used BFTAG in a set analyzed
		   *t;       // pointer to zcf.h.d.t or zcf.h.dp.t depending on direct
	TDB allparents; // table for derivation
    void Init(){izc=1;zcxb.Init();nmmax=0;nmmin=10;direct=0;};	
    inline void LockNestedOne() {izc_one=izc;}
    inline void StartNestedOne() {izc=izc_one;}

    int ChargeSet (USHORT * mi,USHORT nmi,CHOIX_TYPE ty);
	int CopySet(int ie);


	void DeriveDirect(){direct=1;Derive(3,4,3); direct=0;}
    void Derive(int min,int max,int maxs); 
	void DeriveBase(ZCHOIX & chx);
	void DeriveSet(ZCHOIX & chx);

	int Interdit_Base80() ;	 

	int CheckGoNested1(BFTAG bftag,USHORT cand);

	 void Image()
		  {EE.E("\nimage fichier choix izc=");EE.Enl(izc);
	       for(int i=1;i<izc;i++)  {zc[i].Image();EE.Enl();}}
 
 /*  void ListeLiens();
    void ImpPoints(int i,USHORT m);
    int  Limites() {(*Fs)<<"liens limite "<<izc<< "sur"<<zcx_lim<<endl;
                    (*Fs)<<"liens linite zs "<<izs<< "sur"<<zcxb_lim<<endl;
                    return (izc>=limt);}*/
  }zcx ;  
 

