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
/* called to prepare the nested step for candidate cand
   return code is 0 if no new strong link 
                  1 if at least one strond link found
   create the new sets and the new strong links.
*/

int TZCHOIX::CheckGoNested1(BFTAG bftag,USHORT cand)
{int ir=0;
 for(int ie=1;ie<izc_one;ie++)// check all sets
  {ZCHOIX chx=zc[ie];
   USHORT nni=chx.ncd,n=0,toff[10]; 
   if (zc[ie].type-CH_base) continue;
    // forget all sets where one candidate is now true
	// Gen a strong link if reduced to 2 candidates
	// should never be 0 candidate true
	// gen the reduced set if more than 2
   for(int i=0;i<nni;i++)
      {USHORT ti=chx.tcd[i]<<1; // candidate in tag form
       if(bftag.On(ti)) {n=1; break; }// set assigned
	   if(bftag.Off(ti^1)) toff[n++]=chx.tcd[i];;
      }
     if(n<2) continue;
	 if(n==2) // create a new strong link
	     {zcf.LoadBivalue(toff[0],toff[1]); ir=1; }
	 else // create a new set 
	     {ChargeSet(toff,n,CH_base);}
			// CopySet(ie);} ???(copy the previous one)
	}   
return ir;}

