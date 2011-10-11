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

/* that file contains classes for the general management of the grid

GG   short class to use the grid in 81 or 9*9 mode

I81  to be switched in divf some general function to improve code efficiency

CELL_FIX describing the fix date of a cell
TCELL_FIX the table of the fix data for the 81 cells and associated functions
DIVF fix data for the regions including the region to cell indexes and BF81 pattern

UNP  describing the variable part for a cell 
P81  describing a cell including variable part and a pointer to fix part
TP81 the table of 81 P81 and associated function

    and a group fo classes giving alternative entries in the grid
	region -> digit -> cell   region -> cell -> digits
OBBI

*/


#pragma once

//! Short class to define and handle a 9x9 or 81 char field 
/**
 *This is the classical storage for a puzzle.<br>
 * Empty cell should be coded with '0' to have correct count with 
 * <code>NBlancs</code> and <code>Nfix</code>.
 */

#include <string.h>

#include "skfrtype.h"
#include "bitfields.h"
//#include "ratingengine.h"


// dummy class to have optimized functions for most common index correspondances
// pos(i,j) -> 9*i+j
// Mod3(i) -> i%3
// Div3(i) -> i+3
// Div9(i) -> i/9
// Boite(i,j) -> box index 0;9 for row i, column j
// PosBoite(i,j) -> box relative position 0;9 same row;column

class I81 {
public:
	static inline USHORT Pos(int i, int j) {
		return ((i << 3) + i + j);
	}
	static inline USHORT Mod3(int i) {
		if(i < 3)
			return i;
		if(i < 6)
			return i - 3;
		return i - 6;
	}
	static inline USHORT Div3(int i) {if(i<3) return 0;  if(i<6) return 1;  return 2;}
//	static inline USHORT Div9(int i)      {return i/9;     }
	static inline USHORT Boite(int i,int j)
	{int jj=Div3(j); if(i<3) return jj; if(i<6) return (3+jj); else return (6+jj);}
	static inline USHORT PosBoite(int i,int j) { return 3*Mod3(i)+Mod3(j);}
	// similar function in group mode (54), 
	// but no use of the 81 cells function authorised 

	static inline USHORT Box54(USHORT x) {USHORT a=3*(x/9),b=x%3; return (a+b);}
}; //mi81

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
	int ObjCommun(CELL_FIX* p8) {
		return((el == p8->el) || (pl == p8->pl) || eb == p8->eb);
	}

	int BugParite(int ch);  // utilise aztob
};

//! Table containing the fix data for the 81 cells
class TP81F {  
public:
	CELL_FIX t81f[81]; 

	// constructor making all initialisations 
	TP81F() {
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

	DIVF();   // constructor making initialisations
	
	//! Are all cells defined by <code>ze</code> in House with index <code>i</code>
	/** \return 1 if yes, 0 if no */
	int IsObjetI (const BF81 & ze, int i);

	//! Is there a house that contains all cells defined by <code>ze</code>
	/** \return 1 if there is one, 0 if none */
	int IsObjet(BF81 &ze);
     
	//! Get index of a box that contains all cells defined by  <code>ze</code>
	/** \return box index (18-26) or 0 if none */
	int IsBox(BF81 &ze); 
  
	//! Is there a house that contains the two cells
	/**
	 * \param p1 first cell index
	 * \param p2 second cell index
	 * \return 1 if yes, 0 if no
	 */
	int IsObjet(USHORT p1,USHORT p2); 
  
	///\brief Are the cells defined by <code>ze</code> in an other house
	/// than <code>obje</code> house.
	///\param objs int reference to return the house index
	///\return 1 if an other house has been found, 0 if none
	int IsAutreObjet(BF81 &ze,int obje, int &objs);
	
	//! Get valued cell count in the house <code>el</code>
	int N_Fixes(char * pg,int el);
    
}; // DIVF

