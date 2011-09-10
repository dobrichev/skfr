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
/* comments on the process
level 2 base 9.5   Forcing chain authorized
level 3 base 10.0  lev 2 + multiple chains authorized

*/

//==============================================
// 95 Quick is a partial processing for nested + forcing chains
//    and nested + multi chains
// can be used to reduce the number of puzzles when looking for hardest puzzles

void JDK::InitNested()  // common part before starting nested processing
{// lock the start situation with fully expanded  hdp from step 90
	if(Op.ot) {EE.Enl("Init  nested levels  ");}
  zcf.ResetOne(); // restore the index in zcf 
  zcf.ExpandAll();// not derived weak links
 zcf.LockNestedOne(); zcx.LockNestedOne();zcxb.LockNestedOne();
 //zcf.h_one.d.Image();
}

int JDK::Rating_baseNest(USHORT base,int quick)
{	
if(Op.ot){EE.E("start  nested levels base =");EE.Enl(base );
           if(quick) EE.Enl("Quick mode is On");
		   else EE.Enl("Quick mode is Off");}


if(base>95) return 0; // provisoire en attente du code 

tchain.SetMaxLength(base);
zcf.h_nest=zcf.h_one; // create the start for that nested level

 for(int i=2;i<col;i++)  { CANDGO candgo; candgo.GoNestedTag(i,base);  }   
 
//if(Op.ot && jdk.couprem==5)  zcf.h_nest.d.Image();//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
// we have now fully expanded tags in zcf.h_nest.d
// we look for potential eliminations

// cases 1 and 2,{ a=>x a=> ~x } { x => ~a and ~x=> ~a}
// case 2 includes the so called cell or region eliminations for bi values
EE.Enl("start eliminations investigations");
 BFTAG elims1,elims2,elims3,elimst2[300],tchte[500]; 
 USHORT t2[300],it2=0;
 for(int i=2;i<col;i+=2)
 { BFTAG tw=zcf.h_nest.d.t[i],tw1=tw&tw.Inverse(),
         tw2=tw & zcf.h_nest.d.t[i^1];
   tw2=tw2.FalseState();
   tw1=tw1.TrueState();
   if(tw2.IsNotEmpty())  // case 2
   {elimst2[it2]=tw2;t2[it2++]=i; elims2 |= tw2;}
   if(tw1.IsNotEmpty())  // case 1
   {elims1.Set(i);}
}

// case 3, is there a set killing "a"
USHORT tch[500],itch=0;
for(int ie=1;ie<zcx.izc;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   int   n=0,nni=chx.ncd; 
   if (chx.type-CH_base) continue;
   BFTAG bfw; bfw.SetAll_1();
   for(int i=0;i<nni;i++)  bfw &= zcf.h_nest.d.t[chx.tcd[i]<<1] ;
   if(Op.ot && 0)//jdk.couprem ==5)
   {chx.Image();
    bfw.Image("communs",0);
   }
	  bfw=bfw.FalseState();	 
	  if(bfw.IsNotEmpty()) 
	    {tchte[itch]=bfw;
		 tch[itch++]=ie;
	     elims3|=bfw;
       }	   	  
   }// end case 3

BFTAG elimt=elims1.Inverse() | elims2 | elims3;
if(Op.ot) elimt.Image("elim potential",0);
if(elimt.IsEmpty()) return 0;
// if we are in quick mode, set all elims with rating base+ .3/.5

if(quick)
  {elimt.Image("quick elim potential",0);
   int j=3;
   for(int i=3;i<col;i+=2) // first in tchain mode
   if(elimt.On(i))  { j=i;
                       tchain.LoadChain(base+3,"quick nested ",i>>1);
                       elimt.Clear(i);break;}
   for(int i=j+2;i<col;i+=2) // others in direct mode
     if(elimt.On(i))  { zpln.Clear(i>>1);} 
   return Rating_end(200);
  }
// not quick mode, go step by step to find the lowest rating
if(op1 && Op.ot)
  {EE.E("action it2 ="); EE.E(it2);
   EE.E(" itch ="); EE.E(itch);
   EE.Enl(); 
  }

if(elims1.IsNotEmpty())  // some case 1 to apply
 for(int i=2;i<col;i+=2) if(elims1.On(i))
        {CANDGO candgo; candgo.GoNestedCase1(i>>1,base);}

if(it2)  // some case 2 to apply
 for(int i=0;i<it2;i++)
    {BFTAG * ptg=&elimst2[i];
     USHORT ttt[]={t2[i],t2[i]^1};
	 for(int j=3;j<col;j++) if(ptg->On(j))
	 {Rating_Nested(base,ttt,2,j);}
    }

if(itch)  // some case 3 through sets to apply
for(int i=0;i<itch;i++)
    {if(op1 && Op.ot)
     {EE.E("action itch pour i ="); EE.E(i);
      EE.E(" set="); EE.E(tch[i]);
      EE.Enl(); 
      }
	 BFTAG * ptg=&tchte[i];
	 ZCHOIX chx=zcx.zc[tch[i]];
     int   n=0,nni=chx.ncd; 
     USHORT ttt[20];
     for(int j=0;j<nni;j++)  ttt[j]=chx.tcd[j]<<1 ;
	 for(int j=3;j<col;j++) if(ptg->On(j))
	 {Rating_Nested(base,ttt,nni,j);}
    }


 return Rating_end(200);} 

/* find the target in dynamic mode for a set of tags
*/
void JDK::Rating_Nested(USHORT base,USHORT * ttags,USHORT ntags,USHORT target)
{// filter if target if part of the set
 USHORT ctarg=target>>1;
 for(int i=0;i<ntags;i++) if(ttags[i]>>1==ctarg) return;	
	if(op1 && Op.ot)
  {EE.E("entry rating nested ntags ="); EE.E(ntags);
   EE.E(" target "); zpln.ImageTag(target);
   EE.E( "  through" );zpln.ImageTag(ttags[0]); EE.Esp();zpln.ImageTag(ttags[1]);
   EE.Enl(); 
  }
	
USHORT length=0;
 for (int i=0;i<ntags;i++)
 {CANDGO candgo;
  USHORT lx=candgo.GoNestedCase2_3(base,ttags[i], target);
  if(!lx) return; //should never be
  length+=lx;
 }

 int ratch=tchain.GetRating(length,target>>1);
 if(ratch) // chain is accepted load it (more comments in test mode)
    {if(Op.ot) // this can be complex and will be developped later
        { EE.E("chain plus killing "); zpln.ImageTag(target);   EE.Enl();	     
		 }
     tchain.LoadChain(ratch,"chain plus through set",target>>1);
     }// end if



}


void CANDGO::GoNestedTag(USHORT tag,USHORT base)
{ BFTAG tt=zcf.h_nest.d.t[tag] ; 
  if(tt.IsEmpty()) return;
  zcf.StartNestedOne(); zcx.StartNestedOne();zcxb.StartNestedOne();
  opp=0;//if(tag>107 && tag<112) opp=1; else opp=0;
  to=zcf.h_one.dp.t; //forward and back tracking table 
  tcandgo.Init(); // intital storage for strong links and nested eliminations
  tstore.Init(); // start with an empty table of chains stored
 
      //now go forward  till nothing or a contradiction

 for(int i=0;i<640;i++) tsets[i]=0;
 npas=0;
 steps[0]=tt; tt.String(tx[0],itx[0]);
 if(itx[0]>=200)itx[0]=200; // safety measure
  allsteps=cumsteps[0]=steps[0];  
 aig=1; int maxpas=30;  // should be enough in that mode

       //--------------------- loop  forward
 while(aig && npas++<=maxpas)  
 { if(opp)
      {EE.E("tag "); EE.E(tag); zpln.ImageTag(tag);
       EE.E(" npas= "); EE.Enl(npas); 
     }	 
 aig=0; 
 cum = &cumsteps[npas-1]; step=&steps[npas]; step->SetAll_0();
 ta=tx[npas-1];tb=tx[npas];    ita=itx[npas-1];itb=0;

 if(opp)      {(*cum).Image("cum debut",0); }
  GoNestedWhileShort(tag,base);                    // while cycle
  cumsteps[npas]=(*cum) | (*step);   // or allsteps updated in while routine
  if(opp)      {(*step).Image("step",0);        }
  itx[npas]=itb;
 }// end while
 zcf.h_nest.d.t[tag]=cumsteps[npas-1];// note:the last one is empty
}

/* while cycle  for GoNested first run, just find potential
   first apply new actions from direct implications and sets
   then check if new strong links appeared
   if new strong links, search for new indirect forcing chain
   if level, check anyway new multi chain eliminations 

*/
void CANDGO::GoNestedWhileShort(USHORT tag,USHORT base)
{USHORT aignl=1;
 BFTAG * tdpn = dpn.t,  // new set of direct links
	   * hdp=zcf.h.dp.t; // basic set of direct links including dynamic effects

                        // look first for direct 
  
   for(int it=0;it<ita;it++)
     {BFTAG x=to[ta[it]]- allsteps;	  // still free and in the overall path
       if(x.IsNotEmpty()) 
        {(*step)  |= x; // flag it in the BFTAG and load in new
	      if(opp)	x.Image("applied std" ,ta[it]);	   
	     allsteps |= x; // and in the total 
		 hdp[tag] |= x;
         USHORT ty[30],ity=0; x.String(ty,ity);
	     for(int i=0;i<ity;i++) tb[itb++]=ty[i];
		 aig=1;}    
     }

      // check now sets

for(int ie=1;ie<zcx.izc;ie++)
  {ZCHOIX chx=zcx.zc[ie];
   int   n=0,nni=chx.ncd,aig2=0,toff[10]; 
   switch (chx.type)
     {case CH_base:  // must be n-1 false or n false (then all is ok)
            // check if active 0 to 2 cand unknown
       {BFTAG bfw;
		for(int i=0;i<nni;i++) // max one free 
			 {USHORT cd=chx.tcd[i],j=cd<<1; // candidate in tag form
	          if(cum->On(j)) {n=3; break; }// set assigned
	    	  if(cum->Off(j^1))    {toff[n++]=j;if(n>2) break; } 
			  else bfw.Set(j^ 1);
		    }
      
	   USHORT cd1=toff[0],cd2=toff[1]; 
	   if(n==2) {if( hdp[cd1].Off(cd2^1) || hdp[cd2].Off(cd1^1) ||
	             hdp[cd1^1].Off(cd2) || hdp[cd2^1].Off(cd1))
				 
	             {hdp[cd1].Set(cd2^1); hdp[cd2].Set(cd1^1);
	              hdp[cd1^1].Set(cd2); hdp[cd2^1].Set(cd1);
	              aignl=0; 
				  tcandgo.AddStrong(cd1>>1,cd2>>1,bfw,nni-2);
	              }
                break;}
	   if (n>1) break;	// if ok second round for action	
	   for(int i=0;i<n;i++)  
			 {	USHORT j=toff[i]; // candidate in tag form
				 if( ((!n) && allsteps.Off(j))  ||
					(n && cum->Off(j^1)) && allsteps.Off(j))
				{(*step).Set(j);   
				 allsteps.Set(j);  hdp[tag].Set(j);// and in the total 
                 tb[itb++]=j;    tsets[j]=ie;
                 aig=1;  aig2=1;       
				}
		     }
	   	if(opp && aig2)
	      {EE.E("set derive actif ");chx.Image();EE.Enl();}
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
		      if(allsteps.On(j) ) continue;
			  (*step).Set(j);  
			  allsteps.Set(j); hdp[tag].Set(j);// and in the total 
               tb[itb++]=j;    tsets[j]=ie;
                 aig=1;  aig2=1;       
				}
		   if( opp && aig2)
	           {EE.E("set actif event");chx.Image();
		        ev.Image(); }

		   break;	
	 } // end switch
   }// end proc


  if((*step).IsNotEmpty()) return;
  // we look for indirect hints
  // zcf.h_one.dp.Image();dpn.Image();
  Gen_dpnShort(tag);
  BFTAG elims; 
  NestedForcingShort(elims); 
  
  BFTAG x=elims.Inverse() - hdp[tag];
  if(x.IsNotEmpty()) // force false so use elims.inverse()
  {(*step)  |= x; // flag it in the BFTAG and load in new
   if(opp)	x.Image("forcing chain elims" ,0);	
   allsteps |= x; // and in the total 
   hdp[tag] |= x;
   USHORT ty[200],ity=0; x.String(ty,ity);
   if(ity>200) ity=200;
   for(int i=0;i<ity;i++) tb[itb++]=ty[i];
   aig=1;}    

  if(base<100) return;
	  
}

void CANDGO::Gen_dpnShort(USHORT tag)
{          // create the reduced set of tags check tagelim empty 
 dpn.Init(); 
 BFTAG tt=zcf.h.d.t[tag] ,      * tdp=zcf.h.dp.t;
 USHORT tagc=tag; if(tagc&1) tagc^=1;
 for (int j=2;j<col;j++) 
	{if(j==tagc) continue; // don't process the start point
	 if(tt.On(j)) continue; // that tag is defined
	 if(tt.On(j^1)) continue; // that tag is defined as well (to check)
      dpn.t[j]=tdp[j]-tt; // reduce the primary weak links
	 }
if(op0){EE.Enl("image dpn de dpnshort");
        dpn.Image();}
 }


void CANDGO::NestedForcingShort(BFTAG & elims)
{TDB dn; dn.ExpandAll(dpn); // 	
 for(int i=2;i<col;i+=2)
  if(allsteps.Off(i^1) && dn.Is(i,i^1)   )  // a forcing chain found, find the length
   elims.Set(i); 
 
}
