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
void ZPTLN::Clear(){T81t[ig].Change(ch); }
void ZPTLN::Image(int no) {EE.Esp();if(no)EE.E("~");
                     EE.E(ch+1);EE.E(t81f[ig].pt); }

/* creates the table of candidates
   and the direct index cell+ digit -> cand
   looks for true candidates
   init table zcf
   set up the usefull limit for BFCAND and CB1024 
 */


void TZPTLN::Init()
{ip=1; candtrue.SetAll_0();
 for (int i=0;i<9*81;i++)indexc[i]=0;
 for (UCHAR i=0;i<81;i++)
   {if(T81t[i].v.ncand<2) continue; BF16 chs=T81t[i].v.cand;  
    for(UCHAR j=0;j<9;j++) if(chs.On(j))
    {zp[0].Charge(i,j); 
	 if(jdk.solution[i]==(j+'1')) candtrue.Set(ip);
	 indexc[81*j+i]= Charge0();
	}
   }
 col=2*ip; BFTAG::SetIsize(col); 
// BFCAND::SetIsize(ip);
}
/* just put zp[0] in the next position 
   check for free room just in case
   if no free room, aigstop is set to 1 thru Elimite
*/
USHORT TZPTLN::Charge0()
{if(ip>=zpln_lim){  EE.Elimite("TZPLN"); return 0;}
 USHORT ir=ip; zp[ip++]=zp[0]; return ir;}

/* send in TZCF all weak links
   if it is a bi value, then 2 weak links are created
      a - b  and   ~a - ~b
   one entry for 'X' mode and one entry for 'Y' mode
   in 'X' mode we check for no double entry

*/

// this is a process specific to 'Y' mode
// only cell bivalues + 
void  TZPTLN::CellStrongLinks()
{ for(int i=0;i<81;i++) if(T81t[i].v.ncand==2) 
  {iptsch=0; 
   for(int ich=0,jp=i;ich<9;ich++,jp+=81)
	    {int j=indexc[jp];if(j)  ptsch[iptsch++]=j;  }
	 zcf.LoadBivalue(ptsch[0],ptsch[1]);
  }
}
/* generate weak and strong links from cells.
   if biv=1, no generation of strong links
   (if not, biv=3)
*/
void  TZPTLN::CellLinks() 
{ el=0; // to shunt the filter in WeaklinksD
 for(int i=0;i<81;i++) if(T81t[i].v.ncand<2) continue;
else 
  {iptsch=0;  
   for(int ich=0,jp=i;ich<9;ich++,jp+=81)
    {int j=indexc[jp];if(j) ptsch[iptsch++]=j;   }
   if(iptsch==2) { zcf.LoadBivalue(ptsch[0],ptsch[1]);
                   zcf.LoadBase(ptsch[0],ptsch[1]);}
   else  if(iptsch)WeakLinksD();
 }
}
void  TZPTLN::RegionLinks(USHORT ich,int biv)
{for (el=0;el<27;el++)
   { iptsch=0;  if(aztob.tchbit.el[el].eld[ich].n <2 )continue;
     for(int i=0;i<9;i++)
       {USHORT w=indexc[divf.el81[el][i]+81*ich]; 
		if(w) ptsch[iptsch++]=w;}
	if(iptsch==2 && biv) {zcf.LoadBivalue(ptsch[0],ptsch[1]);
	                      zcf.LoadBase(ptsch[0],ptsch[1]);}
	else    if(iptsch)WeakLinksD();
   }
}
void  TZPTLN::WeakLinksD()
{for(int j=0;j<iptsch-1;j++)// look for all pairs
     {USHORT p1=ptsch[j],ig1=zp[p1].ig;
      for(int k=j+1;k<iptsch;k++)
       { USHORT p2=ptsch[k],ig2=zp[p2].ig;
	     // don't generate twice if box/row or box/column 
	    if(el>17 &&   // it is a box
		   (t81f[ig1].el==t81f[ig2].el  ||  t81f[ig1].pl==t81f[ig2].pl ) )continue;
	   zcf.LoadBase(p1,p2)    ;
       }  }}

//---------- gen sets of candidates in a row column box 
void TZPTLN::GenRegionSets()     // can only be one per row col box, only if more than 2 candidates
{  USHORT mch[10];       
   for( int ich=0;ich<9;ich++) for (int el=0;el<27;el++)
   {USHORT nmch=aztob.tchbit.el[el].eld[ich].n,ipts=0;;
	if(nmch<3) continue; // minimum set size is 3
	BF81 zel=divf.elz81[el]&jdk.c[ich];  int j;
    for(j=1;j<ip;j++)
     {if(zp[j].ch-ich )continue;
      if(zel.On(zp[j].ig) ) mch[ipts++]=j; 
     }
     zcx.ChargeSet(mch,nmch,CH_base);
    }
}
void  TZPTLN::GenCellsSets()
{for(USHORT i=0;i<81;i++)
  {USHORT n= T81t[i].v.ncand; if(n<3 || n>chx_max) continue;    
   BF16 ch8=T81t[i].v.cand;  
   USHORT nm=0,tm[9];    for(int j=0,jp=i;j<9;j++,jp+=81) if(ch8.On(j))
     {tm[nm++]=indexc[jp];}
     zcx.ChargeSet(tm,nm,CH_base);  
    }  }

void TZPTLN::Liste()
{EE.Enl("Liste des candidats ");
 for(int j=1;j<ip;j++)  {zp[j].Image();   EE.Enl();}  }

void TZPTLN::ListeIndexc()
{EE.Enl("Liste des candidats via index ");
 for(int i=0;i<81;i++) for(int ich=0;ich<9;ich++) 
 {USHORT j=indexc[ich*81+i];
 if(j)  {zp[j].Image();   EE.Enl();}  }
}

 void  TZPTLN::PrintPath(USHORT * tp,USHORT np)
{if(!Op.ot) return; EE.Echem();
 for(int i=0;i<np;i++)
  {ImageTag(tp[i]);
  if(i<np-1) if(i&1)EE.Esl(); else EE.Ewl(); } 
 EE.Enl();}

 void  TZPTLN::PrintImply(USHORT * tp,USHORT np)
{if(!Op.ot) return; EE.Echem();
 for(int i=0;i<np;i++)
  {USHORT px=tp[i];  ImageTag(tp[i]);
  if(i<np-1) EE.E(" -> ");  } 
 EE.Enl();}

void  TZPTLN::PrintListe(USHORT * tp,USHORT np,int modetag)
{if(!Op.ot) return; EE.E("candidats");
 for(int i=0;i<np;i++)
  {USHORT px=tp[i];  
  if(modetag) ImageTag(px); else  zp[px].Image();
  EE.Esp();  } 
 EE.Enl();}
