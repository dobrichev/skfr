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


class GG {
public:
	// a grid with clues (givens)
	char g [9][9],	///<The grid
		filler,		///<a null to terminate 81 char string
		*pg;		///<a pointer to the beginning of the string

	GG();		// constructor

	inline char * Getpg() {
		return pg;
	}
	//! Copy the grid value
	inline void Copie(GG & ge) {
		strcpy_s(pg, 82, ge.pg);
	} // copie 81 caractères
	
	inline void operator =(GG &ge) {
		strcpy_s(pg,82,ge.pg);
	}
	//! Get the empty cell count
	int NBlancs();
	  
	//! Get the valued cell count
	int Nfix();
	
	//! Print on output file a representation 9x9 of the grid
	/**
	 * Print on a first line the string passed as parameter. Print 9 more lines
	 * representing the grid with line numbers.
	 */
	void Image(char * lib);
};


//! Small class containing non permanent data for a cell
/**
 * These data are candidates, number of candidates and status of the cell (free, given or assigned)
 */
class CELL_VAR {
public:  
	USHORT ncand;	///< number of candidates
	USHORT typ;		///< status of the cell : 0 free  2 given  1 assigned
	BF16 cand;		///< bitfiel representing the candidates

	//! Initialize the cell as free with all candidates On
	void Init()
	{
		ncand=9;		// 9 candidates on
		cand.f=0x1ff;	// all bits on
		typ=0;			// free
	}

	//! Gives a value to a cell (without changing status of the cell)
	inline void Fixer(USHORT i)
	{
		ncand=1;		// 1 candidate
		cand.f=(1<<i);	// only one bit
	}   

	//! Gives a value (given or by resolution) to a cell
	inline void Fixer(USHORT t,USHORT i){typ=t;Fixer(i);}

	//! Clear a candidate for this cell
	/** \return 0 if the candidate was already Off */
	inline int Clear (int i) {
		if(cand.On(i))
		{
			ncand--;	// one candidate less
			cand.f ^= (1<<i); // clear bit
			return 1;
		}
		return 0;
	}

	//!Get first candidate On
	/**
	* <b>BUG :</b> There is a problem in this method. It returns 0 
	* if position 0 is On or if none ! 
	*/
	USHORT GetCand() {
		for(USHORT i=0;i<9;i++)
			if(cand.On(i))
				return i;
		return 0;
	}
};

// class describing and managing a cell.
// as a matter of fact, data have been split in 2 classes, 
//   CELL_FIX permanant data
//   CELL _VAR  non permanant data
// data remaining are related to tagging printing
// the main table is the basic entry to eliminate candidates
class CELL {
public:   
   char scand[10];         // printing data
   CELL_FIX * f; 
   CELL_VAR v;
   void Init(){v.Init();}     // at the start
   int Change (int ch) ;      // clear one candate
   int Change (BF16 cb9) {    // clear candidates 
	   int ir=0;
	   for(int i=0; i < 9; i++) 
		   if(cb9.On(i))
			   ir += Change(i);
	   return ir;
   }
   int Keep (BF16 cb9) {       // clear candidates others than
	   int ir=0;
	   for(int i = 0; i < 9; i++) {
		   if(v.cand.On(i) && !cb9.On(i)) {
			   Change(i);
			   ir=1;
		   }
	   }
	   return ir;
   }
   int Keep(int ch1,int ch2) { // clear candidates others than
	   int ir = 0;
	   for(int i = 0; i < 9; i++) {
		   if(v.cand.On(i) && (i - ch1) && (i - ch2)) {
			   Change(i);
			   ir = 1;
		   }
	   }
	   return ir;
   }
   int ObjCommun(CELL * p8) {
	   return f->ObjCommun(p8->f);
   }   // same row, column or box

   void Fixer(UCHAR type,UCHAR ch) { // force digit as valid in the solution
	   v.Fixer(type, ch);
	   scand[1] = 0;
	   scand[0] = '1' + ch;
   }
   char * strcol() {
	   return scand;
   } // no tagging in print
};

// class collecting all datas for the cells and managing a cell oriented process

class TP81 { 
public:
	CELL t81[81];       

	void init(); 	
	void Fixer(int ch,int i8,UCHAR typ);
	int Clear(BF81 &z,int ch  );
	int Clear(BF81 &z,BF16 ch  );
	int CheckClear(BF81 &z,BF16 ch  );
//	void GenzCand(BF81 & z1,BF81 & z2,int ic);
	BF16   GenCand(BF81 & z);    
	BF16   GenCandTyp01(BF81 & z);
	void Actifs(BF81 & z);
	int RIN(int aig=0);
	void Candidats() {
		//if(Op.ot)
			CandidatsT();
	}
	//void Candidatsz() 
	//{
	//	if(!Op.ot)return;
	//	EE.Enl("justificatif a posteriori"); 
	//	CandidatsT();
	//}
	
private: 	
	void CandidatsT();
};

// several small classes to work on a view per object/pos/digit
//                                         or  object/digit/pos
// updated at the beginning of a new cycle

//! For one digit positions and number of positions
/**
 * <b>QUESTION : </b> Is there an other use like <br>
 * for one cell candidates and number of candidates see OBBITD::Genpo
 */
class OBBITD {
public:
	BF16 b;  //< bitfield for 9 positions
	USHORT n; //< number of postions
	//! Set everything to 0
	void Raz(){b.f=0;n=0;};
	//! Set the position <code>i</code> and increment number
	void Set(int i) {b.Set(i);n++;};
	//! Generate from CELL _VAR <code>p8</code>
	void Genpo(CELL_VAR p8){ n=p8.ncand;b=p8.cand;   };
};

//! vector for 9 cells or 9 digits  
class OBBIEL {
public: 
	OBBITD eld [9];
	//! TO EXPLAIN
	/** Defined in file ecs.cpp */
	int tiroir(BF16 fd, int iold, int irang);
};

//! grouping the 27 regions : rows/cols/boxes
class TOBBIT {
public: 
	OBBIEL el[27];  //< possible position of each digit for each region
};

//! Main class giving regional alternative index or candidates
/**
 * There is a unique instance of this class 
 * Positions are updated once per cycle
 */
class ZTOB {
public: 
	TOBBIT tpobit;  // entry region -> position -> digit
	TOBBIT tchbit;  // entry region -> digit -> position

    void Genere();  // update
};



class PUZZLE
{
public:
    GG gg;          //< copy of the puzzle (normalized form - empty cell '0')
    TP81 tp8N,
		tp8N_cop;  
	BF81 
		c[9],
		c_cop[9];	// grille globale et par chiffre
    BF81 zactif,
		elza81[27],
		csol[9];   //< Solution as positions of the 9 digits
    BFTAG pointK;


    int coup,
		coupMM,
		couprem; 
    char fix[81],
		fixt[81],
		*solution;
    USHORT col,           // highest tag (2 times ( number of candidates +1))
		   stop_rating,   // set to 1
		   nfix;

	// data for the rating 
    USHORT
		 rating_ir,		//< return code for Step()
	     cycle,		//< counting cycles in the main loop
	     assigned,  //< counting assigned in the main loop
	     ermax,     //< final er if ok or cancelled due to filters
	     epmax,     //< final ep if ok or cancelled due to filters
	     edmax,     //< final ed if ok or cancelled due to filters
	     difficulty,  //< storing last call for difficulty used to set er ep ed
          //! store the return code status
	            /**
	              bit 0 =1 if "not rated" or "spit not ok"
	              bit 1 =1 if end in "error" or "not finished"
	              bit 2=1 if not "diamond" "pearl" with that option
	              bit 3=1 if split ok (bit 1 = 0)
	            */
	     c_ret;



	PUZZLE();


    /* set of routines previously in opsudo */

		//! filter on ED 
	int is_ed_ep();   // at the start of a new cycle
	
	int Is_ed_ep_go();

	void Step(SolvingTechnique dif)  ;  // analyse what to do at that level

	inline void SetDif(USHORT dif){difficulty=dif;} // sub level inside a process

	void SetEr();   // something found at the last difficulty level  

	inline void Seterr(int x)  // an error condition has been found
    {ermax=0; epmax=0;edmax=x;
	} 





    //void Initial(); 
	void cInit(int un=0);
	void Copie_T_c();
    void cFixer(int ich,int i8);
    void cReport();   
    void TReport();   
    void Actifs();
    int CheckChange(int i, int ch);
    int ChangeSauf(int elem,BF16 pos,BF16 chiffres );
    int Keep(int elem,BF16 pos,BF16 chiffres );
    int Keep(int ch, const BF81 &zk); // éliminer ailleurs en objets contenant
    int Keep(int ch, USHORT p1, USHORT p2); // éliminer ailleurs de deux points
    int NonFixesEl(int e);
    void FixerAdd(int i, char c, int elt)
    {
		fix[i]=c;
		fixt[i]=(char)elt;
		nfix++;
	};

	//! verify  if the puzzle is correct
	/**
	 * Verify only that each house has no duplicate givens
	 * No check on the number of solutions
	 * \return 0 not correct, 1 correct
	 */
    int Check();
    int Recale();              
	int Traite();
	int Traite_a();
	int Traite_b();
	int Traite_c();
    int Directs();             
	int FaitDirects(int type);
 	int FaitGoA(int i8, char c1, char c2) {
		int ir = FaitGo(i8, c1, c2);
		cReport();
		return ir;
	}
    int TraiteLocked(int rating);  // start for locked in row,cil,box
    int TraiteLocked2(int eld, int elf); // detail for ratings 2.6  2.8
    void PKInit() {
		couprem = 0;
		pointK.SetAll_0();
	}
    void PointK();
    void UsePK(USHORT i);
	// tagging process added here in that version
    void TaggingInit();
	void GenCellBivalues();
	void GenYBivalues();
	void BiValues();
	void BiValues(USHORT ch);
    int AlignedPairN();
	int AlignedTripletN();
	void Chaining(int option, int chain_level, int base);
	int Rating_end(int next);
	int Rating_base_65();  // x or y cycles
	int Rating_base_66();  // x or y cycles
	int Rating_base_70();  // xy chains xy cycles ?
	int Rating_base_75();  // align tripl exclus  //  nishio
	int Rating_base_80();  // dynamic chains
	int Rating_base_85();  // multiple chains
	int Rating_base_90();  // dynamic plus 
	void InitNested();
	int Rating_baseNest(USHORT base, int quick);  // nesting  95 forcing chains
	int Rating_base_95_Quick();  // quick nesting   
	void Rating_Nested(USHORT base, USHORT * ttags, USHORT ntags, USHORT target);
    void ImagePoints(BF81 & zz) const;  
	void ImageCand(BFCAND & zz ,char * lib) const;
	void GetCells(BFCAND & zz,BF81 & cells) const;
	void Image(const BFTAG & zz,char * lib, int mmd) const;
 	void Elimite(char * lib);
	void Estop(char * lib);

private:
	int FaitGo(int i8,char c1,char c2);
};



//That class is dedicated to the processing of algorithms building 
//  a progressive collection of cells
// looking for locked sets
// looking for fishes.

// such algorithms are using  recursive routines 

class TIR 
{
public:
	BF16 non,
		cases,
		wf,
		wi; 
	int rangc,
		rangv,
		e,
		e27,
		typeqc,
		aiggo,
		single,
		hid_dir; //0 dir,1 hidden; 2 both
	OBBIEL * el;
	UCHAR ch; 

	void InitTir(BF16 none,BF16 casese, int rangc,int rangv);

	void InitXW();

	int Tiroirs(int nn,int hidden,int sing);

	int UnTiroir();

	int XW(int nn);  

	int XW(BF16 fd ,int iold,int irang);

	int GroupeObjetsChange(int decalage,int ch);
 }; 




// class collecting cell with 2 digits to find XWings and XYZ wings
// basis for all kinds of bugs as well
// also used in tagging to establish corresponding stong links
class PAIRES {
public:
	BF16 pa;
	USHORT i8;
	void Charge(CELL & p8) {
		pa = p8.v.cand;
		i8 = p8.f->i8;
	}
};

class TPAIRES {
public: 
	PAIRES zp[80], zpt[80]; // collection of bivalues in cells
	int izpd[50];           // start/end index in zp for tp table
	BF16 tp[50],            // collection of different possibilities for bivalues in cells
		el_par_ch[27],    // parity fo digits in pairs in any elements
		el_par2_ch[27],   // parity copie for final check
		candp_or,         // within an elem parity or for paired cells
		candnp_or,        // same for non paired cells
		candp_xor,        // parity for paired (redundancy)
		candnp_xor,       // parity for non paired cells
		wplus[10];        // BF16 within the element for non pair cells (virtual clearing)

	BF81 zplus;             // cells with more than 2
	int tplus[10], ntplus;  // same in table limited to 8
	USHORT tpa[10],         // table for pairs in tha active row/col/bx
		ntpa,            // and number of pairs
		nwp;             // index for wplus
	int ip, np, aigpmax, aigun, brat;
	void CreerTable();
	int CommunPaires(int i, int j);
	int CommunTrio(int i, int j);
	int XYWing(); 
	int XYZWing();
	int UL();           // called in SE for type 1 storing others
	int BUG();  // process all bugs forms
	int Bug1();
	int Bug2();
	int Bug3a(int rat);
	int Bug3(int el);
	int Bug_lock(int el);
	int Bug3_4_Nacked(int el);
	int Nacked_Go(BF16 welim);
	void BugMess(char * lib);
	void CommunLib(int i, int j, int k, char * lib);
};

class UL_SEARCH {
public:
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
	CELL_FIX  p;            // last cell row col box 
	UL_SEARCH() {}      // empty constructor for the storing table below
	UL_SEARCH(UL_SEARCH * old) {
		(*this) = (*old);
	}
	UL_SEARCH(BF16 c, TPAIRES * tpae, PAIRES * pae, USHORT npae) {
		// initial for a new start
		tpa = tpae;
		pa = pae;
		npa = npae;
		chd = cht = c;
		nadds = line_count = 0; 
		char * st = c.String();
		c1 = st[0] - '1';
		c2 = st[1] - '1';
		cells.SetAll_0();
		el_used.f = parity.f = 0;
		for(int i = 0; i < 27; i++)
			elcpt[i] = 0;
	}
	void Copie(UL_SEARCH *x) {
		(*this) = (*x);
	}
	void Set(int i8);  //a new cell is added to the search
	int Add_Chain(int i8);
	int Loop_OK(int action = 0); 
	int Valid_Set(CELL_FIX f, char c) {
		int el = f.el;
		if(c == 'c')
			el = f.pl + 9;
		else if(c == 'b')
			el = f.eb + 18;
		if(el_used.Off(el))
			return el; 
		return -1;
	}
	int Is_OK_Suite(USHORT i8);
	int El_Suite(USHORT el);
	void UL_Mess(char * lib, int pr);
private:
	// Check if the loop has more than 0 solutions by parity
	bool ParityCheck(void);
};

// ULT is a table storing possible UL rating more than 4.6 for further processing
class ULT {
public:
	UL_SEARCH ult[20];  
	int n;
	void Init() {
		n=0;
	}
	void Store(UL_SEARCH *x) {
		if(n < 20)
			ult[n++].Copie(x);
	}
	// the processing is lenght dependant
	int Traite(int rat) {
		// up to 6 cells basis=4.6 (+0.1)  7_8 cells 4.7 (+0.2)  10_.. 4.8 (+0.3)
		//EE.E("recherche UL rating =");EE.E(rat);EE.E(" n=");EE.Enl(n);
		for(int i = 0; i < n; i++) {
			int len = ult[i].line_count, irat, rbase = 46;	
			if(len > 7)
				rbase++;
			if(len > 9)
				rbase += 3;//  ?? what is the rule  ??
			// now the relative difficulty for loop_ok nand UR common processing
			irat = rat - rbase + 1;
			if(irat < 1 || irat > 4)
				continue;		
			if(ult[i].Loop_OK(irat))
				return 1; 
		}
		return 0;
	}
};


// class defined to handle Unique rectangles and Unique loops
// the search for URs is started in TCELL , locating potential URs
// That class is called by TCELL 

class CRIN {
public:     //on ne traite que deux communs. 
	static CELL *ta, *tr; // ta action, tr recherche 
	static OBBIEL * tchel;
	BF16 wc, wou, wr;   
	int ia, ib, ic, id, deux[4], plus[9], pp1, pp2, // voir pourquoi plus est 9
		ndeux, nplus, nwc, ch1, ch2, chc1, chc2, nautres, diag, rating;

	// data specific to common processing UR/UL (need sub routines)
	int th[10], td[10], tnh[10], nth, ntd, nnh, aig_hid;
	BF16 wh, wd, wnh, wnd, wdp;
	BF81 zwel; // to prepare clearing of naked sets

	CRIN();	 // constructor

	// short functions called by the mains ones

	int GetElPlus();
	int IsDiag();
	int Setw() {
		wc = tr[ia].v.cand & tr[ib].v.cand & tr[ic].v.cand & tr[id].v.cand;   
		nwc = wc.QC();
		return nwc;
	}
	int Setwou() {
		wou = tr[ia].v.cand | tr[ib].v.cand | tr[ic].v.cand | tr[id].v.cand;
		wr = wou ^ wc;
		nautres = wr.QC();
		return nautres;
	}

	void CalDeuxd(int i) {
		if((tr[i].v.cand - wc).f)
			plus[nplus++] = i;
		else
			deux[ndeux++] = i;
	}
	void CalcDeux() {
		ndeux = nplus = 0;
		CalDeuxd(ia);
		CalDeuxd(ib);
		CalDeuxd(ic);
		CalDeuxd(id);
		pp1 = plus[0];
		pp2 = plus[1];
	}

	inline int Jum(USHORT el, USHORT ch) {
		if(tchel[el].eld[ch].n == 2)
			return 1;
		return 0;
	}
	int Jumeau(USHORT a,USHORT b,USHORT ch);

	int GetJJDir(USHORT a, USHORT b, USHORT c, USHORT d, int ch) {
		int ok = Jumeau(a, b, ch);
		ok += Jumeau(b, d, ch) << 1;
		ok += Jumeau(c, d, ch) << 2;
		ok += Jumeau(a, c, ch) << 3;
		return ok;
	} 
	int GetJJDir(int ch){ int ok=Jumeau(ia,ib,ch); ok+=Jumeau(ib,id,ch)<<1;
	ok+=Jumeau(ic,id,ch)<<2;ok+=Jumeau(ia,ic,ch)<<3; return ok;}

	void GenCh(){BF16 ww=wr;ch1=ww.First(); ww.Clear(ch1);ch2=ww.First();
	ww=wc;chc1=ww.First(); ww.Clear(chc1);chc2=ww.First();}

	BF81 GetZ(){BF81 zw(ia,ib);zw.Set(ic);zw.Set(id);return zw;} // le BF81 des 4 points

	void ImageRI(char * lib,USHORT a);
	void ImageRI(char * lib);

	// look for pseudo locked set in a unique loop
	int StartECbi(USHORT p1, USHORT p2, BF16 com, int action) {
		pp1 = p1;
		pp2 = p2;
		wc = com;
		chc1 = com.First();
		com.Clear(chc1);
		chc2 = com.First();
		wr = (ta[p1].v.cand | ta[p2].v.cand) - wc;
		nautres = wr.QC();
		return T2(action);
	}

	int T2(USHORT action);
	int T2_el(USHORT el,USHORT action);
	int T2_el_set(USHORT len);// if len =0 get len if len>0 go
	int T2_el_set_nacked(USHORT len);
	int T2_el_set_hidden(USHORT len);
	int T2_el_set_nack_pair();
	//==================================================
	// main subroutines
	int RID(int l1, int l2, int c1, int c2);  // entry : a potential UR to check
	int RID2(int rat);
	int RID3();
	//! TO BE DOCUMENTED !
	// provides a print form of the cells iset to 1  in the field
	// combine rows and column when possible eg : r12c3 r5c678


};

// CRINT is a table storing possible UR type other than 1 for further processing
class CRINT {
public:
	CRIN tur[20];  
	int n;

	void Init() {
		n = 0;
	}
	void Store(CRIN *x) {
		if(n < 20)
			tur[n++] = (*x);
	}
	int Traite(int rat) {
		int irat = rat - 44;
		if(irat < 2 || irat > 4)
			return 0;
		// EE.E("recherche UR rating =");EE.E(rat);EE.E(" pour irat=");EE.Enl(irat);
		for(int i = 0; i < n; i++)
			if( tur[i].T2(irat) == 1) {
				tur[i].ImageRI(" action from object");
				return 1;
			}	
			return 0;
	}
};
