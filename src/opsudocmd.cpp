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
#include <iostream>
#include "skfrtype.h"
#include "opsudocmd.h"
using namespace std;

OPSUDOCMD::OPSUDOCMD()  // constructor, overall initial values for command line
    {   delta=0;		
		o1=ot=os=oq=oexclude=filters.f=0;
		edcycles=1;
		mined=minep=miner=0;maxed=maxep=maxer=200;
		ptime=0;
		logFileName[0]=0;
    }
int  OPSUDOCMD::pctl()  
	// contrôle print on console for test purpose
	// evaluate filters bitfield if min or max required
	// also check command line compatibility (between min and max)
	{
		// split option is exclusively for special "filters"
		if(o1<3) os=0; 	
		cout << "summary of parsing"<<endl;
		if(o1==1) cout << "mode ed delta="<<delta<<endl;
		if(o1==2) cout << "mode ep delta="<<delta<<endl;
		if(o1==3) cout << "special filters on"<<endl;
		if(ot) cout << "test option"<<endl;
		if(maxed<200){cout << "ed<"<<maxed<<endl;filters.f|=1;}
		if(mined){cout << "ed>"<<mined<<endl;filters.f|=1;}
		if(maxep<200){cout << "ep<"<<maxep<<endl;filters.f|=2;}
		if(minep){cout << "ep>"<<minep<<endl;filters.f|=2;} 
		if(maxer<200){cout << "er<"<<maxer<<endl;filters.f|=4;}
		if(miner)
		{
			cout << "differred ed cycl="<<edcycles<<" rating"<<miner<<endl;
			filters.f|=8; 
		}
		if(ptime) cout << "print time"<<endl;
		if(!(filters.f&7)) os=0; // no split on er condition, meaningless
		if(os) cout << "split mode active filters="<<filters.f<<endl;
		if(oexclude) cout << "process limitation code="<<oexclude<<endl;
		if(maxed<=mined || maxep<=minep || maxer<=miner)
		{
			cout << "cancelled due to contradiction in filters"<<endl;
			return 1;
		}
		return 0;
	}