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

/* Dynamic search in nested mode for a candidate
   do both contradiction, 
   direct  a -> x and a -> ~x so 'a' is false
   indirect x -> ~a   and ~x -> ~a  'a' is false
     (also x -> ~a and y -> ~a if xy is a bivalue
   and "set contradiction" 2 a for each start "true" of the set
   tagnot is
     0 if no indirect search has to be done
	 pointer to tag expanded to do a direct check

   zcf.h is refreshed for each new entry from zcf.h_one
   zcf.h.dp follows the dynamic forward process 
     each new link is loaded in the dp table
*/

int CANDGO::GoNested(USHORT cand,BFTAG * tagnot,USHORT base)
{ //opp=(Op.ot && jdk.couprem<5);
 USHORT tag=cand<<1,aigchk=1; 
 BFTAG tt=zcf.h.d.t[tag] ; 
 to=zcf.h_one.dp.t; //forward and back tracking table
   // first a quick check that some new strong links will come in the process
 for(int ie=1;ie<zcx.izc_one;ie++)// check all sets
  {ZCHOIX chx=zcx.zc[ie];
   USHORT nni=chx.ncd,n=0; 
   if (chx.type-CH_base) continue;
    // check all sets for still valid candidates
   for(int i=0;i<nni;i++)
      {USHORT ti=chx.tcd[i]<<1; // candidate in tag form
       if(tt.On(ti)) {n=1; break; }// set assigned
	   if(tt.Off(ti^1)) n++;
      }
   if(n==2) {aigchk=0; break;} // stop at first found
	}   
  if(aigchk) return 0; //no potential bivalue

  if(opp)
    {EE.E("go nested for cand "); zpln.Image(cand);EE.Enl(); }
 
  tcandgo.Init(); // intital storage for strong links and nested eliminations
  tstore.Init(); // start with an empty table of chains stored
 
      //now go forward  till nothing or a contradiction

 to=zcf.dpbase.t; // we use the table without derivations
 for(int i=0;i<640;i++) tsets[i]=0;
 npas=0;
 steps[0].SetAll_0(); steps[0].Set(tag); 
 allsteps=cumsteps[0]=steps[0];  
 tx[0][0]=tag; itx[0]=1; // initial is tag to go forward

 ret_code=0;   aig=1;
 int maxpas=50;  // will be reduced to "one more" if something found

       //--------------------- loop  forward
 while(aig && npas++<=maxpas)  
 {	 aig=0; 
 cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
 ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;

  GoNestedWhile(tag,base);                    // while cycle
  if(opp)
        {EE.E("fin step=");EE.E(npas);
                   step->Image("step ",0); }
  cumsteps[npas]=cumsteps[npas-1] | (*step);
  itx[npas]=itb;


  // check for a contradiction in that lot (stop at first)
  for(int i=0;i<itb;i++) 
  {USHORT tgx=tb[i];
   if(allsteps.On(tgx) && allsteps.On(tgx^1))
   {if(opp)
      {EE.E("found active a -> x and a -> ~x");zpln.ImageTag(tgx);
            EE.E(" step="); EE.Enl(npas);   
            }
	   // we compute back the length and show the sequence 
    if(maxpas>npas) maxpas=npas; // limit the process to 1 more step
    int l1=GoBackNested(tgx,0),l2=GoBackNested(tgx^1,0);
	if((!l1) || (!l2)) continue; // should not be
	int ratch=tchain.GetRating(l1+l2,tag>>1);
    if(ratch) // chain is accepted load it (more comments in test mode)
      {if(Op.ot) // this can be complex and will be developped later
        { EE.E("chain plus killing "); zpln.Image(tag>>1);   EE.Enl();	     
          EE.E("chain 1 ");   GoBackNested(tgx,1) ;
		  EE.E("chain 2 ");  GoBackNested(tgx^1,1);
		 }
	   tchain.LoadChain(ratch,"chain plus",tag>>1);
	  }// end case 1
	if(tgx&1 && tagnot) // case 2 only if false state
     if(allsteps.On(tgx) && tagnot->On(tgx)) // < -> ~a  and ~x -> ~a
	  {if(opp)
	       {EE.E("found active x -> ~a and x -> ~a  for x ");zpln.Image(cand);
	          EE.E(" and ~a  "); zpln.ImageTag(tgx);   EE.E(" step="); EE.Enl(npas);   
              }	 
	   // we compute back the length and show the sequence 
      if(maxpas>npas) maxpas=npas; // limit the process to 1 more step
      int l1=GoBackNested(tgx,0),l2=2;
	  BFTAG w=zcf.h.dp.t[tag^1] ; // primary for ~x
	  if(w.Off(tgx)) //should look for second chain, will seee later
	  {
	                 // a finir, pour seconde chaine
	  }
	  if((!l1) || (!l2)) continue; // should not be
	  int ratch=tchain.GetRating(l1+l2,tag>>1);
      if(ratch) // chain is accepted load it (more comments in test mode)
        {if(Op.ot) // this can be complex and will be developped later
          { EE.E("chain plus killing "); zpln.Image(tag>>1);   EE.Enl();	     
            EE.E("chain 1 ");   GoBackNested(tgx,1) ;
		//    EE.E("chain 2 ");  GoBackNested(tgx^1,1);
		   }
	   tchain.LoadChain(ratch,"chain plus",tag>>1);
	  }

	  }// end case 2
   }// end for i

   // now look for a possible set elimination
/*  for(int ie=1;ie<zcx.izc;ie++)
     {ZCHOIX chx=zcx.zc[ie];
      int   nni=chx.ncd,aig2=0; 
      BFTAG zt;zt.SetAll_1();
      if(chx.type-CH_base) continue;
       // must be  n false 
       for(int i=0;i<nni;i++) // max one free 
		{USHORT cd=chx.tcd[i],j=cd<<1; // candidate in tag form
	     if(cum->On(j))  {aig2=1; break; }// set assigned
	     if(cum->Off(j^1)) zt &= dn.t[j];
		}

*/
  }
  if(op0 && !aig){EE.E("fin aig=0 pour step=");EE.Enl(npas);}
 }// end while
return ret_code;}

/*  en cours
void CANDGO::NestedMulti(BFTAG & elims)
{TDB dn; dn.ExpandAll(dpn); // 
 
for(int ie=1;ie<zcx.izc;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   int   nni=chx.ncd,aig2=0; 
   BFTAG zt;zt.SetAll_1();
   if(chx.type-CH_base) continue;
    // must be  n false 
   for(int i=0;i<nni;i++) // max one free 
		{USHORT cd=chx.tcd[i],j=cd<<1; // candidate in tag form
	     if(cum->On(j))  {aig2=1; break; }// set assigned
	     if(cum->Off(j^1)) zt &= dn.t[j];
		}
	if (aig2 || zt.IsEmpty()) continue;	// if ok second round for action	
   for(int i=3;i<col;i+=2) if(zt.On(i) && allsteps.Off(i) )
	 {	elims.Set(i^1); 
        BFTAG bfs;
        USHORT istored=0,istoref=0,tot_count=0;
		 for(int i2=0;i2<nni;i2++)  
		{USHORT cd=chx.tcd[i2],j=cd<<1; // candidate in tag form
		 if(cum->On(j^1)) {bfs.Set(j^1);continue;}// already false
         
		 BFTAG wch=dpn.t[j]; 
		 int npasch=wch.SearchChain(dpn.t,j,i);	
         if((!npasch )|| (npasch>40) )continue; // should never happen  
	     USHORT tt[50],itt=npasch+2; 
         if(wch.TrackBack(dpn.t,i,i^1,tt,itt,i^1)) // intercept error for debugging
	           {EE.Enl("new nested multi chain bactrack error");		 continue  ;}
	     // tt now contains the imply =>  sequence i => ....=> i^1
        if(opp)  // print it it test mode
	        {EE.Enl("new nested multi chain");	    zpln.PrintImply(tt,itt); }
		   // must add the source for the new strong links
  	    for (int j=1;j<itt-1;j++) // all strong links 
	      { if((!(tt[j]&1)) || (tt[j+1]&1)) continue;
		    CANDGOSTRONG * w=tcandgo.Get(tt[j],tt[j+1]);
	        if(w) bfs |= w->source;// it is a new strong link		
	        }
		 istoref=tstore.AddChain(tt,itt);
		 if(!istored) istored=istoref;
		 tot_count+= npasch+1;
	    }// end i
       if(istored) //should always be
	    {USHORT ii=tstore.AddMul(istored,istoref);
	     tsets[i^1]=-tcandgo.itt; tcandgo.tt[tcandgo.itt++].Add(ii,bfs,tot_count); 
	    }
       } // end tag not anymore valid
	 }// end ie
 
  
}
*/

