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

/* main routine to process chains
   entry at a specific basic value for min rating
   process is cancelled as soon as a solution is found with a shorter rating
   all chain with the minimum rating  are stored
*/
// chain call 1=biv 2= cell_bivalue 4=nishio 8 dynamic 16=multi_chain

void JDK::Chaining(int opt ,int level,int base)
{TaggingInit(); tchain.SetMaxLength(base);
// long tta,ttc;// provisoire, pour test de temps
 int ir=0,iw;   
 if(Op.ot){ EE.E("entree chaining opt=");EE.E( opt);
          EE.E(" base=");EE.E( base); EE.E(" level=");EE.Enl( level);}

 if((opt&3)==2)  // "Y" mode slightly different
     { zpln.CellStrongLinks(); zpln.RegionLinks(0);}
 else
     {if(opt&2)zpln.CellLinks(); 
      zpln.RegionLinks(opt&1);}

 if(opt & 4 ) // dynamic mode 
   { if(opt&2)zpln.GenCellsSets(); 
     zpln.GenRegionSets();  
   }

 BFCAND bf0; // init to no candidate found 

  switch (base)
 {case 65: case 70:    // same process for X;Y;XY loops and chains
          zcf.Aic_Cycle(opt&3);     // several tags
 		  break;


 case 75: //  once derived weak links from directs
	      zcf.CloseOne(); // save basic weak links
	      zcx.DeriveDirect();   
	      zcf.ExpandShort(5); 
		  zcf.ChainPlus(bf0);	
		  if(tchain.IsOK(78) ) return; // the shortest should come here

		  zcf.DeriveCycle(3,6,0,10); // one cycle;
		  if(tchain.IsOK(78) ) return;  // in case multi chain found
 // zcf.ListeForIdPhase(wl_set,zcf.iphase);
		  zcf.ChainPlus(bf0);	
		  if(tchain.ichain) break; // final?? , stop if found

// tout ce qui suit est provisoire en cours de calage
// pour trouver un équilibre efficacité performance

  		  TDB::InitParsing();
  		  EE.E("contrôle parsing mini ");EE.E(tchain.ParsingMini(75));
  		  EE.E (" rating =");EE.Enl(tchain.rating);
 
		  if(!zcf.DeriveCycle(3,7,0,10)) break; // one cycle;  
		  if(tchain.ichain) break;
		   TDB::InitParsing();
	      zcf.ChainPlus(bf0);	
		  if(tchain.ichain) break;


		  iw=1;
		  while(iw++<3)
		   {if(!zcf.DeriveCycle(3,9,0)) break;
		    TDB::InitParsing();
	        zcf.ChainPlus(bf0);
			if(tchain.ichain) break;// will see later if ok
		   }

		 
		  break;
 }
}
	
//========= end of a rating step, check if done or still to be searched

   /* next is the next rating to be searched.
      the search is over if a <= rating has been found.
      likely <= next+1 due to the minimum length adjustment 
      except for aligned triplet */


int JDK::Rating_end(int next)
{if(!tchain.IsOK(next) )return 0;
Op.Step((SolvingTechnique)tchain.rating); if(Op.ir)return 0; 
return tchain.Clean();}
