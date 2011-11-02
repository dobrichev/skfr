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

// _03c_puzzle_fix.h start

#include <string.h>

#include "skfrtype.h"
#include "bitfields.h"
#include "ratEnum.h"			// enum list of solving techniques
#include "bitfields.h"			// bitfields are grouped in this file

extern unsigned long long fsss(const char* in, const unsigned long long maxSolutions, char* out); //fast simple sudoku solver.


//! Short class to define and handle a 9x9 or 81 char field 
/**
 *This is the classical storage for a puzzle.<br>
 * Empty cell should be coded with '0' to have correct count with 
 * <code>NBlancs</code> and <code>Nfix</code>.
 */


// dummy class to have optimized functions for most common index correspondances
// Boite(i,j) -> box index 0;9 for row i, column j
// PosBoite(i,j) -> box relative position 0;9 same row;column

class I81 {
public:
	static inline USHORT Pos(int i, int j) {
		return ((i << 3) + i + j);
	}


	static inline USHORT Boite(int i,int j)
	{int jj=j/3; if(i<3) return jj; if(i<6) return (3+jj); else return (6+jj);}
	static inline USHORT PosBoite(int i,int j) { return 3*(i%3)+j%3;}
	// similar function in group mode (54), 
	// but no use of the 81 cells function authorised 

	static inline USHORT Box54(USHORT x) {USHORT a=3*(x/9),b=x%3; return (a+b);}
}; //mi81


class PUZZLE;

//!Small class containing the permanent data for a cell
class CELL_FIX {
public:
	USHORT 
		i8,		///< cell index (0-80)
		el,     ///< row index (0-8)
		pl,     ///< column index (0-8)
		eb,     ///< box index(0-8) 
		pb,     ///< relative position in the box (0-8)
		pi[20]; ///< list of the  20 cells controled by a cell in an array of cell index
	char pt[5];	///< printing string like R4C9 with \0
	BF81 z;     ///< list of the  20 cells controled by a cell in a bit field

	//! Initialize all member values from the cell index, except control area
	/** \param ie cell index */
	void init(USHORT ie)
	{
		i8=ie; el=ie/9;
		pl=ie-9*el;
		eb=I81::Boite(el,pl);
		pb=I81::PosBoite(el,pl);
		strcpy_s(pt,5,"r c "); pt[1]=(char)(el+'1'); pt[3]=(char)(pl+'1');
	}

	//! Initialize <code>pi</code> array from bitfield <code>z</code>
	void Initpi()  
	{
		int it=0;
		for (int i=0;i<81;i++) 
			if(z.On(i))  
				pi[it++]=i; 
	}  

	//! is the cell <code>p8</code> visible from <code>this</code>
	int ObjCommun(const CELL_FIX *p8) const {
		return((el == p8->el) || (pl == p8->pl) || eb == p8->eb);
	}

	//int BugParite(int ch);  // utilise aztob
};

//! Table containing the fix data for the 81 cells
class CELLS_FIX {  
public:
	CELL_FIX t81f[81];

	// constructor making all initialisations 
	CELLS_FIX() {
		int i, j; 
		for(i=0;i<81;i++)		// initialize all data except influence zone
			t81f[i].init(i);
		for(i=0;i<81;i++) {		// initialize influence zone
			BF81 z;  
			for(j=0;j<81;j++) 
				if((i-j)&& t81f[i].ObjCommun(&t81f[j]))
					z.Set(j);
			t81f[i].z=z;	 
			t81f[i].Initpi();	// initialize array from bitfield
		}
	}

	int GetLigCol (USHORT p1,USHORT p2)  // only if is lig or col (UR)
	{
		if(t81f[p1].el==t81f[p2].el) 
			return t81f[p1].el;
		if(t81f[p1].pl==t81f[p2].pl)
			return (t81f[p1].pl+9);
		return -1;
	}
};

//! A class to represent houses and that provides basic method on them.
/**
 * A house is a row (index 0 to 8) a column (index 9 to 17) or a box (18 to 26).
 * This class gives the list of cells for each house as an array of cell index 
 * or as a bitfield
 */
class DIVF {
public:
	USHORT el81[27][9]; ///< House (0-26) to 9 cells (0-80) as array of cell indexes
	BF81 elz81[27];    ///< House (0-26) to 9 cells (0-80) as a bit field

	//DIVF();   // constructor making initialisations
	DIVF() {  // constructor making initialisations
		for(int r = 0; r < 9; r++) {
			for(int c = 0; c < 9; c++) {	// loop on cells
				int p = I81::Pos(r, c); // cell index (0-80)
				el81[r][c] = p;		// cell is in row r
				el81[c + 9][r] = p;		// cell is in column c
				int eb = I81::Boite(r, c), pb = I81::PosBoite(r, c); 
				el81[eb + 18][pb] = p;	// cell is in box eb and position in box pb
			}
		}
		for(int i = 0; i < 27; i++) {	// convert array to bitfield
			//BF81 z;   
			//for(int j = 0; j < 9; j++) 
			//	z.Set(el81[i][j]);  
			//elz81[i] = z;
			for(int j = 0; j < 9; j++) 
				elz81[i].Set(el81[i][j]);  
		}
	}
	
	//! Are all cells defined by <code>ze</code> in House with index <code>i</code>
	/** \return 1 if yes, 0 if no */
	//int IsObjetI (const BF81 & ze, int i) const;
	int IsObjetI(BF81 const &ze, int i) const {
		return (ze.EstDans(elz81[i]));
	}

	//! Is there a house that contains all cells defined by <code>ze</code>
	/** \return 1 if there is one, 0 if none */
	//int IsObjet(BF81 &ze) const;
	int IsObjet(const BF81 &ze) const {
		for(int i = 0; i < 27; i++)
			if(IsObjetI(ze, i)) 
				return 1;  
		return 0;
	}
	//! Get index of a box that contains all cells defined by  <code>ze</code>
	/** \return box index (18-26) or 0 if none */
	//int IsBox(BF81 &ze) const; 
	int IsBox(const BF81 &ze) const {
		for(int i = 18; i < 27; i++) 
			if(IsObjetI(ze, i))
				return i; 
		return 0;
	} 
	//! Is there a house that contains the two cells
	/**
	 * \param p1 first cell index
	 * \param p2 second cell index
	 * \return 1 if yes, 0 if no
	 */
	//int IsObjet(USHORT p1,USHORT p2) const; 
	int IsObjet(USHORT p1, USHORT p2) const {
		BF81 z(p1, p2);
		return IsObjet(z);
	}
	///\brief Are the cells defined by <code>ze</code> in an other house
	/// than <code>obje</code> house.
	///\param objs int reference to return the house index
	///\return 1 if an other house has been found, 0 if none
	//int IsAutreObjet(BF81 &ze,int obje, int &objs) const;
	int IsAutreObjet(const BF81 &ze, int obje, int &objs) const {
		for(int i = 0; i < 27; i++) {
			if(i == obje)
				continue;
			if(IsObjetI(ze, i)) {
				objs=i;
				return 1; 
			}
		}
		return 0;
	}
	//! Get valued cell count in the house <code>el</code>
	//int N_Fixes(char * pg,int el) const;
	int DIVF::N_Fixes(const char * pg, int el) const {
		int n = 0; 
		for(int i = 0; i < 9; i++) 
			if(pg[el81[el][i]] - '0') 
				n++;
		return n;
	}
}; // DIVF
// DIVF implementation

// _03c_puzzle_fix.h end

class GG {
public:
	// a grid with clues (givens)
	char g [9][9],	///<The grid
		filler,		///<a null to terminate 81 char string
		*pg;		///<a pointer to the beginning of the string

	GG();		// constructor

	//inline char * Getpg() const {
	//	return pg;
	//}
	//! Copy the grid value
	inline void Copie(const GG & ge) {
		strcpy_s(pg, 82, ge.pg);
	} // copie 81 caractères
	
	inline void operator =(const GG &ge) {
		strcpy_s(pg,82,ge.pg);
	}
	//! Get the empty cell count
	int NBlancs() const;
	  
	//! Get the valued cell count
	int Nfix() const;
	
	//! Print on output file a representation 9x9 of the grid
	/**
	 * Print on a first line the string passed as parameter. Print 9 more lines
	 * representing the grid with line numbers.
	 */
	void Image(char * lib) const;
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
	USHORT GetCand() const {
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

class CELLS { 
public:
	PUZZLE * parentpuz;
	FLOG * EE;
	CELL t81[81];       

	void SetParent(PUZZLE * parent,FLOG * xx);
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
 * for one cell candidates and number of candidates see REGION_CELL::Genpo
 */
class REGION_CELL {
public:
	BF16 b;  //< bitfield for 9 positions
	USHORT n; //< number of postions
	//! Set everything to 0
	void Raz() {
		b.f = 0;
		n = 0;
	}
	//! Set the position <code>i</code> and increment number
	void Set(int i) {
		b.Set(i);
		n++;
	}
	//! Generate from CELL _VAR <code>p8</code>
	void Genpo(const CELL_VAR &p8) {
		n = p8.ncand;
		b = p8.cand;
	}
};

//! vector for 9 cells or 9 digits  
class REGION_INDEX {
public: 
	REGION_CELL eld [9];
};

//! grouping the 27 regions : rows/cols/boxes
class ONE_REGIONS_INDEX {
public: 
	REGION_INDEX el[27];  //< possible position of each digit for each region
};

//! Main class giving regional alternative index or candidates
/**
 * There is a unique instance of this class 
 * Positions are updated once per cycle
 */
class TWO_REGIONS_INDEX {
public: 
	ONE_REGIONS_INDEX tpobit;  // entry region -> position -> digit
	ONE_REGIONS_INDEX tchbit;  // entry region -> digit -> position

    void Genere();  // update
};




//That class is dedicated to the processing of algorithms building 
//  a progressive collection of cells
// looking for locked sets
// looking for fishes.

// such algorithms are using  recursive routines 

class SEARCH_LS_FISH 
{
public:
	PUZZLE * parentpuz;
	FLOG * EE;
	REGION_CELL * eld;
	REGION_INDEX * regindp,* regindch,* regindchcol,*regxw;

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

	UCHAR ch; 

    void SetParent(PUZZLE * parent, FLOG * xx);

	void InitTir(BF16 none,BF16 casese, int rangc,int rangv);

	void InitXW();

	int Tiroirs(int nn,int hidden,int sing);

	int Tiroir(BF16 fd,int iold,int irang) ;

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
	void Charge(const CELL & p8) {
		pa = p8.v.cand;
		i8 = p8.f->i8;
	}
};


class TPAIRES {
public: 
    PUZZLE * parentpuz;
	PAIRES zp[80], zpt[80]; // collection of bivalues in cells
	int izpd[50];           // start/end index in zp for tp table
	BF16 tp[50],            // collection of different possibilities for bivalues in cells
		el_par_ch[27],    // parity fo digits in pairs in any elements
		el_par2_ch[27],   // parity copie for final check
		candp_or,         // within an elem parity or for paired cells
		candnp_or;        // same for non paired cells
	//BF16 candp_xor;        // parity for paired (redundancy)
	//BF16 candnp_xor;       // parity for non paired cells
	//BF16 wplus[10];        // BF16 within the element for non pair cells (virtual clearing)

	BF81 zplus;             // cells with more than 2
	int tplus[10], ntplus;  // same in table limited to 8
	USHORT tpa[10],         // table for pairs in tha active row/col/bx
		ntpa,            // and number of pairs
		nwp;             // index for wplus
	int ip, np, aigpmax, aigun, brat;

	void SetParent(PUZZLE * parent);
	void CreerTable(const CELL * tt);
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
	void BugMess(const char * lib) const;
	void CommunLib(int i, int j, int k, char * lib);
};


//#include "_03b_puzzle_chains.h"    //  puzzles chain eliminations

//file _03b_puzzle_chains.h start

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
class PATH {
public:
	USHORT pth[100], ipth;
	//USHORT ipth_plus;
	//inline void Init() {
	//	ipth = ipth_plus = 0;
	//}
	//inline void Set(USHORT i, USHORT x) {
	//	pth[i] = x;
	//}
	inline void Add(USHORT x) {
		if(ipth < 50)
			pth[ipth++] = x;
	}
	//void Add(USHORT *pthp, USHORT ipthp) {
	//	for(int i = 0; i < ipthp; i++)
	//		Add(pthp[i]);
	//}

	void PrintPathTags();


};  

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

};   
/* TCHAIN is a support for chain potential eliminations
   each "load call" with a rating < or =  to  already achieved one
      -> immediate elimination
   if not, all "lowest rating" are stored until the end of the process.

   provides also information on the length that would generate to high rating

*/
class TCHAIN { // storing up to 20 chains having the same "lowest"rating
public:      
	PUZZLE *parentpuz;
	FLOG * EE;
	BFCAND cycle_elims;
	USHORT rating,             // current rating 
		   elims_done,        // set to 1 if a clearing is already done
		   achieved_rating,
		maxlength;          // filter for the search of a path (tags)
	CHAIN chainw, chains[30]; // enough to store several small loops 
	USHORT ichain,           // index to chains 
		base;
	void SetParent(PUZZLE * parent,FLOG * fl);

	void NewCycle() ;

	// the following functions relates to performance control in the search of eliminations
	// should be revised in the new process
	// the first function set the default value for the base

	void SetMaxLength(int bw);

	// entry in tchain is either with an explicit base of with the default value

	int GetRatingBase(USHORT bb, USHORT lengthe, USHORT cand);
	int GetRating(USHORT lengthe, USHORT cand) {
		return GetRatingBase(base, lengthe, cand);
	}

	void LoadChain(USHORT rat, char * lib, USHORT cand);

	int IsOK(USHORT x);

	int Clean(); // clear all pending candidates
	int ClearChain(int ichain); // clear candidates
	int ClearImmediate(USHORT cand);

};// tchain ;








#define zgs_lim 10000
class ZGROUPE {    // en fixe 81 points 8       puis  54 groupes
public:
	BF81 z[zgs_lim];
	PUZZLE *parentpuz;
	USHORT iz;
	ZGROUPE(PUZZLE *parent); 
	void ReInit() {
		iz = 81 + 54;
	} // first after compulsory entries
	//inline BF81 getz(int i) {
	//	return z[i];
	//}
	USHORT Charge(const BF81 & z);
};



class CANDIDATE {
public:  
	USHORT   ch,  // digit
		ig;  // cell id

	inline void Charge(USHORT i,USHORT che) {ig=i;ch=che;}
	//inline BF81 * GetZ() const;     
	void Image(int no=0) const; // no=1 if false state
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
	void PrintPath(USHORT * tp,USHORT np) const;
	void PrintImply(const USHORT * tp,USHORT np) const;
	void PrintListe(USHORT * t,USHORT n,int modetag=0) const;

	void Image(int i,int no=0) const {zp[i].Image(no);  }
	void ImageTag(int i) const {Image(i>>1,i&1);}
	void Liste() const;        // for debugging only
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
void AllParents(const SQUARE_BFTAG & from);
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

//int Plus(int m1, int m2) {
//	if(!t[m1].On(m2)) {
//		t[m1].Set(m2);
//		return 1;
//	}
//	else
//		return 0;
//}
void Plus(int m1, int m2) {
	t[m1].Set(m2);
}

//int Entre(int m1, int m2) {
//	return (Plus(m1,m2^1) + Plus(m2,m1^1));
//}
//void Entre(int m1, int m2) {
//	Plus(m1,m2^1);
//	Plus(m2,m1^1);
//}

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

	PHASE h, hstart, h_one, h_nest,storehw;

	SQUARE_BFTAG hdp_base,         // must contain only basic strong and weak links
		         hdp_dynamic,      // same plus direct events effects
				 hdp_base_nested;  // hdp_base plus nested strong links

	USHORT load_done;

	BFTAG  xb, xi, xbr, xbr2; 
	BFCAND isbival;

	INFERENCES(PUZZLE * parent){parentpuz=parent;}

	//inline BFTAG * Getd(int m) {
	//	return & h.d.t[m];
	//}
	//inline BFTAG * Getdp(int m) {
	//	return & h.dp.t[m];
	//}
	//inline int IsConflitD(int m1, int m2) {
	//	if(m1 == (m2 ^ 1))
	//		return 1;
	//	return h.dp.IsConflit(m1, m2);
	//}
	//inline int IsConflit(int m1, int m2) {
	//	if(m1 == (m2 ^ 1))
	//		return 1;
	//	return h.d.IsConflit(m1, m2);
	//}
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
	//void NewPhase() {
	//	h.d.ExpandAll(h.dp);
	//}
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
	void Aic_Ycycle(USHORT t1, USHORT t2, const BFTAG &loop, USHORT cand);
	void Aic_YcycleD(USHORT t1, USHORT t2, const BFTAG &loop, USHORT cand);
	void Aic_YcycleD2(USHORT t1, USHORT t2, const BFTAG &loop, USHORT cand);
	int Aic_Ycycle_start(USHORT t1, USHORT t1a, USHORT t2, const BFTAG &loop, PATH &result);

	void ExplainPath(BFTAG &forward, int start, int send, int npas, USHORT relay);
	int Fast_Aic_Chain();// quick eliminations high rating reached

	//int Nested_QuickForcing(SQUARE_BFTAG &xt, BFTAG &elims); 
	//int Nested_QuickMulti(SQUARE_BFTAG &xt, BFTAG &elims); 
	//int Nested_QuickDynamic(SQUARE_BFTAG &xt,BFTAG &elims) ; 


private:
	//int Plusp(int m1, int m2) {
	//	return h.dp.Plus(m1, m2);
	//}
	void Plusp(int m1, int m2) {
		h.dp.Plus(m1, m2);
	}
	//int Entrep(int m1, int m2) {
	//	return (Plusp(m1, m2 ^ 1) + Plusp(m2, m1 ^ 1));
	//}
	void Entrep(int m1, int m2) {
		Plusp(m1, m2 ^ 1);
		Plusp(m2, m1 ^ 1);
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

 //void GetCand(BFCAND & ms)// init à la charge de l'appelant
 //          {for(int i=0;i<ncd;i++) ms.Set(tcd[i]);}

 //inline int ChoixBase(){return (!type);}
 //void PrintInterditSimple(USHORT mi,USHORT derive){}//;
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
	//int CopySet(int ie);

	//void DeriveDirect() {
	//	direct = 1;
	//	Derive(3, 4, 3);
	//	direct=0;
	//}
	void Derive(int min, int max, int maxs); 
	void DeriveBase(const SET & chx);
	void DeriveSet(SET & chx);

	int Interdit_Base80() ;	 

	//int CheckGoNested1(const BFTAG &bftag, USHORT cand);

	void Image();

}; //zcx ;  

//file _03b_puzzle_chains.h end

#define pasmax 70

class PUZZLE
{
public:

    SEARCH_LS_FISH yt;
    TCHAIN tchain;
    TWO_REGIONS_INDEX alt_index; 


    GG gg,          //< copy of the puzzle (normalized form - empty cell '0')
       gsolution;   // final result if valid (one solution)
    CELLS tp8N,
		tp8N_cop;  
	BF81 
		c[9],
		c_cop[9];	// grille globale et par chiffre
    BF81 zactif;
	//BF81 	elza81[27],
	//BF81 	csol[9];   //< Solution as positions of the 9 digits

    //long tdebut; // for debugging purpose start time in PUZZLE traite base

    //int coup;
	//int coupMM,
	int couprem; 
    char fix[81];
	char fixt[81],
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


        // data to control the dynamic and nested mode   

	USHORT nested_aig,
		   rbase;
	//USHORT ret_code;
	USHORT npas,
		   nested_print_option;
	int opp;
	short tsets[640];
	USHORT tx[pasmax][700],itx[pasmax];
	USHORT tret[500],pasret[500],itret;
	BFTAG steps[pasmax],cumsteps[pasmax],allsteps, * to;  
	BFTAG  *cum  ,* step; 
	USHORT *ta,ita;//,*tb,itb;
	SQUARE_BFTAG dpn,dn; // in nested mode, dynamic set of primary links
	
	      // added here control for only one search
	      // bit set to 1 when dynamic search done
	      // form1 form 2 indes is candidate
	      // sets index is ""set index"" in ""sets table""
	             
	BFTAG  dynamic_form1,        // form a=> x and  a=>~x		      
		   dynamic_form2[320],   // form x=> ~a and  ~x => ~a
	       dynamic_sets[320];   // maxi is 384 sets less given 

	// set fo data for nested dynamic level 4
	// similar to standard but no "plus" set
	// reworked to use one buffer to store sets

	short chain4_tsets[640];
	USHORT chain4_buf[640],
		   *chain4_tx[pasmax],  // pointers to chain4_buf
		   chain4_itx[pasmax];
	USHORT chain4_result[500],chain4_iresult;
	BFTAG chain4_steps[pasmax],chain4_cumsteps[pasmax],chain4_allsteps, * chain4_to;  
	BFTAG  *chain4_cum  ,* chain4_step, chain4_bf; 
	USHORT *chain4_ta,chain4_ita,
		   chain4_npas,nested_aig2; 


	PUZZLE();
	 

    /* set of routines previously in opsudo */

		//! filter on ED 
	int is_ed_ep();   // at the start of a new cycle
	
	int Is_ed_ep_go();

	void Step(SolvingTechnique dif)  ;  // analyse what to do at that level

	inline void SetDif(USHORT dif){difficulty=dif;} // sub level inside a process

	void SetEr();   // something found at the last difficulty level  

	void Seterr(int x);  // an error condition has been found
    





    //void Initial(); 
	void cInit(int un=0);
	void Copie_T_c();
    void cFixer(int ich,int i8);
    void cReport();   
    //void TReport();   
    void Actifs();
    int CheckChange(int i, int ch);
    int ChangeSauf(int elem,BF16 pos,BF16 chiffres );
    int Keep(int elem,BF16 pos,BF16 chiffres );
    //int Keep(int ch, const BF81 &zk); // éliminer ailleurs en objets contenant
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
    //int Check();
    int Recale();              
	int Traite(char * ze);
	int Traite_a();
	//int Traite_b();
	//int Traite_c();
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
	}
    void PointK();
    //void UsePK(USHORT i);
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
	void Rating_Nested( USHORT * ttags, USHORT ntags, USHORT target);

	void ChainPlus(); // formerly in INFERENCES

	
	void ImagePoints(BF81 & zz) const;  
	void ImageCand(BFCAND & zz ,char * lib) const;
	void GetCells(BFCAND & zz,BF81 & cells) const;
	void Image(const BFTAG & zz,char * lib, int mmd) const;
 	void Elimite(char * lib);
	void Estop(char * lib);


/* redefine here for translation all routines
   formerly in CANDGO 
   */

	int GoBack(USHORT tag,int pr);  // compute the length for one chain 
	int GoBackNested(USHORT tag);  // compute the length for one chain 
	void GoNestedTag(USHORT tag);  // get expanded situation 
	int GoNestedCase1(USHORT cand);  // locate the contradiction with case 1
	int GoNestedCase2_3(     // locate the contradiction  
		USHORT tag,USHORT target);  // case 2 or case 3

	int CaseNestedLevel4( USHORT tag,USHORT target);  // dynamic 
	void NestedChainWhile(USHORT tag);
	int NestedChainGoBack(USHORT tag);  // compute the length for a nested dynamic chain 

	void GoNestedWhile(USHORT tag);
	void GoNestedWhileShort(USHORT tag);
	void Gen_dpn(USHORT tag);
	void Gen_dpnShort(USHORT tag);
	void NestedForcing(BFTAG & elims);
	void NestedForcingShort(BFTAG & elims);
	void NestedMulti(BFTAG & elims);
	void NestedMultiShort(BFTAG & elims);
	void NestedForcingLevel4(BFTAG & elims);
	void NestedForcingShortLevel4(BFTAG & elims);
	void NestedMultiLevel4(BFTAG & elims);
	void NestedMultiShortLevel4(BFTAG & elims);



private:
	int FaitGo(int i8,char c1,char c2);
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
	void Copie(UL_SEARCH &x) {
		(*this) = x;
	}
	void Set(int i8);  //a new cell is added to the search
	int Add_Chain(int i8);
	int Loop_OK(int action = 0); 
	//int Valid_Set(CELL_FIX &f, char c) {
	//	int el = f.el;
	//	if(c == 'c')
	//		el = f.pl + 9;
	//	else if(c == 'b')
	//		el = f.eb + 18;
	//	if(el_used.Off(el))
	//		return el; 
	//	return -1;
	//}
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
	void Store(UL_SEARCH &x) {
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

class SEARCH_UR {
public:     //on ne traite que deux communs. 
	static CELL *ta, *tr; // ta action, tr recherche 
	static REGION_INDEX * tchel;
	BF16 wc, wou, wr;   
	int ia, ib, ic, id, deux[4], plus[9], pp1, pp2, // voir pourquoi plus est 9
		ndeux, nplus, nwc, ch1, ch2, chc1, chc2, nautres, diag, rating;

	// data specific to common processing UR/UL (need sub routines)
	int th[10], td[10], tnh[10], nth, ntd, nnh, aig_hid;
	BF16 wh, wd, wnh, wnd, wdp;
	BF81 zwel; // to prepare clearing of naked sets

	SEARCH_UR();	 // constructor

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

// SEARCH_URT is a table storing possible UR type other than 1 for further processing
class SEARCH_URT {
public:
	SEARCH_UR tur[20];  
	int n;

	void Init() {
		n = 0;
	}
	void Store(SEARCH_UR *x) {
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

// file _20a_event.h start

/* a small class to prepare data to collect for a new event
   each member contains a list of candidates in both forms, table and bit field
*/
class EVENTLOT {
public:
	//BFCAND bf;            // the set in bit field form
	USHORT tcd[30], itcd;  // list and current maxindex
	EVENTLOT() { // constructor
		itcd = 0;
	}
	EVENTLOT(BF81 &x, USHORT ch);  // constructor equivalent to a BF16 pattern for ch
	void AddCand(USHORT cell, USHORT ch);
	int GenForTag(USHORT cand, WL_TYPE type) const;
	void Image() const;
};


/* class to store an potential event waiting for candidates establishing the event.
*/

enum EVENT_TYPE {
	evlockrc,
	evlockbox,
	evpairhidden,
	evpairnacked,
	evxwrow,
	evxwcol
};

class EVENT {
public:
	EVENTLOT evl;   // the list of candidates false (out of the set) if 'true'

	USHORT tcand[4], // to store the candidates of the pattern 
		ntcand,   // number of candidates in the pattern
		tag;  // the tag value (index + tpat_vi)
	EVENT_TYPE type; // the pattern identification
	void Load(USHORT tage, EVENT_TYPE evt, const EVENTLOT & evb, const EVENTLOT & evx);
	int IsPattern (USHORT cand) const {
		for(int i = 0; i < ntcand; i++)
			if(cand == tcand[i])
				return 1;
		return 0;
	}
	void Image() const;
};

/* table of events 
*/

#define event_size 10000
extern USHORT event_vi;

class TEVENT {
public:
	PUZZLE * parentpuz;
	EVENT t[event_size];
	USHORT it;             // next value in t

	TEVENT(PUZZLE * parent){parentpuz=parent;}

	inline void Init() {
		it = 1;
	} // keep 0 for return false and temp PAT_TAG

	void LoadXW();
	void LoadXWD(USHORT ch, USHORT el1, USHORT el2, USHORT p1, USHORT p2, EVENTLOT &eva, EVENTLOT &evx);
	void LoadPairs();
	void LoadPairsD(USHORT cell1, USHORT cell2, USHORT el);
	void PairHidSet(USHORT cell1, USHORT cell2, USHORT el, BF16 com, EVENTLOT & hid);
	void LoadLock();

	void LoadAll() {
		Init();
		LoadLock();
		LoadPairs();
		LoadXW();
		//LoadFin();
	}
	void EventBuild(EVENT_TYPE evt, EVENTLOT & eva, EVENTLOT & evb, EVENTLOT & evx);

	int EventSeenFor(USHORT tagfrom, USHORT tagevent) const;
	void LoadFin(); // only for debugging purpose
}; //tevent;

// file _20a_event.h end

//file _30a_TCANDGO.h start

/* In nested mode, the track back process is not so easy.
   To achieve it relatively simply, we use a side table filled in the forward process.
   for each new strong link found we open a new entry
   for each new generation  in the forward process, we open a new entry
   reached thru a direct index
   but the new entry is done in advance if a side exclusion is found
   each entry contains
      a BFTAG containing all tags used to come to a decision
      the count if the source is a nested rule.
	  the index in CHAINSTORE to find back the chain at print time

*/

class CANDGOFORWARD { // one entry per step.
public:
	BFTAG source;
	USHORT count, index;
	void Add(USHORT ind, const BFTAG &bf, USHORT cpt) {
		index = ind;
		source = bf;
		count = cpt;
	}
};
class CANDGOSTRONG { // no need to sort the key, entry is doubled
public:
	BFTAG source;
	USHORT count,key1,key2;
	void Add(USHORT k1, USHORT k2, const BFTAG &bf, USHORT cpt) {
		key1 = k1;
		key2 = k2;
		source = bf;
		count = cpt;
	}
};

class TCANDGO {
public:
	CANDGOFORWARD tt[5000];
	CANDGOSTRONG ts[1000];
	USHORT itt, its;
	void Init() {
		itt = its = 1;
	} // keep 0 as invalid return
	void AddStrong(USHORT k1, USHORT k2, const BFTAG &bf, USHORT cpt) {
		if(its >= 598)
			return;
		ts[its++].Add(k1, k2, bf, cpt);
		ts[its++].Add(k2, k1, bf, cpt);
	}
	const CANDGOSTRONG * Get(USHORT t1, USHORT t2) const {
		USHORT c1 = t1 >> 1, c2 = t2 >> 1;
		for(int i = 1; i < its; i++) {
			const CANDGOSTRONG * w = &ts[i];
			if(w->key1 - c1)
				continue;
			if(w->key2 - c2)
				continue;
			return w;
		}
		return 0;
	} 

};//tcandgo;

/* storage of the nested chains for pring purpose
  storing is done in a buffer with a double index
  primary for a chain (start,number=
  secondary for a nested object: start index, end index
  */
  
class  CHAINSTORE {
public:
	USHORT buf[50000],         
		ise,
		s2[1000], e2[1000], ise2;
	int starts[3000], ends[3000],
          ibuf;

	void Init() {
		ibuf=0;
		starts[0] = ends[0] = 0;
		ise = 1; 
		s2[0] = 0;
		e2[0] = 0;
		ise2 = 1;
	} // 0 is "empty"
	USHORT AddChain(USHORT * tch, USHORT n) {
		if(ibuf + n > 50000){
			return 0;
		}
		starts[ise] = ibuf; 
		for(int i = 0; i < n; i++)
			buf[ibuf++] = tch[i];
		ends[ise] = ibuf; 
		if(ise >= 3000)
			return ise; // don't pass the limit
		else
			return ise++;
	}
	USHORT AddOne(USHORT * tch, USHORT n) {
		if(ise2 >= 1000)
			return 0;
		s2[ise2] = e2[ise2] = AddChain(tch, n);
		return ise2++;
	}
	USHORT AddMul(USHORT d, USHORT f) {
		if(ise2 >= 1000)
			return 0;
		s2[ise2] = d;
		e2[ise2] = f;
		return ise2++;
	}
	void Print(USHORT index) const;
}; //tstore;

//file _30a_TCANDGO.h end

// global variables for Puzzle


extern OPSUDO Op;
extern FLOG EE;
extern CELLS T81dep;
extern CELLS *T81;
extern CELLS *T81C;		//standard names for main objects of the class
extern CELL *T81t;
extern CELL *T81tc;		//and corresponding tables of cells  
                    // the corresponding tables are located in class PUZZLE
extern CELLS_FIX tp81f;
extern CELL_FIX *t81f;			//pointer to speed up the process   
extern DIVF divf;
//extern TWO_REGIONS_INDEX aztob; 
extern PUZZLE puz;
extern ULT tult;
extern TPAIRES zpaires;
extern SEARCH_UR ur;
extern SEARCH_URT urt;
extern ZGROUPE zgs;
extern CANDIDATES zpln;
extern INFERENCES zcf;
extern SETS_BUFFER zcxb;
extern SETS zcx;
extern TEVENT tevent;
extern TCANDGO tcandgo;
extern CHAINSTORE tstore;

