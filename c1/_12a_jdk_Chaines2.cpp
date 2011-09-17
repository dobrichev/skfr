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

	
	void JDK::TaggingInit()
{zgs.ReInit();   // 81 bits patterns in use 
 zcf.Init();     // elementary weak links
 zcx.Init();     // sets (choices) in use
// tevent.Init();   // event
 BFTAG::SetMax();
}


// chain call 1=biv 2= cell_bivalue 4=nishio 8dynamic 16=multi_chain
//==============================================
// this includes the search for x or y cycle
int JDK::Rating_base_65()
{Chaining(1,0,65);// x cycle  
 if( Rating_end(65)) return 1;
 Chaining(2,0,65);// y cycle  
return Rating_end(70);} 


//=============================================
// this includes the search for xy cycle or chain
int JDK::Rating_base_70()
{Chaining(3,0,70);// xy  forcing chain or loop
return Rating_end(75);} 

//==============================================
// this does not includes the search for aligned triplet 
// only th search for nishio
int JDK::Rating_base_75()
{Chaining(1+4,0,75); 
 return Rating_end(200);} 

//==============================================
// 80 is multi chains
	// we enter now a new group of eliminations. all the work is done using
	// all bi values, basic weak links , basic sets
int JDK::Rating_base_80()
{if(Op.ot) EE.Enl("start rating base 8.0 multi chains");
 TaggingInit();zpln.CellLinks(); zpln.RegionLinks(1);
 zpln.GenCellsSets();      zpln.GenRegionSets();  tchain.SetMaxLength(80);
 zcf.ExpandAll();
 if(zcx.Interdit_Base80() ) return 1;; 
 return Rating_end(200);} 


/* killed for the new one
//==============================================
// 85 is DynamicForcingChain
	// at least the derived weak links from direct weak links
	// we do that once for ever till the end
int JDK::Rating_base_85()
{ if(Op.ot) EE.Enl("start rating base 8.5 dynamic forcing chain");
  if(op0&& Op.ot){   long tw=GetTimeMillis();
             int dt=tw-tdebut;
		     EE.E("time =");EE.Enl(dt);}
 tchain.SetMaxLength(85);
 BFCAND bf0; // init to no candidate found 
 zcf.CloseOne(); // store the index and basic weak links
 zcx.DeriveDirect(); 
 zcf.ChainPlus(bf0);	
 if(tchain.IsOK(87) ) return Rating_end(87); 
 zcf.DeriveCycle(3,3,0,4); // one cycle;
 zcf.ChainPlus(bf0);	
 if(tchain.IsOK(87) ) return Rating_end(87); // the shortest should come here
 // zcf.ListeForIdPhase(wl_set,zcf.iphase);
// tout ce qui suit est provisoire et à préciser

  if(!zcf.DeriveCycle(3,7,0,10)) return Rating_end(100);
  if(tchain.IsOK(90) ) return Rating_end(90); // the shortest should come here
  zcf.ChainPlus(bf0);	
  //if(tchain.ichain) return Rating_end(100);

   int iw=1;
   while(iw++<8)
	   {if(!zcf.DeriveCycle(3,9,0)) return Rating_end(100);
	    zcf.ChainPlus(bf0);
        if(tchain.IsOK(93) ) return Rating_end(93); // the shortest should come here
 	   }

 return Rating_end(200);}
 */ 

/* 85 new process
   expand completely the tags, but try to do it to catch the shortest
   first steps are not standard
   finish with standard expansion
   then look for elimintaions
   */
//==============================================
// 85 is DynamicForcingChain


int JDK::Rating_base_85()
{ if(Op.ot) EE.Enl("start rating base 8.5 dynamic forcing chain");
 tchain.SetMaxLength(85);
 BFCAND bf0; // init to no candidate found 
 zcf.CloseOne(); // store the index and basic weak links
 zcx.DeriveDirect();  // start with only equivalence to pointing claiming
 zcf.DeriveCycle(3,3,0,4); // one cycle short sets
 zcf.DeriveCycle(3,7,0,10); // one more limited to set of 7 candidates
 while(zcf.DeriveCycle(3,9,0)){} // and full expansion
 zcf.ChainPlus(bf0);
 return Rating_end(200);}

/* old process cancelled
//==============================================
// 90 is DynamicForcingChainPlus
// all "events"  claiming, pointing, pair, hidden pair, XWing
// follows an empty step 85
// consider each false candidate as start
// search for new bi values. If none, skip it
// look for new false thru basic sets
int JDK::Rating_base_90()
{if(Op.ot) EE.Enl("start rating base 9.0 dynamic forcing chains plus");
 tchain.SetMaxLength(90);
  zcf.ResetOne(); // restore the index in zcf  
  tevent.LoadAll();
  zcf.CloseOne(); // store the index and basic weak links

  zcf.h.d.ExpandShort(zcf.h.dp,2);
  BFCAND bf0; // init to no candidate found 
  zcf.DeriveCycle(3,4,7,2); // one cycle;
  zcf.ChainPlus(bf0);	
  if(tchain.IsOK(95) ) return Rating_end(95); // the shortest should come here
  zcf.DeriveCycle(3,7,7,10); // one cycle;  
  zcf.ChainPlus(bf0);	
  if(tchain.IsOK(97) ) return Rating_end(97);
  int iw=0;
  while(iw++<20  && zcf.DeriveCycle(3,9,7)  )
	   {zcf.ChainPlus(bf0);
		if(tchain.IsOK(97+iw) )return Rating_end(97+iw);
	   }
 return Rating_end(200);} 
*/

/* as in dynamic forcing chains,
   first a full controlled expansion
   then the search for the shortest
   */
//==============================================
// 90 is DynamicForcingChainPlus
// all "events"  claiming, pointing, pair, hidden pair, XWing
// follows an empty step 85
// consider each false candidate as start
// search for new bi values. If none, skip it
// look for new false thru basic sets
int JDK::Rating_base_90()
{if(Op.ot) EE.Enl("start rating base 9.0 dynamic forcing chains plus");
 tchain.SetMaxLength(90);
  zcf.ResetOne(); // restore the index in zcf  
  tevent.LoadAll();
  zcf.CloseOne(); // store the index and basic weak links

  zcf.h.d.ExpandShort(zcf.h.dp,2);
  BFCAND bf0; // init to no candidate found 
  zcf.DeriveCycle(3,4,7,2); // one cycle;
  zcf.DeriveCycle(3,7,7,10); // one cycle;  
  while(zcf.DeriveCycle(3,9,7)  ){}
  zcf.ChainPlus(bf0);
  return Rating_end(200);} 
