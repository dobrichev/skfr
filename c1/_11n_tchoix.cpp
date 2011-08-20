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


int TZCHOIX::ChargeSet (USHORT * mi,USHORT nmi,CHOIX_TYPE ty)
{if(nmi<2||aigstop) return 0;
 if(ty &&  nmi>(chx_max+1) ) return 0;
 if(!zc[0].Prepare(mi,nmi,ty,izc)) return 0;
if(izc<zcx_lim) {zc[izc++]=zc[0];  
                 if(nmi>nmmax)nmmax=nmi;  
				 if(nmi<nmmin)nmmin=nmi; return 1;}
EE.Elimite("ZCX");return 0;}

int TZCHOIX::CopySet (int i)
{if(izc<zcx_lim) {zc[izc++]=zc[i];  return 1;}
EE.Elimite("ZCX");return 0;}

 // multi chains version
 int TZCHOIX::Interdit_Base80() 
 {t= zcf.h.d.t;
 int ir=0;
  for (int ie=1;ie<izc;ie++)     // all active sets 
   {if(zc[ie].type-CH_base) continue;
    int n=zc[ie].ncd; USHORT *tcd=zc[ie].tcd ; 
	BFTAG tbt; tbt.SetAll_1();
     for(int  i=0;i<n;i++)  tbt &= t[tcd[i]<<1];
 
    if(tbt.IsNotEmpty()) // candidate(s) to clear found
	  {if(Op.ot&& op1){EE.E(" eliminations found in multi chain mode pour ");
	                 zc[ie].Image();EE.Enl();} 

	   for(int  j=3;j<col;j+=2)if(tbt.On(j)) // all tags assigned
	      {int tot_length=0; USHORT jj=j^1;// skip from assigned to eliminated
	       if(Op.ot && op0){EE.E(" Set killing "); zpln.ImageTag(jj); EE.Enl(); }
		   if(Op.ermax>85+n-3) // gofast if already far above
		    {zpln.Clear(jj>>1); ir++;
		   if(Op.ot){EE.E(" Set fast killing "); zpln.ImageTag(jj); EE.Enl();}
		    continue;}
		   for(int i2=0;i2<n;i2++)  
			   {   BFTAG wch=zcf.h.dp.t[jj]; 
		         USHORT end=(tcd[i2]<<1)^1;
				 if(wch.On(end))// this is a direct
				    {tot_length+=2; continue;}
		         int npasch=wch.SearchChain(zcf.h.dp.t,jj,end);
                 if(!npasch) EE.Enl(" 0 partial length "); // debugging message
				 tot_length+=npasch+2;
	     	   }
           int ratch=tchain.GetRatingBase(80,tot_length,jj>>1);
           if(ratch) // chain is accepted load it (more comments in test mode)
               {// in test mode  give the details for the chains
				// in that case, do it again and print
				if(Op.ot)for(int i2=0;i2<n;i2++) 
				  {  BFTAG wch=zcf.h.dp.t[jj]; 
				     USHORT end=(tcd[i2]<<1)^1;
					  if(wch.On(end))// this is a direct
				         {USHORT tt[2],itt=2; tt[0]=jj; tt[1]=end; 
					       zpln.PrintImply(tt,itt);continue;}
		             int npasch=wch.SearchChain(zcf.h.dp.t,jj,end);
                      USHORT tt[50],itt=npasch+2; 
                      wch.TrackBack(zcf.h.dp.t,jj,end,tt,itt,end);
                      zpln.PrintImply(tt,itt);
		          }
               tchain.LoadChain(ratch,"chain",jj>>1);	
               }
 	      } // end  for j
       } // end if
  }// end ie
 return ir;}
 
void TZCHOIX::Derive(int min,int max,int maxs)  
{if(max>nmmax) max=nmmax;   if(min<nmmin) min=nmmin;if(maxs>nmmax) maxs=nmmax;
 int maxt=(max>maxs)?max:maxs;

 if(Op.ot)
  {EE.E("debut Derive min= ");EE.E(min);EE.E("  max= ");EE.E(max);;EE.E("  maxs= ");EE.E(maxs);
   EE.E("  zcf.ic= ");EE.Enl(zcf.ic); }  

 if(direct) {t= zcf.h.dp.t; allparents.AllParents(zcf.h.dp);}
  else {t= zcf.h.d.t; allparents.AllParents(zcf.h.d);}// usually direct=0
  for (int ie=1;ie<izc;ie++)
   {int nnm=zc[ie].ncd;   
    switch (zc[ie].type)
	{case CH_base: if(nnm<=max) DeriveBase(zc[ie]); break;
	 case CH_set : if(nnm<=maxs)DeriveSet(zc[ie]); break;	
	 }   
   }
if(Op.ot)  {EE.E("end Derive  zcf.ic= ");EE.Enl(zcf.ic); }  
}
void TZCHOIX::DeriveBase(ZCHOIX & chx) // each candidate can be the target
{if(op0){EE.E("on traite"); chx.Image(); EE.Enl();     }

 USHORT * tcd=chx.tcd, nni=chx.ncd ; 
 BFTAG tcf2,tcf2f,tcft,bfset;   tcf2.SetAll_1(); tcft=tcf2;
 // bfset is the set itself in bf form for the exclusion of the set itself
 for(int i=0;i<nni;i++) bfset.Set(tcd[i]<<1);

 for(int i=0;i<nni;i++)    tce[i]=allparents.t[(tcd[i]<<1)^1]-bfset;  
 for(int i=0;i<nni;i++)
      {tcf2f=tcf2;  if(i<nni)for(int k=i+1;k<nni;k++) tcf2f=tcf2f&tce[k];
       if(tcf2f.IsNotEmpty())
         {for(USHORT j=2;j<col;j++)  if(tcf2f.On(j)) 
           { if( zcf.IsStart(j,tcd[i]<<1) )continue; // skip if defined		    
		  	 zcf.LoadDerivedTag(j,tcd[i]);
	       }// end j
         } //end if
       tcf2=tcf2&tce[i];if(tcf2.IsEmpty()) return;
	 }// end i
}

/* deriving a set creating an event
   the set must be in a dead state for a candidate in left to right mode
   then, the event is established and the event process is called
*/
void TZCHOIX::DeriveSet(ZCHOIX & chx) // only the "event" can be the target
{USHORT * tcd=chx.tcd, nni=chx.ncd-1 ; 
 BFTAG tcft,bfset;   tcft.SetAll_1();
 // bfset is the set itself in bf form for the exclusion of the set itself
 for(int i=0;i<nni;i++)    bfset.Set(tcd[i]<<1);
 for(int i=0;i<nni;i++)    tce[i]=allparents.t[(tcd[i]<<1)^1]-bfset;  
 for(int i=0;i<nni;i++)    tcft&=tce[i];
 if(tcft.IsNotEmpty()) // event established for any 'true' in tcft
	 {for(USHORT j=2;j<col;j++)  if(tcft.On(j))
           {if(   tevent.EventSeenFor(j,tcd[nni])) // just for diag
			{EE.E("diag choix");chx.Image();}
		  }// end j
	}// end if

}
