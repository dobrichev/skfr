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

/* that file contains classes managing bit fields in the program and a specific class, 
   BF_CONVERT doing specific operations on bit fields to improve the overall performance.

   The specific classes are

   BIT16 a bit field of size 16 bits mainly used as a 9 bits field.
   BIT32 a bit field of size 32 bits used  as a27 bit field for regions
   BIT81 a bit field to describe some "cell" properties in the 81 cells context
   BITCAND a bit field sized to a maximum of 320 bits but worked at the used dimension
           that bit field is used to describe candidates properties.
           the bit entry corresponds to the index in the table of candidates
   BITTAG  a bit field sized to a maximum equal to (BITCAND size * 2)
           that bitfield describe the tags properties
*/

#pragma once
#include "skfrtype.h"

/* bit field functions to improve performances
   cpt gives the count of bit set to "1"
   First gives the place of the first bit
*/
class BF_CONVERT
{
public:
	int tn[256];

	BF_CONVERT() 
	{
		for(int i=0;i<256;i++)
        {
			int n=0; 
			for(int j=0;j<8;j++) 
				if(i&(1<<j)) n++;
                tn[i]=n;
		}
	}
	
	inline int cpt(char x){return tn[x];}

	inline int cpt(USHORT x) {return ( tn[x&255]+ tn[x>>8]);}
	
	inline int cpt(UINT x)
	{
		return ( tn[x&255]+ tn[(x>>8) & 255]
	                     + tn[(x>>16) & 255]+ tn[x>>24]);}
	int cpt(UINT * tx,int nx)
	{
		int n=0;
		for(int i=0;i<nx;i++)
			n+=cpt(tx[i]); 
		return n;
	}

	int First(UINT * tx,int nx)
    {
		int n=0; 
		for(int i=0;i<nx;i++) 
			if(!tx[i]) n+=32; 
		else
		{
			UINT x=tx[i];
			for(int j=0;j<32;j++)
				if(x & (1<<j))
					return (n+j);
		}
		return -1;
	} // -1 is bit field null
};

/* class BIT16 can be used in any case one need a 16 bits field, 
      for example to store options.
   most uses in that program refer in fact to a 9 bits field

   9 bits field can describe many binary facts in the sudoku game,
   for example it can
   describe the digits candidate in a cell
   describe the relative cells occupied by a digit in a row, column, box
   describe the parity of candidates in a row, column, box ...
*/

//! a 9 bitfield to give position of candidate in a house and similar functions
class BF16 {
public:
	//! bitfield
	USHORT f;
	// constructors
	BF16() {
		f = 0;
	}               
	BF16(int i1) {
		f = 1 << i1;
	}
	BF16(int i1, int i2) {
		f = (1 << i1) | (1 << i2);
	}
	BF16(int i1, int i2, int i3) {
		f = (1 << i1) | (1 << i2) | (1 << i3);
	}
	BF16(int i1, int i2, int i3, int i4) {
		f = (1 << i1) | (1 << i2) | (1 << i3) | (1 << i4);
	}
	BF16(int i1, int i2, int i3, int i4, int i5) {
		f = (1 << i1) | (1 << i2) | (1 << i3) | (1 << i4) | (1 << i5);
	}

	//! set all 9 bits to 0
    inline void SetAll_0() {
		f = 0;
	} 
	//! set all 9 bits to 1
	inline void SetAll_1() {
		f = 0x1ff;
	}			
	//! indicate if all bits are 0
    inline int isEmpty() {
		return (f == 0);
	}
	//! indicate if at least one bit is on
    inline int isNotEmpty() {
		return f;
	}
	//! is bit at ch position on
	inline int On(int ch) {
		return ((f & (1 << ch)));
	}		
	//! is bit at ch position off
	inline int Off(int ch) {
		return (!(f & (1 << ch)));
	}	
	//! set bit at ch position
	inline void Set(USHORT ch) {
		f |= (1 << ch);
	}	
	//! reset bit at ch position
	inline void Clear(USHORT ch) {
		f &= ~(1 << ch);
	}
	inline BF16 operator &(BF16 & e) {
		BF16 w;
		w.f = f & e.f;
		return w;
	}
	inline BF16 operator |(BF16 & e) {
		BF16 w;
		w.f = f | e.f;
		return w;
	}
	inline BF16 operator ^(BF16 & e) {
		BF16 w;
		w.f = f ^ e.f;
		return w;
	}
	inline BF16 operator -(BF16 & e) {
		BF16 w;
		w.f = f ^ (e.f & f);
		return w;
	}
	inline int  operator ==(BF16 & e) {
		return(e.f == f);
	}
	inline void operator &=(BF16 & e) {
		f &= e.f;
	}
	inline void operator |=(BF16 & e) {
		f |= e.f;
	}
	inline void operator ^=(BF16 & e) {
		f ^= e.f;
	}
	inline void operator -=(BF16 & e) {
		f ^= (f & e.f);
	}
	/*
	  inline USHORT GetOne() // valable pour un seul en mode très rapide pas de sécurité 
	  { if (f>16) {if(f>64) {if(f&128) return 8; else return 9;}
							else if(f&64) return 7; else return 6;}
			else  if(f<4)return f;//1 ou 2
				   else if(f&4) return 3; else if(f&8) return 4;else return 5;}
	*/
	//! give the first on bit position
	/** \return the position or 9 if there is no bit on */
	int  First() {
		for(int i = 0; i < 9; i++)
			if(On(i))
				return i;
		return 9;
	}
	// optimisable avec un tableau de 512 bytes
	//! get count of on bits
	USHORT QC() {
		USHORT n = 0;
		for(int i = 0 ; i < 9; i++) {
			if(On(i))
				n++;
		}
		return n;
	}
	// optimisable avec un tableau de 512 bytes
	//! is it a pair of position
	int paire() {
		return (QC() == 2);
	}     
	//! get count of on bits and a string of their positions 1-9
	USHORT CountEtString(char *s) {
		USHORT n = 0;
		for (int i = 0 ;i < 9; i++) {
			if(On(i))
				s[n++]=(char)(i+'1');
		}
		s[n] = 0; 
		return n;
	}
	//! get a string representing the positions of on bit. 
	/**
	 * The string contains letters A-I or figures 1-9.
	 * \param lettre if 0 use letter A-I, if 1 use figures 1-9
	 */
	char * String(int lettre = 0) {
		static char ws[10];
		int n = 0;
		for (int i = 0; i < 9; i++) {
			if(On(i))
				ws[n++] = (char)(lettre ? i + 'A' : i + '1');
		}
        ws[n] = 0;
		return ws;
	}
}; // BF16

/* BIT32 is used mainly in that program as a 27 region bit field
   describing some binary properties fo these regions
*/


//! a class for bitfield (limited to 32 bits) and set of functions and operators
class BF32 {
public:
	UINT f;   // bitfield
	BF32() {               // constructor
		f = 0;
	}
	/// is bit at ch position on
	inline int On(int ch) {
		return ((f & (1 << ch)));
	}
	/// is bit at ch position off
	inline int Off(int ch) {
		return (!(f & (1 << ch)));
	}
	/// set bit at ch position
	inline void Set(USHORT ch) {
		f |= (1 << ch);
	}
	/// clear bit at ch position
	inline void Clear(USHORT ch) {
		f &= ~(1 << ch);
	}
	/// invert bit at ch position
	inline void Inv(USHORT ch) {
		f ^= (1 << ch);
	}
	inline BF32 operator -(BF32 & e) {
		BF32 w;
		w.f = f ^ (e.f & f);
		return w;
	}
};

/* BIT81 is, with BIT16 a key bit field in that program.
   The 81 bits of the field relate to the 81 cells of the grid.
   Again, that field can have may uses as, but not limited to

   cells controlled by one cell (not the same digit in these cells)
   description of a cell pattern
   unassigned cells
   rookeries (all candidates of the same digit)  
*/

//! Bitfield 81 positions (one per cell) and usual operations made on that field.
/** 
 * 81 bits field uses 3 32bits int<br>
 * This class is used primarely to gather positions of a candidate in 
 * the grid<br> Using logical operations enable to identify pairs or 
 * triplet for instance
 */
class BF81 {
	static int io, jo;
	//! Split position in 2 index
	/** To calculate which int in <code>i0</code> and 
	 * which position in this int in <code>j0</code> for a global position<br>
	 * <b>WARNING : </b> not thread safe !
	 */
    void ij(int v) 
	{
		io = v >> 5;
		jo = v & 0x1F;
	};   //v/32  v%32
public:  
	unsigned  int f[3];   // bitfield

    BF81(){SetAll_0();}
    BF81(int i1) {
		SetAll_0();
		Set(i1);
	}
    BF81(int i1, int i2) {
		SetAll_0();
		Set(i1);
		Set(i2);
	}
	//!Set all bits to 1
    inline void SetAll_1() {
		f[0] = f[1] =-1;
		f[2]=0x1ffff;
	}
	//!Set all bits to 0
    inline void SetAll_0() {
		f[0] = f[1] = f[2] = 0;
	}
	//!Is position <code>v</code> On
    inline int On(int v) const {
		int io = v >> 5;
		int jo = v & 0x1F;
		//ij(v);
		return (f[io] & (1 << jo));
	}
	////!Is position <code>v</code> Off
	inline int Off(int v) {
		ij(v);
		return (!(f[io] & (1 << jo)));
	}
	//!Set position <code>v</code> to On
    inline void Set(int v) {
		ij(v);
		f[io] |= 1 << jo;
	}
	//!Clear position <code>v</code>
    inline void Clear(int v) {
		ij(v);
		f[io] &= ~(1 << jo);
	}
	//!Is there any bit On
    inline int IsNotEmpty() const {
		return (f[0] || f[1] || f[2]);
	}
	//! Is there no bit On
    inline int IsEmpty() const {
		return ((f[0] | f[1] | f[2]) == 0);
	}

	BF81 operator | (const BF81 & b) const {
		BF81 w;
		w.f[0] = f[0] | b.f[0];
		w.f[1] = f[1] | b.f[1];
		w.f[2] = f[2] | b.f[2];
		return w;
	}

    void operator |= (const BF81 & b) {
		f[0] |= b.f[0];
		f[1] |= b.f[1];
		f[2] |= b.f[2];
	}

	BF81 operator & (const BF81 & b) const {
		BF81 w;
		w.f[0] = f[0] & b.f[0];
		w.f[1] = f[1] & b.f[1];
		w.f[2] = f[2] & b.f[2];
		return w;
	}

    void operator &= (const BF81 & b) {
		f[0] &= b.f[0];
		f[1] &= b.f[1];
		f[2] &= b.f[2];
	}

	BF81 operator ^ (const BF81 & b) const {
		BF81 w;
		w.f[0] = f[0] ^ b.f[0];
		w.f[1] = f[1] ^ b.f[1];
		w.f[2] = f[2] ^ b.f[2];
		return w;
	}

    void operator ^= (const BF81 & b) {
		f[0] ^= b.f[0];
		f[1] ^= b.f[1];
		f[2] ^= b.f[2];
	}

	BF81 operator -(const BF81 & b) const {
		BF81 w;
		for(int i = 0; i < 3; i++)
			w.f[i] = f[i] ^ (f[i]&b.f[i]);
		return w;
	}
    void operator -=(const BF81 & b) {
		for(int i = 0; i < 3; i++)
			f[i] ^= (f[i] & b.f[i]);
	}

	BF81 operator ~() const {
		BF81 w;
		w.f[0] = ~f[0];
		w.f[1] = ~f[1];
		w.f[2] = ~f[2];
		return w;
	}

    int operator ==(const BF81 & b) const {
		return ((f[0] == b.f[0]) && (f[1] == b.f[1]) && (f[2] == b.f[2]));
	}

    int EstDans(const BF81 &fe) const {
		return (((*this) & fe ) == (*this));
	}

	//! Clear all position that are on in <code>z</code>
    void Clear(const BF81 & z) {
		for(int i = 0; i < 3; i++)
			f[i] ^= (f[i] & z.f[i]);
	}


	//! Find the first position on
	/** \return found position or 128 if none */
	int First() const {
		for(int i = 0; i < 81; i++)
			if(On(i))
				return i;
		return 128;
	}     

	//! Count the on bits
    int Count() {
		int n = 0;
		for(int i = 0; i < 81; i++)
			if(On(i))
				n++;
		return n;
	}

 };


/* with BFCAND, we enter in a new kind of bit fields.
   BFCAND describes some candidates properties.
   some of theses properties as
   "is a weak link to another candidate defined" 
   are requiring a n * n bitfield where 'n' is the number of candidates.
   The bit field is designed for the maximum size, 
   but the process will be adjusted to the actual size.
   
   The size is 320 bits (bit 0 never used) which should be enough.
   The process is cancelled in case the number of candidates is higher
   
   some important uses of that field are 

   storing candidates contributing to a path
   storing defined links candidate to candidate
*/

//bitfield 320 bits used to store candidates used in a path

#define BFCAND_size 10
#define BFCAND_BitSize 10*32

class BFCAND {
	UINT f[BFCAND_size];     // the bit field
	static int io, jo, odd, even;
	void ij(int v) {
		io = v >> 5;
		jo = v & 31;
	};
public:
	BFCAND() {
		SetAll_0();
	}
	BFCAND(int a, int b) {
		SetAll_0();
		Set(a);
		Set(b);
	}
	BFCAND(const BFCAND &old) {
		(*this) = old;
	}
	void SetAll_0() {
		for(int i = 0; i < BFCAND_size; i++)
			f[i] = 0;
	} 
	inline int On(int v) const {
		int io = v >> 5;
		int jo = v & 31;
		//ij(v);
		return (f[io] & (1 << jo));
	}
	inline int Off(int v) const {
		int io = v >> 5;
		int jo = v & 31;
		//ij(v);
		return ((f[io] & (1 << jo)) == 0);
	}
	inline void Set(int v) {
		ij(v);
		f[io] |= (1 << jo);
	}
	inline void Clear(int v) {
		ij(v);
		if(On(v))
			f[io] ^= (1 << jo);
	}
	BFCAND operator & (const BFCAND &z2) const;
	BFCAND operator | (const BFCAND &z2) const;
	BFCAND operator ^ (const BFCAND &z2) const;
	BFCAND operator - (const BFCAND &z2) const;
	void operator &= (const BFCAND &z2);
	void operator |= (const BFCAND &z2);
	void operator ^= (const BFCAND &z2);
	void operator -= (const BFCAND &z2);
	int operator == (const BFCAND &z2) const;
	int IsNotEmpty() const {
		for(int i = 0; i < BFCAND_size; i++)
			if(f[i])
				return 1;
		return 0;
	}     
	int IsEmpty() const {
		for(int i = 0; i < BFCAND_size; i++)
			if(f[i])
				return 0;
		return 1;
	}
	int Count() ;
};

/* BFTAG is the key bitfield in the tagging process.
   Each candidates has 2 corresponding tags
     one for the status "true" in the solution
     one for the status "false" in the solution.
   all candidates linked by bivalues are sharing the same couple of tags

   The main use of that bit field is a square table n * n where 'n' is the number of tags.

   The table is designed to the maximum (2 * BFCAND size), but processed according to the actual size.
   bits 0 and 1 are not used.
*/

#define BFTAG_size 20
#define BFTAG_BitSize 20*32

/// \brief Bitfield 640 bits used in chains and within a layer in the solver.
///
/// <b>WARNING </b>: using bit at position 0 implies error when using BFTAG::First<br>
/// This bitfield uses 20 32bits unsigned int.<br>
/// Size of this bitfield class can be changed by 2 <code># define</code> lines<br>
/// A bit represents normally a tag.<br>
/// Bivalues are using 2 consecutive bits  (low order bit 0,1; same value for other bits); this is used
/// in the following method BFTAG::Layers, BFTAG::Layer_Conflict, BFTAG::FLayer, BFTAG::FInv<br>
/// For performance reason, the field is processed partially depending of the number of bit used. 
/// <code>static int isize</code> gives the number of int used and is used by all operator and by 
/// BFTAG::InitUn, BFTAG::NonNul, BFTAG::Nul<br>
/// The global variable <code>col</code> gives the number of bits (col+2)(used for optimization 
/// in the following method : 
/// BFTAG::ImageMarques, BFTAG::Count, BFTAG::Inverse, BFTAG::String, BFTAG::First)<br>
/// <b>WARNING </b>: <br>
/// 1 - As <code>col</code> is a global variable, be sure to give right value to it before calling one of these
/// method. <br>
/// 2 - All method are not thread safe due to the usage of static member as temporary variables BFTAG::io and
/// BFTAG::jo.
class BFTAG {
	UINT f[BFTAG_size];     // the bit field

	static const int false32 = 0xaaaaaaaa;  ///<A 32 bits constant with all false state for a candidate
	static const int true32 = false32 >> 1;	///<A 32 bits constant with all true state for a candidate

public:
	BFTAG() {
		SetAll_0();
	}
	BFTAG(const BFTAG &x) {
		(*this) = x;
	}
	///\brief Clear all bits
	void SetAll_0();
	///\brief Set all bits (limited to <code>isize</code> int)
	void SetAll_1();
	///\brief is bit in position <code>v</code> On
	inline int On(int v) const {
		int io = v >> 5;
		int jo = v & 31;
		return (f[io] & (1 << jo));
	}
	///\brief is bit in position <code>v</code> Off
	inline int Off(int v) const {
		int io = v >> 5;
		int jo = v & 31;
		return ((f[io] & (1 << jo)) == 0);
	}
	///\brief Set bit in position <code>v</code> to On
	inline void Set(int v) {
		f[v >> 5] |= (1 << (v & 31));
	}
	///\brief Clear bit in position <code>v</code> 
	inline void Clear(int v) {
		if(On(v))
			f[v >> 5] ^= (1 << (v & 31));
	}
	void Clear(BFTAG &z2);
	BFTAG Inverse();
	BFTAG operator & (const BFTAG &z2) const;
	BFTAG operator | (const BFTAG &z2) const;
	BFTAG operator ^ (const BFTAG &z2) const;
	BFTAG operator - (const BFTAG &z2) const;
	void operator &= (const BFTAG &z2);
	void operator |= (const BFTAG &z2);
	void operator ^= (const BFTAG &z2);
	void operator -= (const BFTAG &z2);
	int operator == (const BFTAG &z2) const;
	//BFTAG BFTAG::operator ~();
	int IsNotEmpty() const;     
	int IsEmpty() const;
	///\brief get on bits count (limit count to first <code>col+2</code> bits)
	///
	///\sa col in file _00_ assSE.h
	int Count() const;

	///\brief get positon of first bit On (limited by <code>col+2</code>)
	///
	///<b>WARNING BUG :</b> if no bit On return 0 which is the same if bit at position 0 is On.<br>
	///It seems that the first 2 bits are not used see limit to col+2.<br>
	/// <b>OPTIMIZATION :</b> possible to test each int before looking at their 32 positions
	///\return index or 0 if none
	//int First() const;
	///\brief keep only the True state or the false state
	/// 

	BFTAG TrueState() const;
	BFTAG FalseState() const;
	///\brief fill the array (first parameter) with the list of position of On Bit
	/// 
	///\param tr pointer to an array to be filled
	///\param n used to return number of position
	void String(USHORT *tr, USHORT &n) const;

	///\brief Print on output file a representation of the bitfield
	///
	///No output if o$.ot==0 <br>
	///TO VERIFY seems to be used for odd/even bivalue bitfield

	// new features not tested in preparation
	int SearchChain(BFTAG * to, USHORT i, USHORT j);
	int SearchCycle(BFTAG * to, USHORT i, BFTAG & loop);
	int SearchCycleChain(BFTAG * to, USHORT i, USHORT relay, BFTAG & loop);
	int TrackBack(BFTAG * to, USHORT start, USHORT end, USHORT * tt,
		USHORT & itt, USHORT relay);
	void Expand(BFTAG * to, USHORT i); // in nested mode, expansion limited to one tag
};
