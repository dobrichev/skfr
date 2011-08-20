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
#pragma once  
/* the following classes have been designed specifically for SE clone.
   the goal is to find and store chains type eliminations 
   all the necessary stuff for printing is stored
   this is also the place where we keep the filter to limit the search of paths*/

 /* 6.2: Aligned Pair Exclusion
    6.5 - 7.5: X-Cycles, Y-Cycles
    6.6 - 7.6: Forcing X-Chains Y_Chains
    7.0 - 8.0: Forcing Chains, XY-Cycles
    7.5: Aligned Triplet Exclusion
    7.5 - 8.5: Nishio
    8.0 - 9.0: Multiple chains
    8.5 - 9.5: Dynamic chains
    9.0 - 10.0: Dynamic chains (+)
	     requiring another storing organization
    9.5 - ?? : Nested Forcing Chains
    
 */
/*a specific entry is created for each candidate eliminated
  No need to store a detailed explanation of an elimination
     in test mode, this must be done immediately
  storing is limited to 
         the candidate eliminated   
	     the milestone to find back the explanation in test mode
  only chains (up to 30) with the same rating are stored

   */

class CHAIN   //defining a chain and elimination in one buffer
{public: 
USHORT cand,         //candidate to clean
       urem;        // milestone of the explanation

inline void Load(USHORT cande){urem=jdk.couprem; cand=cande;}
int Clean(); // clear candidates
};   



class TCHAIN // storing up to 20 chains having the same "lowest"rating
{public:      
 USHORT rating,             // current rating 
	    maxlength;          // filter for the search of a path (tags)
 CHAIN chainw,chains[30]; // enough to store several small loops 
 USHORT ichain,           // index to chains 
	    base;

 void Init() {rating=200;ichain=0;maxlength=10;}

 // the following functions relates to performance control in the search of eliminations
 // should be revised in the new process
 // the first function set the default value for the base

 void SetMaxLength( int bw)  ;
 inline int GetYmax() {return maxlength;}
 USHORT ParsingMini(USHORT basee);
 int GetYlower(int lg) ;
 // added for the new design
 int GetLimit(int lg,int modexy) ;

 // entry in tchain is either with an explicit base of with the default value

 int GetRatingBase(USHORT bb,USHORT lengthe,USHORT cand);
 int GetRating(USHORT lengthe, USHORT cand)
       {return GetRatingBase(base,lengthe,cand);}

 void LoadChain(USHORT rat,char * lib,USHORT cand);
  
 inline int IsOK(USHORT x){ int ir=((rating <= x ) || (rating <= Op.ermax));
                           if(ir && Op.ot) 
						           {EE.E(" sortie isok chaine x="); EE.E(x); 
                                   EE.E(" rating="); EE.E(rating);  
								   EE.E(" ermax=="); EE.Enl(Op.ermax); }
						   return ir;}
     //{return ((rating <= x ) || (rating <= Op.ermax));}

int Clean()  // clear all pending candidates
      {int ir=0; for(int i=0;i<ichain;i++) ir+=chains[i].Clean();
        return ir;}

 void Status()
     {EE.E("tchain ichain=");EE.E(ichain);EE.E(" rating=");EE.Enl(rating);
     }

 
 }tchain ;

