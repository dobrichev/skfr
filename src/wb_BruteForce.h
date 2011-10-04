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
//class UN_JEU;
#include "wa_puzzle.h"

//! This class provide a brute force solver
/**
 * This solver will stop as soon as 2 solutions have been found.<br><br>
 * Usage of this class is :<ul>
 * <li> create an instance </li>
 * <li> init with the puzzle calling UNPAS::Init </li>
 * <li> for each given call UNPAS::Fixer</li>
 * <li> look for solution(s) calling UNPAS::NsolPas </li>
 * <li> verify the number of solution static member UNPAS::nsol (0,1 or 2)
 * and get the (first found) solution in global variable <code>un_jeu.ggf</code></li>
 * </ul>
 */
class UNPAS 
{
public:   
	static ULONG nsol; 

	UNP tu[81];			//< candidate and status of cells
	GG gg;				//< puzzle as a string
	char * gr;			//< pointer on puzzle string that will evolve to the solution
	BF81 libres;		//< 81 bitfield indicating the empty cell
	USHORT nlibres,		//< number of empty cells
		nactif,
		iactif,
		maxlibres;
	BF16 candactif;

	UNPAS() {gr=gg.pg;}
	UNPAS(UNPAS * old){*this=*old;gr=gg.pg;}

	//! Intialize with the puzzle
	/**
	 * Copy the puzzle in memeber of the class<br>
	 * Consider all cells as free.
	 * Reset number of solution
	 */
	void Init(GG ge)
    {	libres.SetAll_1();
		nlibres=81;
		nsol=0;
		tu[0].Init();	// consider all candidates as possible in all cell
		for(int i=1;i<81;i++)tu[i]=tu[0];
		gg.Copie(ge);    
	}
	//! Give a value to a cell and supress candidates in influence zone
	/**
	 * Doesn't change the status of the cell <code>typ</code>
	 * \param i cell index (0-80)
	 * \param ch digit (0-8)
	 */
	void Fixer(int i,USHORT  ch )
    {
		if(libres.Off(i)) return;  
		libres.Clear(i); 
		nlibres--;
        tu[i].Fixer(ch); // give the value to the cell
		Clear(i,ch);	
		gr[i]=ch+'1';
	}

	//! Supress all candidates corresponding to digit in influence zone of the cell
	/**
	 * uses <code>t81f</code> global variable
	 * \param i8 cell index (0-80)
	 * \param ch digit (0-8)
	 */
	void Clear(int i8,USHORT  ch );

	//! Look for Singles (and do them) and blocking situation
	/**
	 * Loop on {look for "single" and do them} until there is no more single. Singles include :<ul>
	 * <li> naked single (priority)</li>
	 * <li> single in house (row/col/box </li>
	 * </ul>
	 * At each loop verify that there is no blocking situation <ul>
	 * <li> a cell with no possibilities </li>
	 * <li> a house where a digit has no possible position</li></ul>
	 * \return 1 if a blocking has been found(one empty cell has no candidate
	 * or a digit has no position in a house), else 0. This return status is 
	 * independent of the number of single found.
	 */
	int Avance(); // tout ce que l'on peut faire
	
	//! Brute force recursive method to find solution and to know if number of solutions is >1
	/**
	 * This method try first to find all single path. Then look for a cell 
	 * with a minimum number of candidates. Assign it and call itself recursively.
	 * Stop if blocking, if more than 1 solution, or if tries are exhausted
	 * Update static member <code>UNPAS::nsol</code>
	 * Update global variable <code>un_jeu.ggf</code> with the first found solution if any
	 */
	void NsolPas(); //id et lct pas suivant

	//! Get a bitfield representing the givens
	/**
	 * <b>NOTA :</b>This method is not used in the SEclone !
	 */
	BF81 GetZ() {
		BF81 zw;
		for(int i = 0; i < 81; i++) 
			if(gr[i] - '0')
				zw.Set(i);
        return zw;
	}

};


//!A class to check uniqueness
/**
 * That was a class from "Gerard Penet" to handle recursive tasks as <ul>
 * <li> puzzles generators</li>
 * <li> brute force solving ...</li></ul>
 * This class has been reduced here to a minimal form for uniquenss verification
 */

class UN_JEU {
public: 
	UNPAS dep;		//< to invoke brute force solver
	GG gg,			//< puzzle initial string
		ggf;		//< solution as a string
	char *gn;		//< pointer to puzzle string
	
	// constructor
	UN_JEU() {
		gn = gg.pg;
	}
	//! get number of solutions (0,1 or 2)
	/**
	 * Use the brute force method of UNPAS class to verify the
	 * uniqueness of puzzle solution
	 * \return 0,1 or 2 (2 means that the puzzle has several solutions)
	 */
	long Analyse(GG & ge) {
		gg.Copie(ge);
		dep.Init(gg);
        for(int i = 0; i < 81; i++) {
			char c = gg.pg[i];
			if(c > '0' && c <= '9')
				dep.Fixer(i, c - '1');
		}
        dep.NsolPas();
		return dep.nsol;
	}
    
	//! has this puzzle one and only one solution
	int  Unicite(GG & ge) {
		return(Analyse(ge) == 1);
	}
};