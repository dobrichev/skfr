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
/* 
Dynamic search in nested mode for a candidate
   do both contradiction, 
   direct  a -> x and a -> ~x so 'a' is false
   indirect x -> ~a   and ~x -> ~a  'a' is false
     (also x -> ~a and y -> ~a if xy is a bivalue
   and "set contradiction" 2 a for each start "true" of the set
   tagnot is
     0 if no indirect search has to be done
	 pointer to tag expanded to do a direct check

   zcf.h is refreshed for each new entry from zcf.h_one
   zcf.h.dp follows the dynamic forward process 
     each new link is loaded in the dp table
*/
int CANDGO::GoNestedCase1(USHORT cand,USHORT base)
{ opp=0;//(Op.ot && jdk.couprem>14);
 USHORT tag=cand<<1; 
 zcf.StartNestedOne(); zcx.StartNestedOne();zcxb.StartNestedOne();
 BFTAG tt=zcf.h.d.t[tag] ; 
 to=zcf.h_one.dp.t; //forward and back tracking table


  if(opp)
    {EE.E("go nested for cand "); zpln.Image(cand);EE.Enl(); }
 
  tcandgo.Init(); // intital storage for strong links and nested eliminations
  tstore.Init(); // start with an empty table of chains stored
 
      //now go forward  till nothing or a contradiction

 to=zcf.dpbase.t; // we use the table without derivations
 for(int i=0;i<640;i++) tsets[i]=0;
 npas=0;
 steps[0].SetAll_0(); steps[0].Set(tag); 
 allsteps=cumsteps[0]=steps[0];  
 tx[0][0]=tag; itx[0]=1; // initial is tag to go forward

 ret_code=0;   aig=1;
 int maxpas=pasmax;  // will be reduced to "one more" if something found

       //--------------------- loop  forward
 while(aig && npas++<=maxpas)  
 {	 aig=0; 
 cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
 ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;

  GoNestedWhile(tag,base);                    // while cycle
  if(opp)
        {EE.E("fin step=");EE.E(npas);
                   step->Image("step ",0); 
        (*step).Image("all",0);
        }
  cumsteps[npas]=cumsteps[npas-1] | (*step);
  itx[npas]=itb;


  // check for a contradiction in that lot (stop at first)
  for(int i=0;i<itb;i++) 
  {USHORT tgx=tb[i];
   if(allsteps.On(tgx) && allsteps.On(tgx^1))
   {if(op0)
      {EE.E("found active a -> x and a -> ~x");zpln.ImageTag(tgx);
            EE.E(" step="); EE.Enl(npas);   
            }
	   // we compute back the length and show the sequence 
    if(maxpas>npas+2)		maxpas=npas+2; // limit the process to 2 more step
	if(maxpas>pasmax) maxpas=pasmax;
    int l1=GoBackNested(tgx,0),l2=GoBackNested(tgx^1,0);
	if((!l1) || (!l2)) continue; // should not be
	int ratch=tchain.GetRating(l1+l2,tag>>1);
    if(ratch) // chain is accepted load it (more comments in test mode)
      {if(Op.ot) // this can be complex and will be developped later
        { EE.E("chain plus killing "); zpln.Image(tag>>1);   EE.Enl();	     
          EE.E("chain 1 ");   GoBackNested(tgx,1) ;
		  EE.E("chain 2 ");  GoBackNested(tgx^1,1);
		 }
	   tchain.LoadChain(ratch,"chain plus",tag>>1);
	  }// end case 1

   }// end for i
  }
  if(op0 && !aig){EE.E("fin aig=0 pour step=");EE.Enl(npas);}
 }// end while
return ret_code;}




/* 
Dynamic search in nested mode for a candidate
   cleared through a set or the 2 sattes of a candidate
 
 ttags:  the sets or the 2 states in tag form
 ntags   the number of tags to process
 target  the tag to eliminate (candidate in false state)
 base     the base depending on the level 

 expand each tag of ttags till target is found
*/
int CANDGO::GoNestedCase2_3(USHORT base,USHORT tag,USHORT target)
{zcf.StartNestedOne(); zcx.StartNestedOne();zcxb.StartNestedOne();
 BFTAG tt=zcf.h.d.t[tag] ; 
 to=zcf.h_one.dp.t; //forward and back tracking table
  if(opp)
    {EE.E("go nested for tag "); zpln.ImageTag(tag);EE.Enl(); }
 
  tcandgo.Init(); // intital storage for strong links and nested eliminations
  tstore.Init(); // start with an empty table of chains stored
 
      //now go forward  till nothing or a contradiction

 to=zcf.dpbase.t; // we use the table without derivations
 for(int i=0;i<640;i++) tsets[i]=0;
 npas=0;
 steps[0].SetAll_0(); steps[0].Set(tag); 
 allsteps=cumsteps[0]=steps[0];  
 tx[0][0]=tag; itx[0]=1; // initial is tag to go forward

 aig=1;
 int maxpas=pasmax;  

       //--------------------- loop  forward
 while(aig && npas++<=maxpas)  
 {	 aig=0; 
 cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
 ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;

  GoNestedWhile(tag,base);                    // while cycle
  if(opp)
        {EE.E("fin step=");EE.E(npas);
                   step->Image("step ",0); }
  cumsteps[npas]=cumsteps[npas-1] | (*step);
  itx[npas]=itb;


  // check for a contradiction in that lot (stop at first)
 if(allsteps.On(target) )
  {	   // we compute back the length and show the sequence 
   int l1=GoBackNested(target,1);
   return l1;}
   
  if(op0 && !aig){EE.E("fin aig=0 pour step=");EE.Enl(npas);}
 }// end while
return 0;}

