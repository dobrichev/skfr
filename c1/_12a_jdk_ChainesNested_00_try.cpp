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
/* proces final step in quick classification for nested chains
   
   some nested eliminations have been found. 
   Is it enough to eliminate the main candidate

   That process is derived from what is done in CANDGO first phase
*/

int NestedTry(TDB & dx,USHORT tag, BFTAG & tagt,BFTAG & elims)
{for(int i=2;i<col;i++) if(elims.On(i)) tagt.Set(i^1); // new eliminations
 tagt. Expand(dx.t,tag);
 if(tagt.On(tag^1)) return 1;
 // if nothing comes, expand using sets
 USHORT aig=1,npas=0;
 USHORT tx[50][100],itx[50];
 BFTAG steps[50],cumsteps[50], * to =dx.t;   
 BFTAG  *cum ,* step; 
 USHORT *ta,*tb,ita,itb;
 cumsteps[0]=steps[0]=tagt; // inital is the current situation
 itx[0]=0; // initial nothing from directs weak links
 while(aig && npas++<30)  // safety limit, could be no limit
 {	 aig=0; 
 cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
 ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;
    // ----- first direct effect from new (nothing step one)
 for(int it=0;it<ita;it++)
     {BFTAG x=to[ta[it]]- tagt;	  // still free and in the overall path
      if(x.IsNotEmpty()) 
        {(*step)  |= x; // flag it in the BFTAG and load in new
	     tagt |= x; // and in the total 
         USHORT ty[30],ity=0; x.String(ty,ity);
	     for(int i=0;i<ity;i++) tb[itb++]=ty[i];
		 aig=1;}    
     }
    // 
 for(int ie=1;ie<zcx.izc_one;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   int   n=0,nni=chx.ncd,aig2=0; 
   switch (chx.type)
     {case CH_base:  // must be n-1 false or n false (then all is ok)
       for(int i=0;i<nni;i++) // max one free 
			 {if(cum->Off((chx.tcd[i]<<1)^1))    {n++;if(n>1) break; }   }
	   if (n>1) break;	// if ok second round for action	
	   for(int i=0;i<nni;i++)  
			 {	int j=(chx.tcd[i]<<1); 
		        if( ((!n) && tagt.Off(j))  ||
					(n && cum->Off(j^1)) && tagt.Off(j))
				{(*step).Set(j);   tagt.Set(j); // and in the total 
                 tb[itb++]=j;  
                 aig=1;         
				}
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
		      if(tagt.On(j) ) continue;
			  (*step).Set(j);   tagt.Set(j); // and in the total 
               tb[itb++]=j;   
                 aig=1;         
				}
		   break;	
	 } // end switch
   }// end for ie

  cumsteps[npas]=cumsteps[npas-1] | (*step);
  itx[npas]=itb;
  
 } // end while

return 0;}


