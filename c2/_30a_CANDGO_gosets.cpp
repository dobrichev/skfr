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

//--------------------------
// subfunction for GoCand or GoOne, add sets valid
void CANDGO::GoSets()
{for(int ie=1;ie<zcx.izc;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   int   n=0,nni=chx.ncd,aig2=0; 
   switch (chx.type)
     {case CH_base:  // must be n-1 false or n false (then all is ok)
       for(int i=0;i<nni;i++) // max one free 
			 {if(cum->Off((chx.tcd[i]<<1)^1))    {n++;if(n>1) break; }   }
	   if (n>1) break;	// if ok second round for action	
	   for(int i=0;i<nni;i++)  
			 {	int j=(chx.tcd[i]<<1); 
		        if( ((!n) && allsteps.Off(j))  ||
					(n && cum->Off(j^1)) && allsteps.Off(j))
				{(*step).Set(j);   allsteps.Set(j); // and in the total 
                 tb[itb++]=j;    tsets[j]=ie;
                 aig=1;  aig2=1;       
				}
		     }
	   	if(op0 && Op.ot && aig2)
	      {EE.E("set derive actif ");chx.Image();EE.Enl();}

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
			  (*step).Set(j);   allsteps.Set(j); // and in the total 
               tb[itb++]=j;    tsets[j]=ie;
                 aig=1;  aig2=1;       
				}
		   if(op0 && Op.ot&& aig2)
	           {EE.E("set actif event");chx.Image();
		        ev.Image(); }

		   break;	
	 } // end switch
   }// end for ie
}