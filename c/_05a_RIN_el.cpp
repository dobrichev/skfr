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

/* full processing for 2 cells with adds in one object UR or UL
   can be bivalue, "hidden locket set" or "locked set"
   always the lowest rating found
   the rule has been copied from SE code analysis adjusted to lksudokuffixed8 veersion.
   here "basis" is the rating entry for the specific object
       UR basi=4.5 
       UL up to 6 cells basis=4.6 (+0.1)  7_8 cells 4.7 (+0.2)  10_.. 4.8 (+0.3)
   basis        one digit bivalue
   basis +0.1 (n-1) hidden/nacked sets of "n" cells

   for hidden and naked sets, the lowest rating is taken depending on "n" values 

summary of rating having "equalled" hidden and naked as in lksudoku 1.2.5.0

 URUL 
 cells -->  4    6    8   >=10

 pair     4.6  4.7  4.8  4.9    action 2
 triplet  4.7  4.8  4.9  5.0    action 3
 quad     4.8  4.9  5.0  5.1    action 4


*/
int CRIN::T2(USHORT action)
{int ir1=0,ir2=0,iel=GetElPlus();
 if(iel>=0){ir1=T2_el(iel,action);	if(ir1==1) return 1;}
 int eb=t81f[pp1].eb;
 if(eb==t81f[pp2].eb)ir2= T2_el(eb+18,action );
 if(ir2==1) return 1;
 if((ir1+ir2)==0)return 0; else return 2;}



// same but el is now identified 
int CRIN::T2_el(USHORT el,USHORT action)
{//look mode 1 for a bivalue of one of the common digits
if(op0)
  {	ImageRI("");EE.E("UR/UL el ");EE.E(el+1 );EE.E(" action=");EE.E(action );
  EE.E(" pp1=");EE.E(t81f[pp1].pt );EE.E(" pp2=");EE.Enl(t81f[pp2].pt );
   }
if(action == 1) //   this is a "basis"  
  if(Jum(el,chc1)) 
    {int ir1=T81t[pp1].Change(chc2)+T81t[pp2].Change(chc2);
     if(ir1){;EE.E("UR/UL bivalue ");EE.Enl(chc1+1);return 1;}
    }
  else if(Jum(el,chc2)) 
   {int ir1=T81t[pp1].Change(chc1)+T81t[pp2].Change(chc1);
     if(ir1){EE.E("UR/UL bivalue ");EE.Enl(chc2+1 );return 1;}
   }

	// first look  for cells having a potential for hidden/direct locked sets
int ir=0; aig_hid=0; nth=ntd=nnh=0;
wh=wc;wd=wr;wnh=wr;wnd=wc;wdp.f=0;
zwel.SetAll_0(); // to prepare clearing of naked sets
for(int i=0;i<9;i++)
  {int ppi=divf.el81[el][i];if((ppi==pp1)||(ppi==pp2))continue;
   UNP v=T81t[ppi].v;  if(v.typ) continue; // given or assigned 
 	  	// count and store cells including common digits 
   zwel.Set(ppi);//will contain all active cells out of the UR at the end
   BF16  ww=v.cand&wc;
   if(ww.f)  {th[nth++]=ppi;  wh|=v.cand;
              if(ww.f==wc.f)aig_hid=1;} 
   else // naked locked set  can not have common digits	  
   { wnh|=v.cand; 
     tnh[nnh++]=ppi; // nnh to check at least one cell without common digits
     ww=v.cand&wr; 
     if(ww.f)  {td[ntd++]=ppi;  wd|=v.cand;} else wnd|=v.cand;
   }
 }
//ImageRI(" ");EE.E(" nth=");EE.E(nth );EE.E(" wh=");EE.Enl(wh.String() );

if(!nnh) return 0;//all cells have common digits nothing to do

//EE.E(" nth=");EE.E(nth );EE.E(" nnh=");EE.E(nnh );EE.E(" nd=");EE.Enl(ntd );


if(action<2) return 2; // store it if not basic
//  hidden pair 
if(nth<2 && (action==2))  //  hidden pair if active
 {if(T81t[th[0]].Keep(wc))     { EE.Enl("UR/UL hidden pair");return 1;}  }


 // we now look  for  a hidden or nacked set of the appropriate length
 
if(T2_el_set_hidden(action-1)) return 1;
if(T2_el_set_nacked(action-1)) return 1;
  
 return 0;
}
