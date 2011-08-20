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
#include "skfrtype.h"

//!Input file containing a list of puzzle to rate.
/**
 * By default default puz.txt in the current directory.
 */
class FINPUT: private ifstream
{
	USHORT 
		//! Indicate if input file is open 1=open, 0=not open.
		open, 
		//! Indicate if end of file has been found 1=EOF, 0=not EOF.
		endf; 
public:
	//!Input file name (without file type).
	static char * namex; 
	FINPUT();
	//! Set the name of the input file.
	/**
	 * A suffix .txt will be added to this name.
	 * This method should be used before opening the file.
	 */
	void SetName(char * nn);

	//! Open the input file
	/**
	 * Prior you shoud set the input file name if you don't use the default one.
	 * \return 0 if opened, 1 if an error occured.
	 * \sa FINPUT::SetName
	 */
	int OpenFI();
	
	//! Close the input file if it not already closed.
	void CloseFI();
	
	//! Get the next puzzle from the input file and normalize it.
	/*
	 * A normalized puzzle is a 81 char string with '1'-'9' for the givens and '.' for the
	 * empty cells. <br>
	 * This method will stop the reading of the input file if it encounters a too long line 
	 * (more than 250). <br>
	 * It skips lines that are too short (81 char minimum). 
	 * It skip also puzzles that have less than 17 clues.<br>
	 * It normalizes the puzzle, and for that it considers all char 
	 * that are not in the range '1'-'9' as empty cells and converts them to '.'<br>
	 * <b>WARNING :</b> in GG class empty cell are coded as '0' !
	 * \param zp pointer to a buffer 82 char long where the puzzle will be copied.
	 * \return 0 if no read puzzle, 1 if a puzzle has been copied to the buffer.
	 */
	int GetPuzzle(char * zp);  // get next and write on output the puzzle
    
	void GetLogName(char * zn)
	   {strcpy_s(zn,198,namex); 
		int ll=strlen(zn);
        strcpy_s(&zn[ll],30,"_log.txt");
	   }
};



//! Output files for rated and non rated puzzles. 
/**
 * There are 2 output files, one for puzzles that have been rated
 * one for puzzles that cannot be rated (rating too high for the current Version)<br>
 * The names of these output files are derived from the name of input file.<br><br>
 * for rated puzzles inputFileName + "_rat.txt" <br>
 * for non rated puzzles inputFileName + "_N_rat.txt"
 */
class FOUTPUT : public ofstream  // public pour les tests
{
	USHORT 
		//! indicates if file is open 0=not open, 1=open
		open, 
		
		//! indicates if file doesn't need to be closed 1=no need, 0
		endf; //TODO problem of status
public:
	//! Pointer to the input puzzle 
	/** It should be set up before calling PrintErEpEd */
	static char* zpuzx;

	FOUTPUT(){open=0;endf=1;}

	inline void Setzpuz(char * x) {zpuzx=x;}
	//! Open method to call for rated puzzles
	/** \return 0 if opened, 1 in case of error. */
	int OpenFO1();
	
	//! Open method to call for non rated puzzles
	/** \return 0 if opened, 1 in case of error. */
	int OpenFO2(); 

	//! Close this output file
	void CloseFO(){if(endf) return; ofstream::close();endf=1;}
 
	//! Output the current puzzle and its rating
	/**
	 * Current puzzle (string form) should be pointed by <code> zpuz</code> class member
	 * \param er ER rating as an integer (108 will be ouput as 10.8)
	 * \param ep EP rating (highest move until first placement)
	 * \param ed ED rating (first move)
	 */
	void PrintErEpEd(int er,int ep,int ed);
private :
	int OpenFO(char * nam);
    
};

