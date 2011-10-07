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

//#include "stdafx.h"
#include "global.h"
#include "bitfields.h"
#include "flog.h"

//includes above  to be replaced later by (mladen)
//#include "ratingengine.h"

#include "opsudo.h"



OPSUDO::OPSUDO()  // constructor, overall initial values for command line
    {   delta=0;		
		o1=ot=os=oq=oexclude=filters.f=0;
		edcycles=1;
		mined=minep=miner=0;maxed=maxep=maxer=200;
		ptime=0;
		ocopybugs=1;
    }  
void OPSUDO::Init() // start of a resolution
    {   cycle=assigned=c_ret=0;
	    ermax=epmax=edmax=0;
    }
int OPSUDO::is_ed_ep()   // at the start of a new cycle
	{
		if(cycle<2) return 0;
		c_ret = Is_ed_ep_go();
		return c_ret;
	}
void OPSUDO::SetEr()   // something found at the last difficulty level  
	{
		if((cycle==1)&& difficulty>edmax) edmax=difficulty;
		if(((!assigned)|| (!epmax))&& difficulty>epmax) epmax=difficulty;
		if(difficulty>ermax) ermax=difficulty;      
	}


// filter at the start of a new cycle

int OPSUDO::Is_ed_ep_go()  // is the ed or  condition fullfilled
{switch(o1)
  {case 0: return 0;       // nothing to do
 
     // if -d command, other filters ignored  
   case 1: if((ermax- edmax)>delta)  
		      {ermax=0;epmax=0;return 1;}  // not diamond
		   return 0;// continue if still ed


     // if -p command, similar results
   case 2: if(!assigned) return 0;   // -p command
	       if((ermax- epmax)>delta)
		        {ermax=0;return 1;} // not pearl
	       return 0;// continue if still ep
  }	
	
     // now, we have no -d no -p command but at least one filter is on	
     // give priority to the -n() command

if(filters.On(3) )// -n() command
	if(ermax >= miner) {ermax=0;return 3;} // finished
      else if(cycle>edcycles) return 4;

	  //then all max conditions
if(edmax>=maxed || epmax>=maxep || ermax>=maxer)
	     {ermax=0;return 3;} // finished

     // and finally min ED and min EP
if(edmax<=mined)  {ermax=0;return 3;} // finished
if(assigned && epmax<=minep)  {ermax=0;return 3;} // finished
if(!os)return 0; // finish with split ok

     // that sequence should work for any combinaison of filters.
if((filters.f&7) ==1) return 4; // ed ok for split   	
if(assigned && ((filters.f&6) ==2))  return 4; // ep ok for split   	

return 0;}

// the next routine is called at each step of the process
// the searched rating is stored in "difficulty"
// a check of filters is made and if any special filter is active,
// the process is cancelled
// return code 0 nothing special
//             1 process cancelled
//             2 ignore (er<x.y) these routines

void OPSUDO::Step(SolvingTechnique dif) {   // analyse what to do at that level
	ir = 0;
	difficulty = dif;
	if(o1 < 2)
		return; //nothing to do for -d command
	// if -p command, stop if we pass maxep
	if(o1 == 2) {
		if(assigned && difficulty > maxep) {
			ermax = 0;
			ir = 1;
			return;
		} 
		else
			return;
	}
	// now other special filters 
	// -n() active if we pass the limit
	if(filters.On(3)) {    // -n() command
		if(difficulty >= miner) {
			ermax = 0;
			ir = 3;
			return;
		}  // finished
	}
	if(difficulty >= maxer) {
		ermax = 0;
		ir = 2;
		return;
	} // filter on maxer cancelling high rating

	/*
	if(difficulty>maxed || difficulty>maxep ){maxer=0; ir=1;return;}
	*/
}
