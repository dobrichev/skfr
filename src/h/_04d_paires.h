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
// class collecting cell with 2 digits to find XWings and XYZ wings
// basis for all kinds of bugs as well
// also used in tagging to establish corresponding stong links
class PAIRES
 {public: BF16 pa; USHORT i8;
  void Charge(P81 & p8) {pa=p8.v.cand;i8=p8.f->i8;};
  };

 
class TPAIRES 
{public: 
 PAIRES zp[80],zpt[80];  // collection of bivalues in cells
 int izpd[50];           // start/end index in zp for tp table
 BF16 tp[50],           // collection of different possibilities for bivalues in cells
       el_par_ch[27],    // parity fo digits in pairs in any elements
       el_par2_ch[27],   // parity copie for final check
	   candp_or,         // within an elem parity or for paired cells
	   candnp_or,        // same for non paired cells
	   candp_xor,        // parity for paired (redundancy)
	   candnp_xor,       // parity for non paired cells
	   wplus[10];        // BF16 within the element for non pair cells (virtual clearing)

 BF81 zplus;            // cells with more than 2
 int tplus[10],ntplus;    // same in table limited to 8
 USHORT tpa[10],         // table for pairs in tha active row/col/bx
	    ntpa,            // and number of pairs
        nwp;             // index for wplus
 int ip,np,aigpmax,aigun,brat;
 void CreerTable();
 int CommunPaires (int i, int j){return T81t[zp[i].i8].ObjCommun (& T81t[zp[j].i8]) ;}
 int CommunTrio (int i, int j){return T81t[i].ObjCommun (& T81t[zp[j].i8]) ;}
 int XYWing(); 
 int XYZWing();
 int UL();           // called in SE for type 1 storing others
 int BUG();  // process all bugs forms
 int Bug1(); int Bug2(); int Bug3a(int rat);
 int Bug3(int el);int Bug_lock(int el);
 int Bug3_4_Nacked(int el);
 int Nacked_Go(BF16 welim);
 void BugMess(char * lib);
 void CommunLib (int i,int j,int k,char * lib) 
   {if(!Op.ot) return;
    EE.E(lib);  EE.E(t81f[k].pt);EE.E(" ");EE.E(T81t[k].scand);	 
    EE.E(" (1)=");EE.E(t81f[zp[i].i8].pt);EE.E(" ");EE.E(T81t[zp[i].i8].scand);
    EE.E(" (2)=");EE.E(t81f[zp[j].i8].pt);EE.E(" ");EE.Enl(T81t[zp[j].i8].scand);	
    } 


} ;


class UL_SEARCH
{public:
 TPAIRES * tpa;          // calling TPAIRES
 PAIRES * pa;            // subtable in TPAIRES::zpt for processed bivalue
 USHORT npa;             // size of pa for that bivalue 
 BF32 el_used,parity;   // to chexk Used sets 
 BF81 cells;            // to check used cells 
 BF16 chd,cht;          // starting pair ; total digits included
 USHORT nadds,adds[8],   // cells with more digits
	    tcount[40],      // storing the list or cells forming the loop 
	    c1,c2,           // the 2 digits
        elcpt[27],       // count per elem
		last,            // last cell number
        line_count;      // must end with more than 4 to have a valid UL (not a UR)
 P81F  p;            // last cell row col box 
 UL_SEARCH (){}      // empty constructor for the storing table below
 UL_SEARCH (UL_SEARCH * old) {(*this) = (*old);}
 UL_SEARCH ( BF16 c,TPAIRES * tpae,PAIRES * pae,USHORT npae )
	 // initial for a new start
     {tpa=tpae;pa=pae;npa=npae;
	  chd=cht=c; nadds=line_count=0; 
	  char * st=c.String(); c1=st[0]-'1';c2=st[1]-'1';
      cells.SetAll_0();el_used.f=parity.f=0;
      for(int i=0;i<27;i++) elcpt[i]=0;
     } 
 void Copie (UL_SEARCH *x) {(*this)=(*x);}
 void Set(int i8);  //a new cell is added to the search
 int Add_Chain (int i8);
 int Loop_OK(int action=0); 
 int Valid_Set(P81F f,char c) 
    {int el=f.el;if(c=='c') el=f.pl+9; else if(c=='b')el=f.eb+18;
     if(el_used.Off(el)) return el; 
     return -1;}
 int Is_OK_Suite(USHORT i8) ;
 int El_Suite(USHORT el);
 void UL_Mess(char * lib,int pr);
private:
	// Check if the loop has more than 0 solutions by parity
	bool ParityCheck(void);

};


// ULT is a table storing possible UL rating more than 4.6 for further processing
class ULT  
{public:
UL_SEARCH ult[20];  
int n;
void Init() {n=0;}
void Store(UL_SEARCH *x) {if(n<20) ult[n++].Copie(x);}
 // the processing is lenght dependant
int Traite(int rat) 
	{// up to 6 cells basis=4.6 (+0.1)  7_8 cells 4.7 (+0.2)  10_.. 4.8 (+0.3)
  	 //EE.E("recherche UL rating =");EE.E(rat);EE.E(" n=");EE.Enl(n);
      for(int i=0;i<n;i++)
	  {	int len=ult[i].line_count,irat,rbase=46;	
	    if(len>7) rbase++; if(len>9) rbase+=3;//  ?? what is the rule  ??
		// now the relative difficulty for loop_ok nand UR common processing
		irat=rat-rbase+1;if(irat <1 || irat>4 )continue;		
		if(ult[i].Loop_OK(irat))return 1; 
	  }
	 return 0;}
};

/* searching a UL
  start on a "tp" in PAIRES (one 2 digits combinason giving bivalue cells)
  take in sequence one of the corresponding cells in zpt as start
      give up if not start in row/col
  then try to go ahead, looking for next line
     priority 1) row/col bivalue  2) box bivalue 
	          3)row/col bi value plus 4) box with bivalues plus
      note : use of box catches a double Xwing type pattern with additionnal digits
   no reuse of a set, no reuse of an object (el)
   stop true if the process loops with even count in all elems and more than four lines
   stop false if more than 2 adds with more than one extra digit
              if no new line found
  (SE) if the UL is "first rating"  do it else store it for later processing.
*/ 