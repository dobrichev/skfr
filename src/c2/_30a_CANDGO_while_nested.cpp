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
/* while cycle  for GoNested 
   first apply new actions from direct implications and sets
   then check if new strong links appeared
   if new strong links, search for new indirect forcing chain
   if level, check anyway new multi chain eliminations 

*/
void CANDGO::GoNestedWhile(USHORT tag,USHORT base)
{USHORT aignl=1;
 BFTAG * tdpn = dpn.t,  // new set of direct links
	   * hdp=zcf.h.dp.t; // basic set of direct links including dynamic effects

                        // look first for direct 
  
   for(int it=0;it<ita;it++)
     {BFTAG x=to[ta[it]]- allsteps;	  // still free and in the overall path
      if(x.IsNotEmpty()) 
        {if(opp)  x.Image("applied std" ,ta[it]);
	  	(*step)  |= x; // flag it in the BFTAG and load in new
	     allsteps |= x; // and in the total 
		 hdp[tag] |= x;
         USHORT ty[30],ity=0; x.String(ty,ity);
	     for(int i=0;i<ity;i++) tb[itb++]=ty[i];
		 aig=1;}    
     }

      // check now sets

for(int ie=1;ie<zcx.izc;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   int   n=0,nni=chx.ncd,aig2=0,toff[10]; 
   switch (chx.type)
     {case CH_base:  // must be n-1 false or n false (then all is ok)
            // check if active 0 to 2 cand unknown
       {BFTAG bfw;
		for(int i=0;i<nni;i++) // max one free 
			 {USHORT cd=chx.tcd[i],j=cd<<1; // candidate in tag form
	          if(cum->On(j)) {n=3; break; }// set assigned
	    	  if(cum->Off(j^1))    {toff[n++]=j;if(n>2) break; } 
			  else bfw.Set(j^ 1);
		    }
      
	   USHORT cd1=toff[0],cd2=toff[1]; 
	   if(n==2) {if( hdp[cd1].Off(cd2^1) || hdp[cd2].Off(cd1^1) ||
	             hdp[cd1^1].Off(cd2) || hdp[cd2^1].Off(cd1))
				 
	             {hdp[cd1].Set(cd2^1); hdp[cd2].Set(cd1^1);
	              hdp[cd1^1].Set(cd2); hdp[cd2^1].Set(cd1);
	              aignl=0; 
				  tcandgo.AddStrong(cd1>>1,cd2>>1,bfw,nni-2);
	              }
                break;}
	   if (n>1) break;	// if ok second round for action	
	   for(int i=0;i<n;i++)  
			 {	USHORT j=toff[i]; // candidate in tag form
				 if( ((!n) && allsteps.Off(j))  ||
					(n && cum->Off(j^1)) && allsteps.Off(j))
				{(*step).Set(j);   
				 allsteps.Set(j);  hdp[tag].Set(j);// and in the total 
                 tb[itb++]=j;    tsets[j]=ie;
                 aig=1;  aig2=1;       
				}
		     }
	   	if(opp && aig2)
	      {EE.E("set derive actif ");chx.Image();
		   EE.E("  valid ");zpln.ImageTag(tb[itb-1]);EE.Enl();}
	   }
		    break;
	  case CH_set : // in a set all components must be on
           for(int i=0;i<(nni-1);i++)  
			 {if(cum->Off((chx.tcd[i]<<1)^1))    {n++;if(n) break; }   }
		   if(n) break; // must all be there
		   EVENT ev=tevent.t[chx.tcd[nni-1]-event_vi];
           EVENTLOT evl=ev.evl;
 		   for(int i=0;i<evl.itcd;i++)
             {int j=(evl.tcd[i]<<1)^1; // false state on
		      if(allsteps.On(j) ) continue;
			  (*step).Set(j);  
			  allsteps.Set(j); hdp[tag].Set(j);// and in the total 
               tb[itb++]=j;    tsets[j]=ie;
                 aig=1;  aig2=1;       
				}
		   if( opp && aig2)
	           {EE.E("set actif event");chx.Image();
		        ev.Image(); }

		   break;	
	 } // end switch
   }// end proc

  if((*step).IsNotEmpty()) return;

 
  // we look for indirect hints
  Gen_dpn(tag);    // create a fresh reduced set of tags 
 // zcf.h_one.dp.Image();dpn.Image();
  BFTAG elims; 
  NestedForcing(elims); 
  if(opp && Op.ot) elims.Image("netforcing recap",0);
  BFTAG x=elims;// - hdp[tag]; // elims in false state
  if(x.IsNotEmpty()) 
  {(*step)  |= x; // flag it in the BFTAG and load in new
   allsteps |= x; // and in the total 
   hdp[tag] |= x;
   USHORT ty[100],ity=0; x.String(ty,ity);
   for(int i=0;i<ity;i++) tb[itb++]=ty[i];
   aig=1;}    
 if(base<100) return;
 if((*step).IsNotEmpty()) return;

  BFTAG elims2; 
  NestedMulti(elims2); 
  if(opp && Op.ot) elims2.Image("multiforcing recap",0);
  x=elims2;// - hdp[tag];  // elims in false state
  if(x.IsNotEmpty())  
  {(*step)  |= x; // flag it in the BFTAG and load in new
   allsteps |= x; // and in the total 
   hdp[tag] |= x;
   USHORT ty[100],ity=0; x.String(ty,ity);
   for(int i=0;i<ity;i++) tb[itb++]=ty[i];
   aig=1;}    



}



/*  create a reduced tdb without
    the candidate studied
	all candidates forced or cleared if the candidate studied is true
	including all new strong links generated
	and eliminations depending on the level
*/

void CANDGO::Gen_dpn(USHORT tag)
{          // create the reduced set of tags check tagelim empty 
 dpn.Init(); 
// BFTAG tt=zcf.h.d.t[tag] ,      * tdp=zcf.h.dp.t;
 BFTAG tt=allsteps ,      * tdp=zcf.h.dp.t;


 for (int j=2;j<col;j++) 
	{if(j==tag) continue; // don't process the start point
	 if(tt.On(j)) continue; // that tag is defined
	 if(tt.On(j^1)) continue; // that tag is defined as well (to check)
      dpn.t[j]=tdp[j]-tt; // reduce the primary weak links
	 }
  if(op0) {tt.Image("allsteps at gen time",0);
	       tt.Image("zcf.h.d.t[tag]",0);
		   zcf.h.dp.Image();
		   dpn.Image();
          }
 dn.ExpandAll(dpn);
 }


//--------------------------------------------------


int CANDGO::GoBackNested(USHORT tag,int pr)
{if(0){EE.E("goback");zpln.ImageTag(tag);EE.Enl();}
USHORT itret1=0,nestedlength=0;  itret=0;
 BFTAG bf; 
 tret[itret++]=tag;bf.Set(tag);
  while(itret1<itret && itret<150) // solve each entry back
 { USHORT x=tret[itret1],aig=1; // first locate where x has been loaded
   int index=tsets[x];
   if(0 && pr) 
         {EE.E("go back look for "); zpln.ImageTag(x);
              EE.E(" index= ");EE.E( index);
              EE.E(" itret1=");EE.E(itret1);EE.E(" itret=");EE.Enl(itret);}
   for(int i=0;i<=npas;i++) if(steps[i].On(x))
      {aig=0; 
	   if(i)   // not initial assumption
	   if(!index) // this is a direct step
	       {USHORT z=0;
	         for(int i2=0;i2<i;i2++)
			 for(int j=0;j<itx[i2];j++) // look for the earliest  possible parents
	       // take in priority one already there  
	         {USHORT y=tx[i2][j]; 
			  if(to[y].On(x)) 
			       {if(0 && pr) 
			            {to[y].Image("to image ",y);}
					if(!z)z=y;   if(bf.On(y)) {z=y;break;}			     } 
	         }
            if(z && bf.Off(z)) {tret[itret++]=z;bf.Set(z);}
           }
	   else  if(index>0) // it comes from a set, we know which one
	        {ZCHOIX chx=zcx.zc[tsets[x]];
	        if( 0 && pr)
			   {EE.E("set");chx.Image(); EE.Enl();}
			int n=chx.ncd; if(chx.type==CH_set) n--; // for a set, forget the event
		   for(int j=0;j<n;j++) 
		   {USHORT y=chx.tcd[j]<<1; if(y==x) continue;
		    y^=1; if(bf.Off(y)){tret[itret++]=y;bf.Set(y);}
		   }
	      }

	    else  // index <0 this is a nested elimination
	     {	CANDGOFORWARD w=tcandgo.tt[-index];
		  nestedlength += w.count;
		  BFTAG bfn=w.source-bf; // add missing in source
          if(0 && pr)
			  {EE.E("back forcing for "); zpln.ImageTag(x),EE.Enl();
		       w.source.Image("source",0);
			   bfn.Image("solde source",0);}
		  if( 0 && pr) tstore.Print(w.index);
		  for(int j=2;j<col;j++) if(bfn.On(j))
           {tret[itret++]=j;bf.Set(j);}
	     }
        i=100;// force end of process after it has been found
	   }  // end i
  if(aig || itret>150) return 0; // not found, should never be
  itret1++;
  if(0 && Op.ot) {EE.E("go back end step   itret1=");EE.E(itret1);EE.E(" itret=");EE.Enl(itret);}
 }
 if(pr && Op.ot)  // printing in increasing order of generation
    {EE.Enl(" eliminations justification ");
	 for(int i=0;i<=npas;i++) for(int j=0;j<itret;j++) 
		 if(steps[i].On(tret[j])) 
		 {USHORT wt=tret[j]; 
		  zpln.ImageTag(wt); // print the tag annd look for explanation
		  int index=tsets[wt];
	      if(!index) 	     EE.Enl();  // direct no comment
		  else  if(index>0) // it comes from a set, we know which one
	        {ZCHOIX chx=zcx.zc[index];
	         EE.E(" through set ");chx.Image(); EE.Enl(); 		    
	        }

	    else  // index <0 this is a nested elimination
	     {CANDGOFORWARD w=tcandgo.tt[-index];
		  EE.E(" through chain(s) "); EE.Enl();
		   tstore.Print(w.index);
	     }
	      
      EE.Enl();
        }
    EE.E("return itret=");EE.E( itret);EE.E(" nestedplus="); EE.Enl(nestedlength);
    }
return itret + nestedlength;}






/* looking for fresh forcing chain
   we have to find
   the equivalent length
   all data necessary to track back the path, which means
   all candidates that must be eliminated to generate new strong links in use in the chain
 
  to explain the path
    go bakcward
    take all even steps (start=1) which are strong links
    check if this is a new link if yes take the equivalent BFTAG

*/
void CANDGO::NestedForcing(BFTAG & elims)
{for(int i=2;i<col;i+=2)
  if( dn.Is(i,i^1)   )  // a forcing chain found, find the length
   { if(op0)  {zpln.ImageTag(i); EE.Enl("search  nested forcing chain");  } 
	   BFTAG wch=dpn.t[i],bfs; 

     int npasch=wch.SearchChain(dpn.t,i,i^1);
     if(!npasch) continue; // should never happen
	 if(npasch>40) continue; // should never happen either
     if (op0)  {dpn.t[i].Image("dpn",0);
           dn.t[i].Image("dn",0);
		   wch.Image("wch",0);
	       EE.E(" npasch="); EE.Enl(npasch);  } 
	 // must add the source of the new strong links
	 USHORT tt[50],itt=npasch+2; 
     if(wch.TrackBack(dpn.t,i,i^1,tt,itt,i^1)) // intercept error for debugging
	 {
	 continue  ;}
	 // tt now contains the imply =>  sequence i => ....=> i^1
    if(opp)  // print it it test mode
	   {EE.E("forcing chain ");
	    zpln.PrintImply(tt,itt); }
	 elims.Set(i^1); 
	for (int j=1;j<itt-1;j++) // all strong links 
	  { if((!(tt[j]&1)) || (tt[j+1]&1)) continue;
		  CANDGOSTRONG * w=tcandgo.Get(tt[j],tt[j+1]);
	    if(w) bfs |= w->source;// it is a new strong link		
	  }
	USHORT ii=tstore.AddOne(tt,itt);
	tsets[i^1]=-tcandgo.itt; tcandgo.tt[tcandgo.itt++].Add(ii,bfs,npasch+1); 
  }// this is the final length
 
}

/*

Now looking for multi chains eliminations if any
the bfs must contain all candidates killed by the main assumption
and the source of all new strong links used

*/

void CANDGO::NestedMulti(BFTAG & elims)
{ if(op0) allsteps.Image("allsteps",0);
  for(int ie=1;ie<zcx.izc;ie++)
    {ZCHOIX chx=zcx.zc[ie];
   int   nni=chx.ncd,aig2=0; 
   BFTAG zt;
   // we take only the first elimination which is not morph independant.
   // keeping choice open lead to a huge number of possibilities.
   zt.SetAll_1();zt=zt.FalseState()-(allsteps | elims);
   if(chx.type-CH_base) continue;
    // must be  n false 
   for(int i=0;i<nni;i++) // max one free 
		{USHORT cd=chx.tcd[i],j=cd<<1; // candidate in tag form
	     if(cum->On(j))  {aig2=1; break; }// set assigned
	     if(cum->Off(j^1)) zt &= dn.t[j];
		}
	if (aig2 || zt.IsEmpty()) continue;	// if ok second round for action	
   for(int i=3;i<col;i+=2) if(zt.On(i) )
	 {	elims.Set(i); 
        BFTAG bfs;
        USHORT istored=0,istoref=0,tot_count=0;
		 for(int i2=0;i2<nni;i2++)  
		{USHORT cd=chx.tcd[i2],j=cd<<1; // candidate in tag form
		 if(cum->On(j^1)) {bfs.Set(j^1);continue;}// already false

		 // here can be direct and this is not done in search chain
		 // dummy cycle if direct to have common process
         
		 BFTAG wch=dpn.t[j]; 
		 USHORT tt[50],itt=2; tt[0]=j;tt[1]=i;
		 if(wch.Off(i))
		     {int npasch=wch.SearchChain(dpn.t,j,i);	
             if((!npasch )|| (npasch>40) )continue; // should never happen  
		     itt=npasch+2; 
             if(wch.TrackBack(dpn.t,j,i,tt,itt,i)) // intercept error for debugging
	           {EE.E ("new nested multi chain bactrack error");
		        dpn.t[j].Image("dpn",0);
                dn.t[j].Image("dn",0);
		        wch.Image("wch",0);
	            EE.E(" npasch="); EE.Enl(npasch);   EE.Enl();		 
		        continue  ;}
		     }
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
		 tot_count+= itt;
	    }// end i
       if(istored) //should always be
	    {USHORT ii=tstore.AddMul(istored,istoref);
	     tsets[i]=-tcandgo.itt; tcandgo.tt[tcandgo.itt++].Add(ii,bfs,tot_count); 
        if(opp)  // print it it test mode
	        {EE.E("new eliminated");   zpln.ImageTag(i); 
		     EE.E("  ichain="); EE.E(tstore.ise); 
		     EE.E("  stored as "); EE.Enl(tsets[i]); 
             }
	   }
       } // end tag not anymore valid
	 }// end ie
 
  
}
