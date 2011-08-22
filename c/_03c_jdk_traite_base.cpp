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

int JDK::Traite()
{//================== assign clues to start
 
 cInit(1); PKInit();
 tdebut=GetTimeMillis();
 long told=tdebut;
 T81->init();  //initial candidats au lieu de Traite_Init(); 
 for(int i=0; i<81;i++)  // first assignment from clues
     {int w=gg.pg[i];if(w>'0' && w<='9') T81->Fixer(w-'1',i,2);}
 cReport(); // and preparing T81 from PM per digit

aigstop=0;  
Op.Init(); 
EE.Enl( );// new line in case test is done
  
 //===========================================================
while (Op.cycle++<150) 
  { if(Op.cycle>148) {Op.Seterr(7); break; } // 
	if   (aigstop){Op.Seterr(5); break; } 
    long tcycle=GetTimeMillis();
//	if(tcycle-told>3000) {Op.Seterr(6); break; }  
	if(op0) {if(tcycle-tdebut>20000) {Op.Seterr(6); break; } 
		      int dt=tcycle-tdebut;
		      EE.E("time =");EE.Enl(dt);}
	told=tcycle;
    int ir_ed=Op.is_ed_ep();
    if(ir_ed>2)return ir_ed;//split filter
	if(ir_ed) return 0; // pure ed ep filter
	if   (!Recale())  {Op.Seterr(6);break;}
    if   (!gg.NBlancs()) break; // finished
	    // processing 1.0 to <6.2
	int ir_a=Traite_a();    
	//cout<<"cycle="<<Op.cycle<<"retour a="<<ir_a<<" Op.ir="<<Op.ir<<endl;
	
	if(!ir_a )return Op.ir; else  if(ir_a<2)continue;

       //  start of processes using the tagging infrastructure.
       // the table of candidates is loaded once per cycle
    
   zpln.Init();  // table of candidates  
   zgs.ReInit(); // 81 bits patterns in use      
   if(aigstop) break; // in case we would exceed the TCAND limit
               //=========================
   tchain.Init();


	Op.Step(AlignedPairExclusion); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
    if(jdk.AlignedPairN()){Op.SetEr();continue;}  //6.2


    if(Op.ermax<74) // skip that if high ER already reached
	 {Op.Step(AIC_X_cycle); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
      if(jdk.Rating_base_65()){Op.SetEr();continue;}  //6.5 and 6.6 
	  Op.Step(AIC_XY); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
      if(jdk.Rating_base_70()){Op.SetEr();continue;}  //70
	 }
	
	 else if(Op.ermax<77) // just skip 6.5 6.6 as a first step
		  { if(Op.ot) EE.Enl("go direct to XY"); 
			  Op.Step(AIC_XY); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
             if(jdk.Rating_base_70()){Op.SetEr();continue;}  //70
	      }		
	  else	{ if(Op.ot) EE.Enl("gofast");
		       if(zcf.Fast_Aic_Chain()) continue; } 
     
	Op.Step(AlignedTripletExclusion); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
     if(jdk.AlignedTripletN()){Op.SetEr();continue;}  //7.5
     if(jdk.Rating_base_75()){Op.SetEr();continue;}  //7.5
if(Op.oexclude<1)
 {Op.Step(MultipleForcingChain); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
  if(jdk.Rating_base_80()){Op.SetEr();continue;}  //8.0
 
  if(Op.oexclude<2)	 	
   { Op.Step(DynamicForcingChain); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
    if(jdk.Rating_base_85()){Op.SetEr();continue;}  //8.5

 	if(Op.oexclude<3)	
	 {Op.Step(DynamicForcingChainPlus); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
     if(jdk.Rating_base_90()){Op.SetEr();continue;}  //9.0

	 if(Op.oexclude<4)	
	 { if(Op.oq) // mode quick last step is quick mode 
       { EE.Enl("entry base 95 quick");
		   Op.Step(NestedForcingChain); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
         if(jdk.Rating_base_95_Quick()){Op.SetEr();continue;}  //9.5
       }
	 else
      {  EE.Enl("entry base 95 std");
		  Op.Step(NestedForcingChain); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
       if(jdk.Rating_baseNest(95)){Op.SetEr();continue;}  //9.5
/*
	   EE.Enl("entry base 100 multi chaines dynamic");
		  Op.Step(NestedForcingChain); if(Op.ir>1)return Op.ir; else if(Op.ir)continue;
       if(jdk.Rating_baseNest(95)){Op.SetEr();continue;}  //9.5
*/
	 
	 }
	 } // end if <4
	} // end if <3
  } // end if <2
} // end if <1
	if( Rating_end(200)){Op.SetEr();continue;} // clean the file
	if(Op.ot)T81->Candidats();
     aigstop=2; break;
     }     
//=========================================================================	 
 EE.E("fin traitement aigstop=");EE.Enl(aigstop );
 gg.Image("fin");
return aigstop;}

int JDK::Traite_a()
{	if   (Directs ())
	  {Op.Step(LastCell); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	   if( FaitDirects(LastCell)) {Op.SetEr();return 1;}   //1.0

	   Op.Step(SingleBox); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	   if( FaitDirects(SingleBox)) {Op.SetEr();return 1;}  //1.2

	   Op.Step(Single_R_C); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	   if( FaitDirects(Single_R_C)) {Op.SetEr();return 1;}  //1.5
	  }
	Actifs(); // update of active cells must be done from here

	Op.Step(Single_after_Locked); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(TraiteLocked(Single_after_Locked)){Op.SetEr();return 1;}  //1.7

	Op.Step(HiddenPair_single); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(2,1,1)){Op.SetEr();return 1;}  //2.0


	if   (Directs ())
	  {Op.Step(NakedSingle); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	   if( FaitDirects(NakedSingle)) {Op.SetEr();return 1;}}  //2.3


    Op.Step(HiddenTriplet_single); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(3,1,1)){Op.SetEr();return 1;}  //2.5

	Op.Step( Locked_box); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(TraiteLocked( Locked_box)){Op.SetEr();return 1;}  //2.6

	Op.Step( Locked_RC); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(TraiteLocked( Locked_RC)){Op.SetEr();return 1;}  //2.8

	Op.Step(NakedPair); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(2,0,0)){Op.SetEr();return 1;}  //3.0


	Op.Step(XWing); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.XW(2)){Op.SetEr();return 1;}  //3.2

	Op.Step(HiddenPair); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(2,1,0)){Op.SetEr();return 1;}  //3.4

	Op.Step(Naked_triplet); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(3,0,0)){Op.SetEr();return 1;}  //3.6

	Op.Step(swordfish); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.XW(3)){Op.SetEr();return 1;}  //3.8

	Op.Step(HiddenTriplet); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(3,1,0)){Op.SetEr();return 1;}  //4.0

    jdk.Copie_T_c(); // to be done now copie for UR same rating
    zpaires.CreerTable();  

	Op.Step(XYWing); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(zpaires.XYWing()){Op.SetEr();return 1;}  //4.2

	Op.Step(XYZWing); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(zpaires.XYZWing()){Op.SetEr();return 1;}  //4.4

    if(Op.ot)T81->Candidats();

	Op.Step(UniqueRect1); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	if(T81->RIN()){Op.SetEr();return 1;}  //4.5

	Op.Step(UniqueRect2); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(urt.Traite(46)){Op.SetEr();return 1;}  //4.6 t
    if(zpaires.UL()){Op.SetEr();return 1;}    


	Op.Step(UniqueRect3); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	 if(urt.Traite(47)){Op.SetEr();return 1;} //4.7
	 if(tult.Traite(47)){Op.SetEr();return 1;} 

	Op.Step(UniqueLoop1); if(Op.ir>1)return 0; else if(Op.ir)return 1;
     if(urt.Traite(48)){Op.SetEr();return 1;} //4.7
	 if(tult.Traite(48)){Op.SetEr();return 1;}  //4.8
	                  	
	Op.Step(UniqueLoop2); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	if(tult.Traite(49)){Op.SetEr();return 1;}  //4.9

	Op.Step(NakedQuad); if(Op.ir>1)return 0; else if(Op.ir)return 1;
 	if(tult.Traite(50)){Op.SetEr();return 1;}  //4.9
    if(yt.Tiroirs(4,0,0)){Op.SetEr();return 1;}  //5.0

	Op.Step(UniqueLoop3); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	if(tult.Traite(51)){Op.SetEr();return 1;}  //5.1
 
	Op.Step(Jellyfish); if(Op.ir>1)return 0; else if(Op.ir)return 1;
	if(tult.Traite(52)){Op.SetEr();return 1;}  //5.2
    if(yt.XW(4)){Op.SetEr();return 1;}  //5.2

	Op.Step(HiddenQuad); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(yt.Tiroirs(4,1,0)){Op.SetEr();return 1;}  //5.4


	Op.Step(BUG); if(Op.ir>1)return 0; else if(Op.ir)return 1;
    if(zpaires.BUG()){Op.SetEr();return 1;}  //5.6 a 5.8

	if(zpaires.aigun)
	 {Op.Step((SolvingTechnique)59); if(Op.ir>1)return 0; else if(Op.ir)return 1;
      if(zpaires.Bug3a(59)){Op.SetEr();return 1;}  //5.9
	  Op.Step((SolvingTechnique)60); if(Op.ir>1)return 0; else if(Op.ir)return 1;
      if(zpaires.Bug3a(60)){Op.SetEr();return 1;}  //6.0
	  Op.Step((SolvingTechnique)61); if(Op.ir>1)return 0; else if(Op.ir)return 1;
      if(zpaires.Bug3a(61)){Op.SetEr();return 1;}  //6.1
	}

return 2;}

