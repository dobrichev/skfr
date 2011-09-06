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
level 4 base 10.5  lev 3 + dynamic forcing chains authorized
*/

//==============================================
// 95 is the first level for nesting  follows an empty step 90

// first expand each candidate depending on the level
// then look for one of the following eliminatons
//
// a) a => x and a=> ~x  a not valid  (a true state)
// b) x=> ~a and ~x => ~a   a not valid 
// c) set {x;y;z} (cell or region set) x=> ~a  y=>~a z=>~a a not valid
//
// if a non valid condition is found, a second run is done keeping data for length computation


// consider each  candidate as start (must have ~ active if valid) 
// search for new bi values. If none, skip it
// look for new false depending on the nested level
// and check if now not eliminated

int JDK::Rating_baseNest(USHORT base)
{tchain.SetMaxLength(base);
// BFTAG * tdp=zcf.h.dp.t; // must include the new strong links
 if(Op.ot) {EE.E("start  nested levels base =");EE.Enl(base );}
 zcf.h_nest=zcf.h_one; // create the start for that nested level
 for(int i=1;i<zpln.ip;i++) 
  {// nothing to do if part of the solution and no direct start for "false"
   USHORT tag=i<<1;
   BFTAG tt=zcf.h_one.d.t[tag],* ttc=& zcf.h_one.d.t[tag^1];
   if(ttc->IsEmpty()) ttc=0;
   if(zpln.candtrue.On(i) && (!ttc)) continue;
   zcf.StartNestedOne(); zcx.StartNestedOne();zcxb.StartNestedOne();
   CANDGO candgo; candgo.GoNested(i,ttc,base);

 } // end i
  return Rating_end(200);} 
