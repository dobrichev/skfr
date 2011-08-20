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

 /* In that process, we do in the same pass the search for
    x cycle;chain  or  y cycle;chain  or XY cycle;chain

	if nothing comes from  ?chain, there is no need to search the loop
	in case we have results from ?chain,
	we expand all and collect 
	  all eliminations ("true status"), 
	  all tags in loop
    
	for each elimination, we first look for the chain, then for the loop if any
    tchain takes care of the smaller ratings


  Note we look only in that process,  elimination found thru a "true"  state of a candidate
  nota : to get a "false" elimination we need
  ~a - ~b = b - ..... - x = ~x - ~a (only possible weak link ~x - ~y)
  but then  b - a = ~a - ~x = x ..... - b   always exists
*/

void TZCF::Aic_Cycle(int opx)  // only nice loops and solve them
            // first collect tags in loop
{//if(opx==2) h.dp.Image();
 BFTAG elims;
 int npaselim=h.d.SearchEliminations(h.dp,elims);
 if(!npaselim) return; //nothing to find
h.d.ExpandAll(h.dp); // 	
BFTAG xi;
xb.SetAll_0(); xi.SetAll_0();// collect tags in loop ok and "to eliminate in true state"
for(int i=2;i<col;i++) {if(h.d.Is(i,i)&& (h.d.Is(i^1,i^1)) )xb.Set(i);  
                        if(h.d.Is(i,i^1) && (!(i&1)) )xi.Set(i); }
if( xi.IsEmpty() )return;  
if(0 && Op.ot){xi.Image("candidates potential  eliminations",0);
                }

// now check all eliminations for a chain or a cycle    
for(int i=2;i<col;i+=2) if(xi.On(i))
{ if(0 && Op.ot){EE.E("\n\nanalyzing "); zpln.ImageTag(i); EE.Enl();}
	// look for a chain
  if( (opx-2))//  || (!Op.ocopybugs)) // skip Y chains in serate mode
  {
    BFTAG wch=h.dp.t[i]; int npasch=wch.SearchChain(h.dp.t,i,i^1);
    if(op0 && Op.ot){EE.E(" npasch= "); EE.Enl(npasch);}
    if(!npasch) continue; // should never happen
    int ratch=tchain.GetRatingBase((opx==3)?70:66,npasch+1,i>>1);
    if(ratch) // chain is accepted load it (more comments in test mode)
      {if(Op.ot){ ExplainPath(wch,i,i^1,npasch+2,i^1);  }
       tchain.LoadChain(ratch,"chain",i>>1);	
      }
  }
  //--------------------------- now look for a loop 
  BFTAG w=h.dp.t[i]&xb; 
  if(0 && Op.ot) w.Image("loop contacts",i);
  if(w.Count()<2) continue; // must have contact with 2 candidates ( false state )
  //in 'X' mode cycle eliminations are done only in the same region
 //first, collect all contacts in a table  and process all pairs
 USHORT tt[100],itt;  w.String(tt,itt);
 for(int i1=0;i1<itt-1;i1++)
	 {USHORT t1=tt[i1]^1; // t1 is turned to "true" 
	  for(int i2=i1+1;i2<itt;i2++)
      {USHORT t2=tt[i2];  // t2 kept to "false"
       if(h.dp.t[t1].On(t2)) //  same cell or region
        {// except in 'XY' mode, must be the same digit
		 if(opx-3)
			{ZPTLN candt1=zpln.zp[t1>>1],candt2=zpln.zp[t2>>1];
             if(candt1.ch-candt2.ch) continue; 
		     }
		 // now we want the xycle crossing t1 and t2.
		  // by construction, this is a weak link, never a strong link
         BFTAG wstart; wstart.Set(t2);  //we start a search with t1 -> t2
         int npascycle= wstart.SearchCycle(h.dp.t,t1,xb) ;
         if(!npascycle) continue; // could  ben ??
         int rat=tchain.GetRatingBase((opx==3)?70:65,npascycle+1,i>>1);
         if(!rat) continue;// chain is accepted load it (more comments in test mode)
         if(Op.ot){ ExplainPath(wstart,t1,t1,npascycle+2,t2);  }
         tchain.LoadChain(rat,"cycle",i>>1); 
	    }
	   else if(opx==2) // not allowed in 'X' mode and no interest in 'XY'mode
          Aic_Ycycle(t1,t2^1,xb,i>>1); // both turned to true 

	  } }// end for i1 i2

  }// end for i
}

/* we are looking in 'Y' mode for a loop crossing 2 points inside the loop.
   me must find the shortest loop
   that processs can be very long, so it must be designed carefully
   the general idea is the following
   look in priority for a candidate (t1 or t2) with only 2 possible starts in the loop.
   If this exists, take that point as start, if not, 
   take the smallest number of starts and try all of them

   in the search, the strategy is to take the shortest path.
   Doing that, we can kill all possibilities to go back.
   If there is no way back, we try the start from the second candidate in the cell
   
   That process can miss some possibilities, what is accepted.

*/
void  TZCF::Aic_Ycycle(USHORT t1,USHORT t2,BFTAG & loop,USHORT cand)
{	// forget if not same digit or t2 is ~t1
ZPTLN candt1=zpln.zp[t1>>1],candt2=zpln.zp[t2>>1];
if(cand==(t2>>1)) return;
if(candt1.ch-candt2.ch) return;
USHORT ct1= (h.dp.t[t1] & loop).Count(),ct2=(h.dp.t[t2] & loop).Count();

if(ct1<1 || ct2<1) return; // should never be

if(ct1<2){ Aic_YcycleD(t1,t2^1,loop,cand); return;}  
if(ct2<2){ Aic_YcycleD(t2,t1^1,loop,cand); return;}  
USHORT wt1=t1,wt2=t2; if(ct2<ct1) {wt1=t2;wt2=t1;}
Aic_YcycleD(wt1,wt2^1,loop,cand);
}

/* now the all possibilities with start t1 and crossing t2
   we must start with a weak link so we exclude as start the strong link
*/
void  TZCF::Aic_YcycleD(USHORT t1,USHORT t2,BFTAG & loop,USHORT cand)// up to 4 starts
{USHORT tt[20],itt,lg=200;   PATH resf,resw;
 USHORT tagc=cand<<1,tagcn=tagc^1;
 (h.dp.t[t1] & loop).String(tt,itt); // starts in table
 for(int i=0;i<itt;i++) 
  {if(h.dp.t[tt[i]].On(t1)) continue; // this is a strong link, skip it
   if(tt[i]==tagcn) continue; // don't start with the elimination
   int lw=Aic_Ycycle_start(t1,tt[i],t2,loop,resw);
   if(lw)
    {//if(resw.On(tagcn)) continue; // cand must not be in the loop
     //if(resw.On(tagc)) continue; // to be validated
     if(lw<lg){lg=lw;resf=resw;}
    }
  }
 // lg final count, resf final result finish the task
 if(lg>100) {Aic_YcycleD2(t1,t2,loop,cand); return; }// try the second way
 int rat=tchain.GetRatingBase(65,lg,cand);
 if(!rat) return;
     // chain is accepted load it and in test mode, find more comments
 if(Op.ot){EE.Enl("Y cycle out of the region");
           resf.PrintPathTags();   }
	     //  ExplainPath(resf,t1,t1,lg+2,t2);  }
 tchain.LoadChain(rat,"Y cycle",cand); 

}


/* first process failed
   try starttin from the second candidate in the cell containing t1
*/

void  TZCF::Aic_YcycleD2(USHORT t1x,USHORT t2x,BFTAG & loop,USHORT cand)// up to 4 starts
{if(0) {EE.E("Aic_Ycycle d2"); 
         EE.E(" t1x=");zpln.ImageTag(t1x);
         EE.E(" t2x"); zpln.ImageTag(t2x); 
         EE.E(" cand="); zpln.Image(cand);EE.Enl();
		 h.dp.t[t1x^1].Image("liens",0);EE.Enl();
        }
USHORT t2=t2x^1,t1=0; // new target is  "on"
 USHORT tt[20],itt,lg=200;   PATH resf,resw;
 h.dp.t[t1x^1].String(tt,itt); // starts in table
 if(itt<1) return;// should never be
 t1=tt[0];
 if(0) {EE.E("Aic_Ycycle d2 go"); 
         EE.E(" t1=");zpln.ImageTag(t1);EE.Enl();
        }
 USHORT tagc=cand<<1,tagcn=tagc^1;
 (h.dp.t[t1] & loop).String(tt,itt); // starts in table
 for(int i=0;i<itt;i++) 
  {if(h.dp.t[tt[i]].On(t1)) continue; // this is a strong link, skip it
   if(tt[i]==tagcn) continue; // don't start with the elimination
   int lw=Aic_Ycycle_start(t1,tt[i],t2,loop,resw);
   if(lw)
    {//if(resw.On(tagcn)) continue; // cand must not be in the loop
     //if(resw.On(tagc)) continue; // to be validated
     if(lw<lg){lg=lw;resf=resw;}
    }
  }
 // lg final count, resf final result finish the task
 if(lg>100) return; // should never happen
 int rat=tchain.GetRatingBase(65,lg,cand);
 if(!rat) return;
     // chain is accepted load it and in test mode, find more comments
 if(Op.ot){EE.Enl("Y cycle out of the region");
           resf.PrintPathTags();   }
	     //  ExplainPath(resf,t1,t1,lg+2,t2);  }
 tchain.LoadChain(rat,"Y cycle",cand); 

}





/* process one start  t1->t1a looking for t1
   and get back the count
   go first to t2
   then track back the path to clean the "done" filtering the process
   and continue to t1. send back the count and the BFTAG
*/
int TZCF::Aic_Ycycle_start(USHORT t1,USHORT t1a,USHORT t2,BFTAG & loop,PATH & path)
{ if(0) {EE.E("Aic_Ycycle_start"); 
         EE.E(" start=");zpln.ImageTag(t1);
         EE.E(" thru="); zpln.ImageTag(t2); 
         EE.E(" first="); zpln.ImageTag(t1a);EE.Enl();
		// loop.Image("loop",0);
         EE.Enl();
        }
  BFTAG wstart; wstart.Set(t1a);  //we start a search with t1 -> t1a
   int npascycle= wstart.SearchCycleChain(h.dp.t,t1,t2,loop) ;
  if(!npascycle) return 0;
  path.ipth=npascycle+2;  // itt set to last tag in the path
  if(wstart.TrackBack(h.dp.t,t1,t2,path.pth,path.ipth,t1a))return 0; // bug collecting the path 

  if(0) {EE.Enl("Aic_Ycycle_start end of first track back"); 
         path.PrintPathTags();EE.Enl();  }

  // we go back with a reduced loop, restarting from t2

  BFTAG  wstart2; wstart2.Set(t2);  //we start next  search from t2 
  BFTAG  loop2(loop); // we cancel the forward path in the loop
  for(int i=1;i<path.ipth;i++) loop2.Clear(path.pth[i]); // BFTAG equivalent to tt
  
  int npas2= wstart2.SearchCycleChain(h.dp.t,t2,t1,loop2) ;// and now we continue to the end

  if(0) {EE.E("Aic_Ycycle_start after  second npas2="); EE.Enl(npas2);
        loop2.Image("loop2",0); EE.Enl();}

  // if npas2=0 it can be due to the fact that the first path lock the way back
  // we then try a start from the other candidate in the cell containing t1


  if(!npas2) return 0;
  PATH path2; path2.ipth=npas2+1;  // itt set to last tag in the path
  if(wstart2.TrackBack(h.dp.t,t2,t1,path2.pth,path2.ipth,t1))return 0; // bug collecting the path 

  if(0) {EE.Enl("Aic_Ycycle_start end of second track back"); 
         path2.PrintPathTags();EE.Enl();  }

  // expand path1 with path2
  for(int i=1;i<path2.ipth;i++) path.Add(path2.pth[i]);
  return (path.ipth-1);  
}

/* we have found something in a forward step
  in test mode, we have to publish an equivalent path
  */
void TZCF::ExplainPath(BFTAG & forward,int start,int end,int npas,USHORT relay)
{if(npas>40) {EE.E("path too long in Explainpath npas=");EE.Enl(npas);
               return;}
	          
 USHORT tt[50],itt=npas; 
   // EE.E("explain start=");zpln.ImageTag(start);EE.E(" end=");zpln.ImageTag(end);
   // EE.E(" npas=");EE.Enl(npas);
// forward.Image("forward",start);
 forward.TrackBack(h.dp.t,start,end,tt,itt,relay);
// jdk.PointK(); // put a milestone and print the chain
 zpln.PrintImply(tt,itt);
}
/* done when higher rating  already found
   just eliminate candidates (true) without looking for loops
   replacing X Y and XY search
   */
int TZCF::Fast_Aic_Chain()  
{int ir=0;
 jdk.TaggingInit();
 zpln.CellLinks();    zpln.RegionLinks(1); 	         
 h.d.ExpandAll(h.dp); // 	
 for(int i=2;i<col;i+=2)  if(h.d.Is(i,i^1) )
 { zpln.Clear(i>>1); 	// just clear it if not test mode
  ir++;
  if(op0 && Op.ot)
    {EE.E("clear "); zpln.ImageTag(i); EE.Enl();}
     BFTAG wch=h.dp.t[i]; int npasch=wch.SearchChain(h.dp.t,i,i^1);
     ExplainPath(wch,i,i^1,npasch+2,i^1); 
  }
 return ir;}

 