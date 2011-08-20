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
int TPAIRES::Bug3(int el) 
{if((ntplus==2) && Bug_lock(el)) return 1;
 if(el<18) 	return  Bug3_4_Nacked(el) ;
EE.Enl("recherche en boite"); 

      // we have now a box and not 2 cells with a digit locked

if(ntplus>3) return 0; // would not work with that process
      // look first row and col with only one cell
BF16 wrow,wcol;  // on cherche parite de row/col
P81 *pp;  
USHORT elx,phx[5];
for(int i=0;i<ntplus;i++) // first look for parity
    {pp = & T81t[tplus[i]]; phx[i]=0;
     BF16 wr(pp->f->el), wc(pp->f->pl);
     wrow ^= wr; wcol ^= wc; // change parity
     }
 BF16 wcx, welim,annul,wpar; 

EE.Enl("recherche en boite phase 1"); 

for(int i=0;i<ntplus;i++)
{pp = & T81t[tplus[i]]; 
if(wrow.On(pp->f->el) ) elx = pp->f->el;    
 else if(wcol.On(pp->f->pl))  elx=pp->f->pl+9; 
 else continue;// not processed the 
 phx[i]=1;
 wcx = pp->v.cand & el_par_ch[elx]  ;
 annul = pp->v.cand -  wcx;
 wpar^=wcx;  // to adjust parity for the last if needed
 if( (wcx.QC() - 2) ) return 0;
 welim |= annul;
}	 
EE.E("recherche en boite phase 2"); 
EE.E(" wpar");EE.Enl(wpar.String());

 wpar ^=  el_par_ch[el]  ;  // adjust parity in the box

// finish the task if one has no row/col free
for(int i=0;i<ntplus;i++)
{if(phx[i] )continue;// done
 pp = & T81t[tplus[i]]; 
  wcx = pp->v.cand & wpar  ;
 annul = pp->v.cand -  wcx;
 EE.E(t81f[tplus[i]].pt); EE.Esp();
 EE.E(" wpar");EE.E(wpar.String());
 EE.E(" wcx");EE.Enl(wcx.String());
  if( (wcx.QC() - 2) ) return 0;
 welim |= annul;
}	 

return Nacked_Go(welim);
}


int TPAIRES::Nacked_Go(BF16 welim)
{            //we look now for  "naked locked sets"
EE.E("recherche  bug3_4 Nacked ok to go welim= "); EE.Enl(welim.String());
int nelim=welim.QC(); // look for naked in increasing order
if(nelim <2  || nelim>5) return 0;
if(brat==58 && nelim<3) //then search for naked 2
 for(int i=0;i<ntpa;i++) if(T81t[tpa[i]].v.cand.f==welim.f)// we found it
 {int ir=0; for(int j=0;j<ntpa;j++) if(j-i)
                ir+=T81t[tpa[j]].Change(welim);
  if(ir) {BugMess("type 3/4 naked pair");Op.SetDif(58); return 1;}
 }
if(brat ==59)         // look for triplet
for(int i1=0;i1<ntpa-1;i1++)for(int i2=i1+1;i2<ntpa;i2++)
  {BF16 ww=welim | T81t[tpa[i1]].v.cand | T81t[tpa[i2]].v.cand;
   if(ww.QC()-3) continue; // if not we got it
   int ir=0; for(int j=0;j<ntpa;j++) if((j-i1)&&(j-i2))
                ir+=T81t[tpa[j]].Change(ww);
   if(ir) {BugMess("type 3/4 naked triplet"); Op.SetDif(59); return 1;}
  }// end triplet
if(brat ==60)                    // look for quad
for(int i1=0;i1<ntpa-2;i1++)for(int i2=i1+1;i2<ntpa-1;i2++)for(int i3=i2+1;i3<ntpa;i3++)
  {BF16 ww=welim | T81t[tpa[i1]].v.cand | T81t[tpa[i2]].v.cand| T81t[tpa[i3]].v.cand;
   if(ww.QC()-4) continue; // if not we got it
   int ir=0; for(int j=0;j<ntpa;j++) if((j-i1)&&(j-i2)&&(j-i3))
                ir+=T81t[tpa[j]].Change(ww);
   if(ir) {BugMess("type 3/4 naked quad"); Op.SetDif(60);return 1;}
  }// end quad
 if(brat ==61)                    // look for (5)
for(int i1=0;i1<ntpa-3;i1++)   for(int i2=i1+1;i2<ntpa-2;i2++)
for(int i3=i2+1;i3<ntpa-1;i3++)for(int i4=i3+1;i4<ntpa;i4++)
  {BF16 ww=welim | T81t[tpa[i1]].v.cand | T81t[tpa[i2]].v.cand
                  | T81t[tpa[i3]].v.cand | T81t[tpa[i4]].v.cand ;
   if(ww.QC()-5) continue; // if not we got it
   int ir=0; for(int j=0;j<ntpa;j++) if((j-i1)&&(j-i2)&&(j-i3)&&(j-i4))
                ir+=T81t[tpa[j]].Change(ww);
   if(ir) {BugMess("type 3/4 naked (5)");Op.SetDif(61); return 1;}
  }// end (5)
return 0;}