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

//===================  all cells in the same  element(s )(can be type 2)
int TPAIRES::Bug_lock(int el) 
{EE.Enl("recherche  bug_lock"); 
BF16 clock=candnp_or - candp_or;  // locked candidate
EE.E(" clock=");EE.Enl(clock.String());
 if(!clock.f) return 0; 

P81 p1=T81t[tplus[0]],p2=T81t[tplus[1]];
int el1=p1.f->el,el2=p2.f->el; // use the row in priority
if(el<9) {el1=p1.f->pl+9; el2=p2.f->pl+9;}  // and col if row is "el"
 BF16 wc1=(p1.v.cand & el_par_ch[el1]) ,wce1=p1.v.cand-wc1,
	   wc2=(p2.v.cand & el_par_ch[el2]) ,wce2=p2.v.cand-wc2 ;
EE.E(p1.f->pt); EE.E(" el=");EE.E(el1+1);
EE.E(" wc1=");EE.Enl(wc1.String());
EE.E(p2.f->pt); EE.E(" el=");EE.E(el2+1);
EE.E(" wc2=");EE.Enl(wc2.String());


 if( ( wc1.QC()-2)  ||
	 ( wc2.QC()-2)  ) return 0;	 

 T81t[tplus[0]].Keep(wce1|clock);
 T81t[tplus[1]].Keep(wce2|clock);
 BugMess("3/4 a digit locked");
 Op.SetDif(57);return 1;
}

//============================================
 int TPAIRES::Bug3_4_Nacked(int el) 
{EE.Enl("recherche  bug3_4 Nacked"); 

USHORT ctl=ntplus,aig=1;  
P81 *pp;  USHORT elx; BF16 wcx, welim,annul; 

for(int i=0;i<ntplus;i++)
{pp = & T81t[tplus[i]];
 elx = pp->f->el;    if(el<9)  elx=pp->f->pl+9; 
 wcx = pp->v.cand & el_par_ch[elx]  ;
 annul = pp->v.cand -  wcx;
 //EE.E(t81f[tplus[i]].pt); EE.E(" el=");EE.E(elx+1);
 //EE.E(" wc=");EE.Enl(wcx.String());
 if( (wcx.QC() - 2) ) return 0;
 welim |= annul;
}	

return Nacked_Go(welim);
}
