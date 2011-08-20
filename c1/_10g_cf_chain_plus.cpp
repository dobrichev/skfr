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

/* looking for elimination in dynamic mode
   in SE, such eliminations are always worked out of 2 chains
   analyzed form the left to the right.

   That process works in the same way, so elimination is one of the followings

   a-> b  and a -> ~b  (the classical  form a->~a is included here)
   x-> ~a and ~x -> ~a
   
   region or cell multi chain  a -> {~x ~y ~z);

   to be revisited to optimize the length control
   provisionnal status, each search is force to parsemini=100 in direct path
   */
void TZCF::ChainPlus(BFCAND & dones)   
{BFTAG *t=h.d.t,*tp=h.dp.t; 
for(int i= 2;i<col;i+=2) 
  { BFTAG  zi=(t[i].Inverse()).TrueState(),zw1=t[i] & zi,
		   zw2=(t[i] & t[i^1]).FalseState();
    if(zw1.IsNotEmpty()) // this is a a-> b  and a -> ~b
	  {if(op1 && Op.ot){//long tw=GetTimeMillis();
                //  int dt=tw-tdebut;
				 // EE.E("time =");EE.Enl(dt);
		          EE.E("found active a->x and a->~x");
				  zw1.Image(" elims",i);
		          }
		CANDGO cg; cg.GoCand(i); 
	  }
	// if we start form a bi value, SE does not say
	// ~x but goes immediatly to the bi-value  saving one step.
	if(zw2.IsNotEmpty()) // this is x-> ~a and ~x -> ~a
	  {	if(op1 && Op.ot){//long tw=GetTimeMillis();
                 // int dt=tw-tdebut;
		        //  EE.E("time =");EE.Enl(dt);
                  EE.E("found active x->~a and ~x->~a");
	              zw2.Image("elims",i);
	              }
	    CANDGO cg1; cg1.GoOne(i,zw2); 
		CANDGO cg2; cg2.GoOne(i^1,zw2);
		BFTAG bf1=zw2 & cg1.cumsteps[cg1.npas],
			  bf2=zw2 & cg2.cumsteps[cg2.npas],
		      bft=bf1&bf2;
       if(bft.IsEmpty()) continue;
	   // find the smallest overall length
	   int totlength=300,i2f=0;
	   for(int i2=3;i2<col;i2+=2) if(bft.On(i2))
	   {EE.E("essaichain plus killing "); zpln.Image(i2>>1);   EE.Enl();	
		 int l1=cg1.GoBack(i2,0),l2=cg2.GoBack(i2,0);
	    if((!l1) || (!l2)) continue;// should never be
		// if chain 2 starts by a strong link, reduce the count by one
		if((zcf.dpbase.t[i^1]& cg2.cumsteps[1]).IsNotEmpty()) l2--;
		int ll=l1+l2; if(ll<totlength) {totlength=ll;i2f=i2;}
	   }
	   if(i2f) 
	   {int ratch=tchain.GetRating(totlength,i2f>>1);
        if(ratch) // chain is accepted load it (more comments in test mode)
         {if(Op.ot) // this can be complex and will be developped later
            { EE.E("chain plus killing "); zpln.Image(i2f>>1);   EE.Enl();	     
             EE.E("chain 1 ");   cg1.GoBack(i2f,1) ;
		     EE.E("chain 2 ");   cg2.GoBack(i2f,1);
		     }
	     tchain.LoadChain(ratch,"chain plus",i2f>>1);
	    }
	   }// end if i2f

	 }// end if zw2

  } // end for i

if(tchain.base<85) return;

// now check multichains in a similar way but not for nishio
 for(int ie=1;ie<zcx.izc;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   if(chx.type-CH_base) break; // only base sets can be used
   int n=chx.ncd; USHORT *tcd=chx.tcd ; 
   BFTAG tbt,bfset; tbt.SetAll_1();
   for(int i=0;i<n;i++) bfset.Set((tcd[i]<<1)^1);
   for(int  i=0;i<n;i++)  tbt &=  (t[tcd[i]<<1]-bfset); 
   if(tbt.IsNotEmpty()) // candidate(s) to clear found
	  {if(Op.ot&& op0){tbt.Image(" eliminations  multi chain dynamic mode  ",0);
	                   chx.Image();EE.Enl();} 
	   for(int  j=3;j<col;j+=2)if(tbt.On(j)) // all tags killed
	      {int tot_length=0; 
	       if(Op.ot && op0){EE.E(" Set killing "); zpln.Image(j>>1); EE.Enl(); }
		   for(int i2=0;i2<n;i2++)  
			   { BFTAG wbf;  	wbf.Set(j);
				 CANDGO cg1; cg1.GoOne(tcd[i2]<<1,wbf); 
		         if(cg1.cumsteps[cg1.npas].Off(j))      {tot_length=0;break;}// process stopped
                 tot_length+=cg1.GoBack(j,0);
	     	   }
		   if(!tot_length) continue;
           int ratch=tchain.GetRating(tot_length,j>>1);
           if(ratch) // chain is accepted load it (more comments in test mode)
               {// in test mode  give the details for the chains
				// in that case, do it again and print
				if(Op.ot)for(int i2=0;i2<n;i2++) 
				  {  BFTAG wbf;       wbf.Set(j);
		             CANDGO cg1; cg1.GoOne(tcd[i2]<<1,wbf); 
					 cg1.GoBack(j,1);
		          }
               tchain.LoadChain(ratch,"chain",j>>1);	
               }
 	      } // end  for j
       } // end if
	
   }// end for ie


}
 
