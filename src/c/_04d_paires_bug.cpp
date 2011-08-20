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
/* searching and processing all forms of bugs included in SE
   type 1 one extra cell  5.6
   type 2 2 extra cells with one same extra digit  5.7
   type 3 extra cells combined to a naked duo  5.8
   type 3 extra cells combined to a naked triplet 5.9
   type 3 extra cells combined to a naked quad 6.0 
   type 3 extra cells combined to a naked (5) 6.1
   type 4 extra cells (2) have a locked digit 5.7   */
//==============================
int TPAIRES::BUG() 
{EE.Enl("debut recherche bug");
aigun=0;
 if(ntplus>6|| aigpmax>4) return 0;  // maxi a vérifier 6 cases et 4 candidats
// set the parity of digits for bivalue cells in all elements
for(int i=0;i<27;i++) el_par_ch[i].f=0;
for(int i=0;i<ip;i++)
        {P81 p=T81t[zp[i].i8];  
         el_par_ch[p.f->el]^=p.v.cand; 
         el_par_ch[p.f->pl+9]^=p.v.cand; 
         el_par_ch[p.f->eb+18]^=p.v.cand;         }
//EE.Enl("debut recherche bug 1");
if(ntplus==1) return Bug1();
//EE.Enl("debut recherche bug 2");
if(Bug2()) return 1; // priority to bug 2
//EE.Enl("debut recherche bug 3 4");
if(Bug3a(58)) return 1;
return 0;}
//===================== calls 
int TPAIRES::Bug3a(int rat) 
{brat=rat;  // maximum authorized in that step
for(int i=0;i<27;i++)  if( zplus.EstDans(divf.elz81[i]))	  
{ntpa=nwp=0;// collect data in that row/col/box
 candp_or.f=candnp_or.f=candp_xor.f=candnp_xor.f=nwp=0;
 for(int j=0;j<9;j++)  
  {int i8=divf.el81[i][j]; P81 p=T81t[i8];
   if(zplus.On(i8)) // wplus has the same order as tplus
        {candnp_or|=p.v.cand;candnp_xor ^=p.v.cand;wplus[nwp++]=p.v.cand;}
   else if(p.v.ncand==2) 
         {candp_or|=p.v.cand;candp_xor ^=p.v.cand;tpa[ntpa++]=i8;}
  }
 aigun=1; if (Bug3(i)) return 1;//bug3 and bug4
 continue;//only one row/col/box
}
return 0;}


//===========================
int TPAIRES::Bug1() 
{ int i8=zplus.First();
 P81 p=T81t[i8];
 BF16  wc=p.v.cand & el_par_ch[p.f->el],
	    w=p.v.cand - wc;
 if(wc.QC()-2) return 0; //must be 2 to reach parity
 T81t[i8].Keep(w); // eliminate the others
 BugMess(" 1");
 return 1;}



//================================ cells in different objects or one digit
int TPAIRES::Bug2()  // any number of cells, but 6 seems very high
{if(ntplus>6 || aigpmax>3) return 0;  
 BF16 possible;possible.f=0x1ff;
 BF32 b18;b18.f=0; // find parity of cells in r/c
 for(int i=0;i<ntplus;i++)
  {P81F p1=t81f[tplus[i]];
   b18.f ^= 1<<p1.el; b18.f ^= 1<<(p1.pl+9);  }
 BF81 zw; zw.SetAll_1();
 for(int i=0;i<ntplus;i++) // analyse all cells
   { P81 p1=T81t[tplus[i]];zw&=p1.f->z;
     BF16 w1; w1.f=0;
	 if( b18.On(p1.f->el))	w1 =p1.v.cand - el_par_ch[p1.f->el];
	 if(!w1.f)// if nothing comes in row odd cells, try the column
	   {if( b18.On(p1.f->el)) return 0;
	     else    w1=p1.v.cand - el_par_ch[p1.f->pl+9] ;}
	 else if( b18.On(p1.f->pl+9))// check other direction
	  { BF16 w2=p1.v.cand - el_par_ch[p1.f->pl+9];
	   if(w1.f - w2.f) return 0; }
     possible &= w1;  }
 if(zw.IsEmpty()) return 0;// must have a comon control on some cells
 if(possible.QC()-1) return 0; // must be one common digit
// last check, parity ok everywhere
for(int i=0;i<27;i++) el_par2_ch[i]=el_par_ch[i];
for(int i=0;i<ntplus;i++) // change parity for all cells
{P81F p1=t81f[tplus[i]]; 
BF16 wch=T81t[tplus[i]].v.cand-possible;
 el_par2_ch[p1.el]^=wch;
 el_par2_ch[p1.pl+9]^=wch;
 el_par2_ch[p1.eb+18]^=wch;
}
for(int i=0;i<27;i++) if(el_par2_ch[i].f) return 0;; 
 
 
 // ok for bug type 2 clear the commonly controled  cells

int ir=0,ch=possible.First();; 
for(int i=0;i<81;i++)if(zw.On(i)) ir+=T81t[i].Change(ch);
if(ir){BugMess("2 same digit");Op.SetDif(57);}
return ir;}
//===========================================
void TPAIRES::BugMess(char * lib)
{EE.E("Bug type ");EE.Enl(lib);
T81C->Candidats();}