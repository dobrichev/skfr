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

/* when a potential elimination is found
   we must create the set of new weak links
   should be 
      our xtp less
      original dpbase less our "tt" =  d derived of the tag "on" 
    
*/
void QuickFinal(USHORT i,BFTAG & elims,BFTAG & tagelim,BFTAG tt,USHORT base)
{if(elims.IsEmpty() && tagelim.IsEmpty()) return;
 if(op1 && Op.ot )
	 {tt.Image("try nested for ",i<<1);   
      if(tagelim.IsNotEmpty()) tagelim.Image("éliminables directs",0);
      elims.Image("trouve elims chain",0);
	  }
 int irtry=NestedTry(zcf.h_one.dp,i<<1,tt,elims|tagelim);
			   //tt.Image("new nested for ",i<<1);
 if(zpln.candtrue.Off(i))
	{if(irtry)  tchain.LoadChain(base,"quick nested",i);
	 else
	  {BFTAG ttw1=tt & tt.Inverse();
	   if(ttw1.IsNotEmpty())   tchain.LoadChain(base,"quick nested",i);
	  }
	 }  
 else // with a true cand, look for indirect 2
	 {USHORT tagn=(i<<1)^1;
	  BFTAG ttn(zcf.h_one.dp.t[tagn]);
      ttn.Expand(zcf.h_one.dp.t,tagn);
		 tt &= ttn;      //    zcf.h_one.dp.t[(i<<1)^1];
	  if(tt.IsNotEmpty())
		{ if(op0 && Op.ot )
	      {tt.Image("try nested  bis for ",i<<1); 
           zcf.h_one.dp.t[(i<<1)^1].Image("associated tag",0);
            tt.Image(" elimination indirecte à finaliser  pour ",i<<1);
	      }
	      for(int j=3;j<col;j+=2) // only false processed
		  if(tt.On(j))   tchain.LoadChain(105,"quick nested indirect",j>>1);
		}
	 }
} 


int JDK::Rating_base_95_Quick()
{BFTAG * tdp=zcf.h.dp.t; // must include the new strong links
 zcf.h.dp=zcf.dpbase; // but not all derived weak links
 // so we store dpbase and d fully extended
 if(Op.ot) EE.Enl("start rating quick nested levels");
 // lock the start situation
 zcf.LockNestedOne(); zcx.LockNestedOne();zcxb.LockNestedOne();
 for(int i=1;i<zpln.ip;i++) 
  {// nothing to do if part of the solution and no direct start for "false"
   if(zpln.candtrue.On(i) && zcf.h_one.dp.t[(i<<1)^1].IsEmpty()) continue;
   BFTAG tt=zcf.h_one.d.t[i<<1];
   zcf.StartNestedOne(); zcx.StartNestedOne();zcxb.StartNestedOne();
   if(zcx.CheckGoNested1(tt,i))
      {// to see what is going on, create a "reduced table of implications"
       TDB dpn; dpn.Init(); BFTAG tti=tt.Inverse(),tagelim; tagelim.SetAll_0();
	   for (int j=2;j<col;j++) 
	      {if((j>>1)==(i>>1)) continue; // don't process the start point
	       if(tti.On(j)) continue; // that tag is defined
	       if(tti.On(j^1)) continue; // that tag is defined as well (to check)

		   if((tdp[j]&tti).IsNotEmpty() ) {tagelim.Set(j); continue;}
		    dpn.t[j]=tdp[j]-tt; // reduce the primary weak links
	      }
	   BFTAG elims;
       if(zcf.Nested_QuickForcing(dpn,elims))
           QuickFinal(i,elims,tagelim, tt,105);
	   if(tchain.rating<110) continue;

	   BFTAG elims2;
       if(zcf.Nested_QuickMulti(dpn,elims2))
	     {// EE.E("quick multi reponse positive");
           QuickFinal(i,elims|elims2,tagelim, tt,110);
	     }

       } // end  check go nested
   else EE.Enl();
  } // end i
  return Rating_end(200);} 

/* quick search of potential eliminations thru forcing chains in nested mode
   xtp is the direct weak and strong links 
   including nested generated weak links

   SE has a dynamic search, to simulate partially that,
   that process loops with fresh new strong links 
*/

int TZCF::Nested_QuickForcing(TDB & xtp,BFTAG & elims)  
{int ir=0; elims.SetAll_0();
 TDB xt; xt.ExpandAll(xtp); // 	
 for(int i=2;i<col;i+=2)
  if(xt.Is(i,i^1)  ){elims.Set(i); ir=1; }
 
return ir;}

/* quick search of potential eliminations thru multi chains in nested mode
   basically, we work on a PM equivalence to the puzzle 
   after all effects of a candidate set to "true" 
   The process is then identical to the standard multi chain.
   For us, we must use a cleaned set of direct weak and strong links
     without effect of direct events
   Same definition as in Quick Forcing
   So, as in multi, no need to use parents (left to right)
   we work on the sets generated in zcx.CheckGoNested1
*/

int TZCF::Nested_QuickMulti(TDB & xtp,BFTAG & elims)  
{int ir=0; elims.SetAll_0();
 TDB xt; xt.ExpandAll(xtp);  	
 for(int ie=zcx.izc_one;ie<zcx.izc;ie++)
 {ZCHOIX chx=zcx.zc[ie];
   if (chx.type-CH_base) continue;// should never be
   USHORT nni=chx.ncd; 
   BFTAG tbt; tbt.SetAll_1();
   for(int  i=0;i<nni;i++)  tbt &= xt.t[chx.tcd[i]<<1];
   if(tbt.IsEmpty()) continue ;// nothing assigned
   elims |= tbt.Inverse(); 	// add eliminations
   ir=1;
 } 
return ir;}

