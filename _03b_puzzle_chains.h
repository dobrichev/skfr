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

// that table contains 81 bits pattern in use in the process
// the 81 first entries are the 81 bit patterns corresponding to each of the 81 cells
// next 54 entries were assigned to common cells row/box and col/box;
// more  are filled upon request.
// it could be that in SE clone only the first 81 entries are used
// in the table storing candidates or group of candidates, we have a pointer to ZGROUPE

/* the class PATH has been specifically designed for SE clone
   it is a support to look for the shortest path 
     after elimination/cycle has been found in tags terms
   having that class designed, all path are using it
*/
class PATH     
{public: USHORT pth[100],ipth,ipth_plus;
inline void Init(){ipth=ipth_plus=0;}
inline void Set(USHORT i,USHORT x){pth[i]=x;}
inline void Add(USHORT x){if(ipth<50)pth[ipth++]=x;}
void Add(USHORT * pthp,USHORT ipthp)
        {for(int i=0;i<ipthp;i++) Add(pthp[i]);}

void PrintPathTags();

inline USHORT GetLast(){if(ipth) return pth[ipth-1];else return 0;}
inline USHORT GetFirst(){if(ipth) return pth[0];else return 0;}
inline USHORT Get(int x){return pth[x];}
inline void Back(){ipth--;}

} ;  

/* the following classes have been designed specifically for SE clone.
   the goal is to find and store chains type eliminations 
   all the necessary stuff for printing is stored
   this is also the place where we keep the filter to limit the search of paths*/

 /* 6.2: Aligned Pair Exclusion
    6.5 - 7.5: X-Cycles, Y-Cycles
    6.6 - 7.6: Forcing X-Chains Y_Chains
    7.0 - 8.0: Forcing Chains, XY-Cycles
    7.5: Aligned Triplet Exclusion
    7.5 - 8.5: Nishio
    8.0 - 9.0: Multiple chains
    8.5 - 9.5: Dynamic chains
    9.0 - 10.0: Dynamic chains (+)
	     requiring another storing organization
    9.5 - ?? : Nested Forcing Chains
    
 */
/*a specific entry is created for each candidate eliminated
  No need to store a detailed explanation of an elimination
     in test mode, this must be done immediately
  storing is limited to 
         the candidate eliminated   
	     the milestone to find back the explanation in test mode
  only chains (up to 30) with the same rating are stored

   */

class CHAIN {  //defining a chain and elimination in one buffer
public: 
	USHORT cand,         //candidate to clean
		urem;        // milestone of the explanation

	inline void Load(USHORT cande);
	int Clean(); // clear candidates
};   

class TCHAIN { // storing up to 20 chains having the same "lowest"rating
public:      
	USHORT rating,             // current rating 
		maxlength;          // filter for the search of a path (tags)
	CHAIN chainw, chains[30]; // enough to store several small loops 
	USHORT ichain,           // index to chains 
		base;

	void Init() {
		rating = 200;
		ichain = 0;
		maxlength = 10;
	}

	// the following functions relates to performance control in the search of eliminations
	// should be revised in the new process
	// the first function set the default value for the base

	void SetMaxLength(int bw)  ;
	inline int GetYmax() {
		return maxlength;
	}
	USHORT ParsingMini(USHORT basee);
	int GetYlower(int lg) ;
	// added for the new design
	int GetLimit(int lg, int modexy) ;

	// entry in tchain is either with an explicit base of with the default value

	int GetRatingBase(USHORT bb, USHORT lengthe, USHORT cand);
	int GetRating(USHORT lengthe, USHORT cand) {
		return GetRatingBase(base, lengthe, cand);
	}

	void LoadChain(USHORT rat, char * lib, USHORT cand);

	//inline int IsOK(USHORT x);
	int IsOK(USHORT x);

	int Clean() { // clear all pending candidates
		int ir = 0;
		for(int i = 0; i < ichain; i++)
			ir += chains[i].Clean();
		return ir;
	}

	void Status();
};// tchain ;








#define zgs_lim 10000
class ZGROUPE     // en fixe 81 points 8       puis  54 groupes
 {public: BF81 z[zgs_lim];
  PUZZLE * parentpuz;
  USHORT iz;
  ZGROUPE(PUZZLE * parent); 
  void ReInit(){iz=81+54;} // first after compulsory entries
  inline BF81 getz(int i) {return z[i];};
  USHORT Charge(BF81 & z);
  };



class CANDIDATE {
public:  
	USHORT   ch,  // digit
		ig;  // cell id

	inline void Charge(USHORT i,USHORT che) {ig=i;ch=che;}
	inline BF81 * GetZ();     
	void Image(int no=0); // no=1 if false state
	void Clear();
};

#define zpln_lim 320
class CANDIDATES {
public:
	   PUZZLE * parentpuz;
	   CANDIDATE zp[zpln_lim];
	   BFCAND candtrue; 
	   USHORT ip;            // index to zp
	   USHORT indexc[9*81];  // digits 81 cells -> cand
	   USHORT ptsch[10],iptsch, // to generate weak links
		   el;               

	   USHORT Getch(int i){return zp[i].ch;};

	   CANDIDATES(PUZZLE * parent) {parentpuz=parent;}
	   void Init();
	   USHORT Charge0();

	   void CellStrongLinks(); // Y mode
	   void CellLinks();   // including bivalues  not done in 'X'mode 
	   void RegionLinks(int biv)  // bivalues  except in Y mode
	   {for(USHORT ich=0;ich<9;ich++) RegionLinks(ich,biv); 
	   }
private:
	void RegionLinks(USHORT ch,int weak);   
	void WeakLinksD();
public:
	void GenRegionSets();  
	void GenCellsSets();

	//========= TPT1 work within a layer
	// void SuppMarque(USHORT mm,int direct=0);   
	void Clear(USHORT c) {zp[c].Clear();}
	int ChainPath(int i1,int i2);
	void PrintPath(USHORT * tp,USHORT np);
	void PrintImply(USHORT * tp,USHORT np);
	void PrintListe(USHORT * t,USHORT n,int modetag=0);

	void Image(int i,int no=0) {zp[i].Image(no);  }
	void ImageTag(int i){Image(i>>1,i&1);}
	void Liste();        // for debugging only
	void ListeIndexc();  // for debugging only
};  


 
class SQUARE_BFTAG {
public:  
static BFTAG done[50]; // already in the path
static USHORT ich,mode,length,mmfin;
static PATH path, pathr;
static USHORT   maxsearch,  // external limitation for path search
	            maxlength,  // search limit as of the existing rating
				maxichain,  // ongoing search value
	            parsediag,  // debugging only
				parsecount; // length in parsing mode
static BFCAND tbf_end,tbf_endok;// one for each way

BFTAG t[BFTAG_BitSize],parents;	

void Init() {
	t[0].SetAll_0();
	for(int i=1;i<BFTAG_BitSize;i++) {
		t[i]=t[0];
	}
}
void ExpandAll(SQUARE_BFTAG & from);
void ExpandShort(SQUARE_BFTAG & from,int npas);
void AllParents(SQUARE_BFTAG & from);
int SearchEliminations(SQUARE_BFTAG & from,BFTAG & elims);
inline void Set(int i, int m) {t[i].Set(m);};
inline int Is(int i,int m){return t[i].On(m);};
inline int IsConflit(int m1, int m2) {return Is(m1,m2^1);}
inline int IsOu (int m1, int m2) {return Is(m1^1,m2);}

static inline void InitParsing(){maxsearch=maxlength;}
static inline void SetParsingMini(USHORT x) {
	if(x<maxlength)
		maxsearch=x;
	else maxsearch=maxlength;
}
inline void StoreTbf(){tbf_endok=tbf_end;}
inline void GetBackTbf() {tbf_end=tbf_endok;}

void Parents(USHORT x);
int ExpandToFind(USHORT td,USHORT tf,USHORT lim);
void Shrink(BFTAG & x,USHORT it);
int Plus (int m1,int m2)  {if (!t[m1].On(m2))
		  { t[m1].Set(m2);  return 1; }else return 0;};
int  Entre(int m1,int m2){ return (Plus(m1,m2^1)+ Plus(m2,m1^1));};
void Image();
private:
};


/* enum describes the following situations
  wl_basic       cell of region weak link
  wl_bivalue     cell of region bivalue
  wl_set a -> b  where b is the last valid candidate in a set
  wl_event  a -> event - b 
  wl_ev_direct ~a -> event _ b 
*/
enum WL_TYPE {
	wl_basic,
	wl_bivalue,
	wl_set,
	wl_event,
	wl_ev_direct
};

/*INFERENCES is somehow the key class in the tagging process.
  it contains 
  PHASE a couple of SQUARE_BFTAG bit maps   of the same links in the form a=>~b
    one bit map is the primary table the second one is the extended table.
	each time a new cycle of derivation is done, a "start situation" is needed 
	 this is the hstart  
    dpbase is the relevant situation to start a SE mode process

	some cleaning has still to be done in that class
 */
class INFERENCES {
	class PHASE {
	public:
		SQUARE_BFTAG d, dp;
		int icf;
		void Init() {
			d.Init();
			dp = d;
		}
	};

public:
	PUZZLE * parentpuz;

	PHASE h, hstart, h_one, h_nest;

	SQUARE_BFTAG hdp_base,         // must contain only basic strong and weak links
		         hdp_dynamic,      // same plus direct events effects
				 hdp_base_nested;  // hdp_base plus nested strong links

	USHORT load_done;

	BFTAG  xb, xi, xbr, xbr2; 
	BFCAND isbival;

	INFERENCES(PUZZLE * parent){parentpuz=parent;}

	inline BFTAG * Getd(int m) {
		return & h.d.t[m];
	}
	inline BFTAG * Getdp(int m) {
		return & h.dp.t[m];
	}
	inline int IsConflitD(int m1, int m2) {
		if(m1 == (m2 ^ 1))
			return 1;
		return h.dp.IsConflit(m1, m2);
	}
	inline int IsConflit(int m1, int m2) {
		if(m1 == (m2 ^ 1))
			return 1;
		return h.d.IsConflit(m1, m2);
	}
	inline int IsStart(int i, int j) {
		return hstart.d.Is(i, j);
	}
	inline int Isp(int i, int j) {
		return h.dp.Is(i, j);
	}

	inline void LockNestedOne() {
		h_one = h;
	}
	inline void StartNestedOne() {
		h = h_one;
	}

	void Init() {
		h.Init();
		isbival.SetAll_0();
	} 
	void ExpandAll() {
		h.d.ExpandAll(h.dp);
	}
	void NewPhase() {
		h.d.ExpandAll(h.dp);
	}
	int DeriveCycle(int nd, int nf, int ns, int npas = 0);
	void ExpandShort(int npas) {
		h.d.ExpandShort(h.dp, npas);
	}
	void LoadBase(USHORT cd1, USHORT cd2);
	void LoadBivalue(USHORT cd1, USHORT cd2);
	void LoadDerivedTag(USHORT tg1, USHORT cd2);
	void LoadEventTag(USHORT tg1, USHORT cd2);
	void LoadEventDirect(USHORT cd1, USHORT cd2);


	// that lot is  designed to process X Y XY cycles and eliminations
	// either in std mode or in fast mode
	void Aic_Cycle(int opx); // only nice loops  X or Y
	void Aic_Ycycle(USHORT t1, USHORT t2, BFTAG &loop, USHORT cand);
	void Aic_YcycleD(USHORT t1, USHORT t2, BFTAG &loop, USHORT cand);
	void Aic_YcycleD2(USHORT t1, USHORT t2, BFTAG &loop, USHORT cand);
	int Aic_Ycycle_start(USHORT t1, USHORT t1a, USHORT t2, BFTAG &loop, PATH &result);

	void ExplainPath(BFTAG &forward, int start, int send, int npas, USHORT relay);
	int Fast_Aic_Chain();// quick eliminations high rating reached

	int Nested_QuickForcing(SQUARE_BFTAG &xt, BFTAG &elims); 
	int Nested_QuickMulti(SQUARE_BFTAG &xt, BFTAG &elims); 
	int Nested_QuickDynamic(SQUARE_BFTAG &xt,BFTAG &elims) ; 


private:
	int Plusp(int m1, int m2) {
		return h.dp.Plus(m1, m2);
	}
	int Entrep(int m1, int m2) {
		return (Plusp(m1, m2 ^ 1) + Plusp(m2, m1 ^ 1));
	}
}; 




/* new design for the "sets" table
   all entries in candidates mode + 'event' if any
   no duplicate check
   limit to size SET_max
*/


#define setsbuffer_lim 100000
class SETS_BUFFER   // buffer for candidates + "events"
{public: 
 PUZZLE * parentpuz;
 USHORT zs[setsbuffer_lim],izs,izs_one;
 SETS_BUFFER(PUZZLE * parent){parentpuz=parent;}
 inline void Init(){izs=0;}
 inline void LockNestedOne() {izs_one=izs;}
 inline void StartNestedOne() {izs=izs_one;}
 void GetSpace(USHORT *(& ps),int n);
 
}; //zcxb;   



/* in SE we keep sets in candidate mode (index in CANDIDATES)
   identical sets are sorted out before 
   types identified (enum list) are
   SET_base  cell or region list of candidates
   SET_set   event set, last "candidate" is the event 
*/
enum SET_TYPE{SET_base,SET_set=4};
class SET
{public:      
 USHORT *tcd,  // "set" table pointer to TCXI
        ix,   // index in TSET 
		ncd,   // number of candidates + "event"
		type; // as of SET_TYPE 

 int Prepare (USHORT * mi,USHORT nmi,SET_TYPE ty,USHORT ixe);

 void GetCand(BFCAND & ms)// init à la charge de l'appelant
           {for(int i=0;i<ncd;i++) ms.Set(tcd[i]);}

 inline int ChoixBase(){return (!type);}
 void PrintInterditSimple(USHORT mi,USHORT derive){}//;
 void Image() const;

 } ;




#define sets_lim 20000
class SETS {
public: 
	PUZZLE * parentpuz;
	SET zc[sets_lim];
	USHORT izc,     //index to zc
		izc_one,
		direct,  // 0 except if derivation from direct weak links
		nmmin,nmmax;    //min  max  size seen for a set
	BFTAG tce[20],  // storing used BFTAG in a set analyzed
		*t;       // pointer to zcf.h.d.t or zcf.h.dp.t depending on direct
	SQUARE_BFTAG allparents; // table for derivation

	SETS(PUZZLE * parent){parentpuz=parent;}
	void Init();
	inline void LockNestedOne() {
		izc_one = izc;
	}
	inline void StartNestedOne() {
		izc=izc_one;
	}

	int ChargeSet(USHORT * mi, USHORT nmi, SET_TYPE ty);
	int CopySet(int ie);

	void DeriveDirect() {
		direct = 1;
		Derive(3, 4, 3);
		direct=0;
	}
	void Derive(int min, int max, int maxs); 
	void DeriveBase(SET & chx);
	void DeriveSet(SET & chx);

	int Interdit_Base80() ;	 

	int CheckGoNested1(const BFTAG &bftag, USHORT cand);

	void Image();

}; //zcx ;  



