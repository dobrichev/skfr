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
/* an attempt to design an alternative process for the search 
in dynamic mode and later
*/

class CANDGO
{public:
 USHORT aig,ret_code,npas;
 int opp;
 short tsets[640];
 USHORT tx[50][100],itx[50];
 USHORT tret[300],pasret[300],itret;
 BFTAG steps[50],cumsteps[50],allsteps, * to;  
 BFTAG  *cum ,* step; 
 USHORT *ta,*tb,ita,itb;
 CANDGO(){opp=0;} // print option in nested
 TDB dpn; // in nested mode, dynamic set of primary links
                        // look first for direct 
 
 int GoCand(USHORT tag);  // locate the contradiction
 int GoOne(USHORT tag,BFTAG tagse); // find all targets
 void GoSets();
 int GoBack(USHORT tag,int pr);  // compute the length for one chain 
 int GoBackNested(USHORT tag,int pr);  // compute the length for one chain 
 int GoNested(USHORT cand,BFTAG * tagnot,USHORT base);  // locate the contradiction
 void GoNestedWhile(USHORT tag,USHORT base);
 void Gen_dpn(USHORT tag);
 void NestedForcing(BFTAG & elims);
 void NestedMulti(BFTAG & elims);
};

/* here weknow that a contradiction must occur 
   if the path is small enough
   */

int CANDGO::GoCand(USHORT tag)
{if(Op.ot && op1){EE.E("gocand killing"); zpln.ImageTag(tag);EE.Enl();
	             }
 to=zcf.dpbase.t; // we use the table without derivations
 for(int i=0;i<640;i++) tsets[i]=0;
 npas=0;
 steps[0].SetAll_0(); steps[0].Set(tag); 
 allsteps=cumsteps[0]=steps[0];  
 tx[0][0]=tag; itx[0]=1; // initial is tag to go forward

 ret_code=0;   aig=1;
 int maxpas=(tchain.maxlength>52)?50:tchain.maxlength-2;
 while(aig && npas++<=maxpas)  
 {	 aig=0; 
 cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
 ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;
                        // look first for direct 
  if(op0){EE.E("step");EE.Enl(npas);
          //cum->Image("cumstep",0);
		  //steps[npas-1].Image("step precedent",0); 
          }

   
   for(int it=0;it<ita;it++)
     {BFTAG x=to[ta[it]]- allsteps;	  // still free and in the overall path
      if(x.IsNotEmpty()) 
        {(*step)  |= x; // flag it in the BFTAG and load in new
	     allsteps |= x; // and in the total 
         USHORT ty[30],ity=0; x.String(ty,ity);
	     for(int i=0;i<ity;i++) tb[itb++]=ty[i];
		 aig=1;}    
     }
   GoSets();                    // look now for sets

  if(op0 && Op.ot){EE.E("fin step=");EE.E(npas);
                   step->Image("step ",0); }
  cumsteps[npas]=cumsteps[npas-1] | (*step);
  itx[npas]=itb;
  // check for a contradiction in that lot (stop at first)
  for(int i=0;i<itb;i++) 
  {USHORT tgx=tb[i];
   if(allsteps.On(tgx) && allsteps.On(tgx^1))
   {if(op0){EE.E("found active");zpln.ImageTag(tgx);
            EE.E(" step="); EE.Enl(npas);   
            }
	   // we compute back the length and show the sequence 
    if(maxpas>npas) maxpas=npas; // limit the process to 1 more step
    int l1=GoBack(tgx,0),l2=GoBack(tgx^1,0);
	if((!l1) || (!l2)) continue; // should not be
	int ratch=tchain.GetRating(l1+l2,tag>>1);
    if(ratch) // chain is accepted load it (more comments in test mode)
      {if(Op.ot) // this can be complex and will be developped later
        { EE.E("chain plus killing "); zpln.Image(tag>>1);   EE.Enl();	     
          EE.E("chain 1 ");   GoBack(tgx,1) ;
		  EE.E("chain 2 ");  GoBack(tgx^1,1);
		 }
	   tchain.LoadChain(ratch,"chain plus",tag>>1);
	  }
   }
  }
  if(op0 && !aig){EE.E("fin aig=0 pour step=");EE.Enl(npas);}
 }// end while
return ret_code;}


//---------------------------------------------


int CANDGO::GoOne(USHORT tag,BFTAG tagse)
{if(op0 && Op.ot  ){EE.E("goone for "); zpln.ImageTag(tag); 
	tagse.Image("killing one of these",0);
	             }
 to=zcf.dpbase.t; // we use the table without derivations
 for(int i=0;i<640;i++) tsets[i]=0;
 npas=0;
 steps[0].SetAll_0(); steps[0].Set(tag); 
 allsteps=cumsteps[0]=steps[0];  
 tx[0][0]=tag; itx[0]=1; // initial is tag to go forward

 aig=1;
 int maxpas=(tchain.maxlength>52)?50:tchain.maxlength-2;
 while(aig && npas++<=maxpas)  
 {	 aig=0; 
  cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
  ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;
                        // look first for direct 
  if(op0){EE.E("step");EE.Enl(npas);
          cum->Image("cumstep",0);
		  steps[npas-1].Image("step precedent",0); 
          }

   
   for(int it=0;it<ita;it++)
     {BFTAG x=to[ta[it]]- allsteps;	  // still free and in the overall path
      if(x.IsNotEmpty()) 
        {(*step)  |= x; // flag it in the BFTAG and load in new
	     allsteps |= x; // and in the total 
         USHORT ty[30],ity=0; x.String(ty,ity);
	     for(int i=0;i<ity;i++) tb[itb++]=ty[i];
		 aig=1;}    
     }
   GoSets();                    // look now for sets

  if(op0 && Op.ot){EE.E("fin step=");EE.E(npas);
                   step->Image("step ",0); }
  
  cumsteps[npas]=cumsteps[npas-1] | (*step);
  itx[npas]=itb;
  // check for all the target found
  if(op0){BFTAG ww=cumsteps[npas]&tagse;
          if(ww.IsNotEmpty())
		    {EE.E("fin step=");EE.Enl(npas);
		     ww.Image("trouvés ",0);}
         }
  BFTAG w=tagse-cumsteps[npas]; if(w.IsEmpty()) return 1;
  if(op0 && (!aig)){EE.E("fin aig=0 pour step=");EE.Enl(npas);}
 }// end while
return 0;}



//--------------------------------------------------


int CANDGO::GoBack(USHORT tag,int pr)
{if(op0){EE.E("goback");zpln.ImageTag(tag);EE.Enl();}
	USHORT itret1=0;  itret=0;
 BFTAG bf; 
 tret[itret++]=tag;bf.Set(tag);
  while(itret1<itret && itret<150) // solve each entry back
 { USHORT x=tret[itret1],aig=1;; // first locate where x has been loaded
   if(op0 && Op.ot) {EE.E("go back look for "); zpln.ImageTag(x);
              EE.E(" itret1=");EE.E(itret1);EE.E(" itret=");EE.Enl(itret);}
	for(int i=0;i<=npas;i++) if(steps[i].On(x))
      {aig=0;
	   if(i)   // not initial assumption
	   if(!tsets[x]) // this is a direct step
	       {USHORT z=0;
	         for(int i2=0;i2<i;i2++)
			 for(int j=0;j<itx[i2];j++) // look for the earliest  possible parents
	       // take in priority one already there  
	         {USHORT y=tx[i2][j]; 
			  if(to[y].On(x)) 
			       {if(!z)z=y;   if(bf.On(y)) {z=y;break;}			     } 
	         }
            if(z && bf.Off(z)) {tret[itret++]=z;bf.Set(z);}
           }
	   else   // it comes from a set, we know which one
	        {ZCHOIX chx=zcx.zc[tsets[x]];
	        if(op0){EE.E("set");chx.Image(); EE.Enl();}
			int n=chx.ncd; if(chx.type==CH_set) n--; // for a set, forget the event
		   for(int j=0;j<n;j++) 
		   {USHORT y=chx.tcd[j]<<1; if(y==x) continue;
		    y^=1; if(bf.Off(y)){tret[itret++]=y;bf.Set(y);}
		   }
	      }
         i=100;// force end of process
	   }  
  if(aig || itret>150) return 0; // not found, should never be
  itret1++;
  if(op0 && Op.ot) {EE.E("go back end step   itret1=");EE.E(itret1);EE.E(" itret=");EE.Enl(itret);}
 }
 if(pr && Op.ot)  // printing in increasing order of generation
    {EE.E("seq ");
	 for(int i=0;i<=npas;i++) for(int j=0;j<itret;j++) 
		 if(steps[i].On(tret[j])) {zpln.ImageTag(tret[j]); EE.Esp();}
      EE.Enl();
    }
return itret;}

