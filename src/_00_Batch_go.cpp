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

// loop to process the batch and puzzles verifications

// local functions
void PrintTime(long ts,long te,int cons=0);    // Print elapsed time


//! Evaluation loop of puzzle and print the result for each puzzle
void Batch_Go()
{	long tstart=GetTimeMillis();// for the overall elapsed time
	char ze[82];
	foutput.Setzpuz(ze);		// prepare to output the puzzle string
	while(finput.GetPuzzle(ze)) 
	{
		long tpuz_start=GetTimeMillis(); // for puzzle elapsed time
		int er, ep, ed, aig;

		int ir=ratePuzzleC(ze, & er, & ep, & ed, & aig); 
		if(Op.os)    // split option
        {
			if(ir<4) se_refus<< ze 
		//		<< " " << Op.ermax << " " << Op.epmax<< " " << Op.edmax
				<< endl; 
			else  foutput << ze 
		//		<< " " << Op.ermax << " " << Op.epmax<< " " << Op.edmax 
				<<endl; 
		}
		else
		{  
			if(aig)er=0;
			bool refus = (er<10) || (er>120);
			if(refus)se_refus.PrintErEpEd(er,ep,ed);
			else  foutput.PrintErEpEd(er,ep,ed);
			if(Op.ptime && !refus ) 
			{
				long tpuz_end=GetTimeMillis();
				PrintTime(tpuz_start,tpuz_end);
			}
			if(refus) se_refus << endl;
			else  foutput<<endl;
		}
	}
	// print global elapsed time
	long tend=GetTimeMillis();
	PrintTime(tstart,tend,1); // foutput<<endl;
}




// builing an appropriate message depending on the elapsed time te-ts
void PrintTime(long ts,long te,int cons)
{UINT dt=te-ts,dtmil=dt%1000,dts=dt/1000,dth=dts/3600;   dth=dth%1000;
if(cons)
   { cout << endl<<"total elapsed time "; 
     UINT dtm=dts/60; dts=dts%60 ,   dth=dtm/60, dtm=dtm%60;
     if(dth) cout <<dth<<"h "; 
	 if(dth || dtm) cout <<dtm<<"m "; 
     cout	<<dts <<"s ";
	 if(dtmil<10) cout << "00"; else  if(dtmil<100) cout << '0';
	 cout <<dtmil<<"ms ";   return;
    }
  foutput	 <<";"<<dts <<".";
  if(dtmil<10) foutput << "00"; else  if(dtmil<100) foutput << '0';
  foutput <<dtmil<<"s ";  
}


