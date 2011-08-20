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
#pragma once
enum WL_TYPE  {wl_basic,wl_bivalue,wl_set,wl_event,wl_ev_direct};

/* enum describes the following situations
  wl_basic       cell of region weak link
  wl_bivalue     cell of region bivalue
  wl_set a -> b  where b is the last valid candidate in a set
  wl_event  a -> event - b 
  wl_ev_direct ~a -> event _ b 
*/

/*TZCF is somehow the key class in the tagging process.
  it contains 
  PHASE a couple of TDB bit maps   of the same links in the form a=>~b
    one bit map is the primary table the second one is the extended table.
	each time a new cycle of derivation is done, a "start situation" is needed 
	 this is the hstart  
    dpbase is the relevant situation to start a SE mode process

	some cleaning has still to be done in that class
 */
class TZCF
{  class PHASE{public: TDB d,dp; int icf;
               void Init(){d.Init();dp=d;}};

   public:  PHASE h,hstart,h_one;
   TDB dpbase;
   USHORT ic, iphase,ic_one,
	      ic1;   // value of ic at the end of the common loading phase
   BFTAG  xb,xi,
		   xbr,xbr2; 
   BFCAND tbf[BFCAND_BitSize],tbfwl[BFCAND_BitSize],isbival;


   inline BFTAG * Getd(int m) {return & h.d.t[m];};
   inline BFTAG * Getdp(int m) {return & h.dp.t[m];};
   inline int IsConflitD(int m1,int m2)   
         {if(m1==(m2^1)) return 1; return h.dp.IsConflit(m1,m2);}
   inline int IsConflit(int m1,int m2)   
         {if(m1==(m2^1)) return 1; return h.d.IsConflit(m1,m2);}
   inline int IsOu (int m1,int m2){return  IsConflit(m1^1,m2^1);}
   inline int Is(int i,int j)  {return h.d.Is(i,j);};
   inline int IsStart(int i,int j)  {return hstart.d.Is(i,j);};
   inline int Isp(int i,int j) {return h.dp.Is(i,j);};
   inline int IsXorp( int i,int j)
		  {return (  h.dp.Is(i,j^1) &&   h.dp.Is(i^1,j)) ;};
   inline int IsCandToCand(USHORT cd1,USHORT cd2)
          {return tbf[cd1].On(cd2);}
   inline void CloseOne(){ic1=ic;dpbase=h.dp;}
   inline void ResetOne(){ic=ic1;h.dp=dpbase;}
   inline void LockNestedOne() {ic_one=ic;h_one=h;}
   inline void StartNestedOne() {ic=ic_one;h=h_one;}

   void Init() {ic=1;iphase=0;h.Init();isbival.SetAll_0();
                tbf[0].SetAll_0(); tbfwl[0]=tbf[0];
				for(int i=0;i<BFCAND_BitSize;i++) {tbf[i]=tbf[0];tbfwl[i]=tbf[0];}
                } 
   void ExpandAll() {h.d.ExpandAll(h.dp);}
   void PlusPhase() {iphase++;} 
   void NewPhase(){h.d.ExpandAll(h.dp);  PlusPhase();  }
   int DeriveCycle(int nd,int nf,int ns,int npas=0);
   void ExpandShort(int npas) {h.d.ExpandShort(h.dp,npas);}
   void LoadBase (USHORT cd1 ,USHORT cd2);
   void LoadBivalue (USHORT cd1 ,USHORT cd2);
   void LoadDerivedTag (USHORT tg1 ,USHORT cd2);
   void LoadEventTag (USHORT tg1 ,USHORT cd2);
   void LoadEventDirect (USHORT cd1 ,USHORT cd2);



 //  int LChemin(USHORT m1,USHORT m2)
 //           {h.d.Parents(m2); h.d.parents.Set(m2);
 //            h.dp.Direct_Path(m1,m2,h.d.parents); 
  //            return h.dp.length;}
 
     // that lot is  designed to process X Y XY cycles and eliminations
     // either in std mode or in fast mode
   void  Aic_Cycle(int opx) ; // only nice loops  X or Y
   void  Aic_Ycycle(USHORT t1,USHORT t2,BFTAG & loop,USHORT cand);
   void  Aic_YcycleD(USHORT t1,USHORT t2,BFTAG & loop,USHORT cand);
   void  Aic_YcycleD2(USHORT t1,USHORT t2,BFTAG & loop,USHORT cand);
   int Aic_Ycycle_start(USHORT t1,USHORT t1a,USHORT t2,BFTAG & loop,PATH & result);
   void ExplainPath(BFTAG & forward,int start,int send,int npas,USHORT relay);
   int Fast_Aic_Chain();// quick eliminations high rating reached

   int  Nested_QuickForcing(TDB & xt,BFTAG & elims) ; 
   int  Nested_QuickMulti(TDB & xt,BFTAG & elims) ; 
   int  Nested_QuickDynamic(TDB & xt,BFTAG & elims) ; 
    
   void  ChainPlus(BFCAND & dones);
 
  private:
    int LoadVoid(USHORT cd1 ,USHORT cd2,WL_TYPE type,USHORT ph); 
    int Plusp (int m1,int m2) {return h.dp.Plus(m1,m2);};
    int Entrep(int m1,int m2){ return(Plusp(m1,m2^1)+Plusp(m2,m1^1));};
 }zcf;  


