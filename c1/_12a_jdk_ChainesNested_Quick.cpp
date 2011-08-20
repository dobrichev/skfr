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
/* comments on the process
level 2 base 9.5   Forcing chain authorized
level 3 base 10.0  lev 2 + multiple chains authorized

*/

//==============================================
// 95 Quick is a partial processing for nested + forcing chains
//    and nested + multi chains
// can be used to reduce the number of puzzles when looking for hardest puzzles


int JDK::Rating_base_95_Quick()
{BFTAG * tdp=zcf.h.dp.t; // must include the new strong links
 zcf.h.dp=zcf.dpbase; // but not all derived weak links
 // so we store dpbase and d fully extended
 if(Op.ot) EE.Enl("start rating quick nested levels");

// zcf.h.dp.t[8].Image("dep",8);zcf.h.dp.t[9].Image("dep",9);

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
