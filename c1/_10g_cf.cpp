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
int TZCF::DeriveCycle(int nd,int nf,int ns,int npas)
{int icd=ic;  // to check if something new comes
 hstart=h; // copy to have the situation at the start
 PlusPhase(); // save for later comments in test mode
 zcx.Derive(nd,nf,ns);
 if(ic==icd) return 0;
 if(!npas) h.d.ExpandAll(h.dp); else h.d.ExpandShort(h.dp,npas);
 return 1;}



// put in the table if there is some room
// return 1 if no room
int TZCF::LoadVoid(USHORT cd1 ,USHORT cd2,WL_TYPE type,USHORT ph)
{ic++;
	//if(ic>=zcf_lim) {EE.Elimite("ZCF");return 1;}
 //f[ic++].Load(cd1,cd2,type,ph);
return 0;} 

// entry for basic weak link a - b
void TZCF::LoadBase (USHORT cd1 ,USHORT cd2)
{if(LoadVoid(cd1,cd2,wl_basic,0)) return;  
 Entrep (cd1<<1,cd2<<1);  
}
// entry for bi value  a = b thru a - b and ã - ~b   
void TZCF::LoadBivalue (USHORT cd1 ,USHORT cd2)
{if(LoadVoid(cd1,cd2,wl_bivalue,0)) return;  
 Entrep ((cd1<<1)^1,(cd2<<1)^1);  
 isbival.Set(cd1);isbival.Set(cd2);
}

// entry for weak link derived from a set  a -> b
void TZCF::LoadDerivedTag (USHORT tg1 ,USHORT cd2)
{if(LoadVoid(tg1,cd2,wl_set,iphase)) return;
 Plusp (tg1,cd2<<1);  //  a -> b
 }
// entry for event  a - b
void TZCF::LoadEventTag (USHORT tg1 ,USHORT cd2)
{if(LoadVoid(tg1,cd2,wl_event,iphase)) return;
 Plusp (tg1,(cd2<<1)^1); // a -> ~b
}
// entry for direct event  ~a - b
void TZCF::LoadEventDirect (USHORT cd1 ,USHORT cd2)
{if(LoadVoid(cd1,cd2,wl_ev_direct,iphase)) return;
 Plusp ((cd1<<1)^1,(cd2<<1)^1); 
}
