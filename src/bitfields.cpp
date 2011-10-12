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

#include "bitfields.h"
#include <memory.h>

BF_CONVERT bfconv;

// BF81
// global variable for class BF81
//int BF81::io = 0, BF81::jo = 0;

// BFTAG
// global variable for class BFTAG
//int BFTAG::io = 0, BFTAG::jo = 0;
//int BFTAG::isize = 20;


//-----
void BFTAG::SetAll_0() {
	//for(int i = 0; i < BFTAG_size; i++)
	//	f[i] = 0;
	for(int i = 0; i < 5; i++)
		ff[i].clear();
}
void BFTAG::SetAll_1() {
	//for(int i = 0; i < BFTAG_size; i++)
	//	f[i] = -1;
	for(int i = 0; i < 5; i++)
		ff[i] = maskffff;
}
bool BFTAG::IsNotEmpty() const {
	//for(int i = 0; i < BFTAG_size; i++)
	//	if(f[i])
	//		return true;
	//return false;
	for(int i = 0; i < 5; i++)
		if(!ff[i].isZero())
			return true;
	return false;
}
bool BFTAG::IsEmpty() const {
	//for(int i = 0; i < BFTAG_size; i++)
	//	if(f[i])
	//		return false;
	//return true;
	for(int i = 0; i < 5; i++)
		if(!ff[i].isZero())
			return false;
	return true;
}

inline unsigned int popCount32(unsigned int v) { // count bits set in this (32-bit value)
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
}
//  GP 2011 10 9 should be replaced by a process using BF_CONVERT
//
int BFTAG::Count() const {
	int c = 0;
	//for(int i = 0; i < BFTAG_size; i++)
	//	c += popCount32((unsigned int) f[i]);

	for(int i = 0; i < 5; i++)
		c += popcount_128(ff[i].bitmap128.m128i_m128i);

	//for(int i = 0; i < 5; i++)
	//	for(int j = 0; j < 4; j++)
	//		c += popCount32(ff[i].bitmap128.m128i_u32[j]);

	//for(int i = 0; i < BFTAG_BitSize; i++)
	//	if(On(i))
	//		c++;

	return c;
}

// GP 2011 10 9
// should be replaced by a quick process using true32 false32
//fx= (f&true32)<<1 + (f&false32 >>1)
//----
BFTAG BFTAG::Inverse() const {
	BFTAG w;
	for(int i = 0; i </*puz.col + 2*/ BFTAG_BitSize; i++)
		if(On(i))
			w.Set(i ^ 1);
	return w;
}

BFTAG BFTAG::TrueState() const {
	BFTAG w = (*this);
	//for(int i = 0; i < BFTAG_size; i++)
	//	w.f[i] &= true32;
	for(int i = 0; i < 5; i++)
		w.ff[i] &= true128;
	return w;
};

BFTAG BFTAG::FalseState() const {
	BFTAG w = (*this);
	//for(int i = 0; i < BFTAG_size; i++)
	//	w.f[i] &= false32;
	for(int i = 0; i < 5; i++)
		w.ff[i] &= false128;
	return w;
};

//----
void BFTAG::String(USHORT * r, USHORT &n) const {
	n = 0;
	//for(int i = 0; i < BFTAG_size; i++) {
	//	if(f[i]) {
	//		for(unsigned int j = 1, k = 0; k < 32; j <<= 1, k++) {
	//			if(f[i] & j)
	//				r[n++] = (USHORT)(i * 32 + k);
	//		}
	//	}
	//}

	//for(int i = 0; i < 5; i++) {
	//	if(!ff[i].isZero()) {
	//		for(unsigned int k = 0; k < 128; k++) {
	//			if(ff[i].isBitSet(k))
	//				r[n++] = (USHORT)(i * 128 + k);
	//		}
	//	}
	//}

	for(int i = 0; i < 5; i++) {
		int m = ff[i].nonzeroOctets(); // 16-bit mask of octets having non-zero bits
		for(int oct = 0; m && oct < 16; m >>= 1, oct++) {
			if(m & 1) { //octet has a bit set
				int b = ff[i].bitmap128.m128i_u8[oct];
				for (int bit = 0; b && bit < 8; b >>= 1, bit++) {
					if(b & 1) {
						r[n++] = (USHORT)(i * 128 + oct * 8 + bit);
					}
				}
			}
		}
	}
}
//BFTAG BFTAG::operator & (const BFTAG &z2) const {
//	BFTAG w;
//	for(int i = 0; i < BFTAG_size; i++)
//		w.f[i] = f[i] & z2.f[i];
//	return w;
//}
//BFTAG BFTAG::operator | (const BFTAG &z2) const {
//	BFTAG w;
//	for(int i = 0; i < BFTAG_size; i++)
//		w.f[i] = f[i] | z2.f[i];
//	return w;
//}
//BFTAG BFTAG::operator ^ (const BFTAG &z2) const {
//	BFTAG w;
//	for(int i = 0; i < BFTAG_size; i++)
//		w.f[i] = f[i] ^ z2.f[i];
//	return w;
//}
void BFTAG::operator &= (const BFTAG &z2) {
	//for(int i = 0; i < BFTAG_size; i++)
	//	f[i] &= z2.f[i];
	for(int i = 0; i < 5; i++)
		ff[i] &= z2.ff[i];
}
void BFTAG::operator |= (const BFTAG &z2) {
	//for(int i = 0; i < BFTAG_size; i++)
	//	f[i] |= z2.f[i];
	for(int i = 0; i < 5; i++)
		ff[i] |= z2.ff[i];
}
//void BFTAG::operator ^= (const BFTAG &z2) {
//	for(int i = 0; i < BFTAG_size; i++)
//		f[i] ^= z2.f[i];
//}
bool BFTAG::operator == (const BFTAG &z2) const {
	//for(int i = 0; i < BFTAG_size; i++)
	//	if(!(f[i] == z2.f[i]))
	//		return false;
	//return true;
	for(int i = 0; i < 5; i++)
		if(!(ff[i] == z2.ff[i]))
			return false;
	return true;
}
//BFTAG BFTAG::operator - (const BFTAG &z2) const {
//	BFTAG w;
//	  for(int i = 0; i < BFTAG_size; i++)
//		  w.f[i] = f[i] ^ (f[i] & z2.f[i]);
//	  return w;
//}
void BFTAG::operator -= (const BFTAG &z2) {
	//for(int i = 0; i < BFTAG_size; i++)
	//	f[i] &= ~z2.f[i];
	for(int i = 0; i < 5; i++)
		ff[i] -= z2.ff[i];
}

bool BFTAG::substract(const BFTAG &z2) {
	//UINT accum = 0;
	////for(int i = 0; i < BFTAG_size; i++) {
	////	accum |= (f[i] &= ~z2.f[i]);
	////}
	////for(int i = 0; i < BFTAG_size; i++) {
	////	  f[i] &= ~z2.f[i];
	////	  accum |= f[i];
	////}

	//accum |= (f[0] &= ~z2.f[0]);
	//accum |= (f[1] &= ~z2.f[1]);
	//accum |= (f[2] &= ~z2.f[2]);
	//accum |= (f[3] &= ~z2.f[3]);
	//accum |= (f[4] &= ~z2.f[4]);
	//accum |= (f[5] &= ~z2.f[5]);
	//accum |= (f[6] &= ~z2.f[6]);
	//accum |= (f[7] &= ~z2.f[7]);
	//accum |= (f[8] &= ~z2.f[8]);
	//accum |= (f[9] &= ~z2.f[9]);
	//accum |= (f[10] &= ~z2.f[10]);
	//accum |= (f[11] &= ~z2.f[11]);
	//accum |= (f[12] &= ~z2.f[12]);
	//accum |= (f[13] &= ~z2.f[13]);
	//accum |= (f[14] &= ~z2.f[14]);
	//accum |= (f[15] &= ~z2.f[15]);
	//accum |= (f[16] &= ~z2.f[16]);
	//accum |= (f[17] &= ~z2.f[17]);
	//accum |= (f[18] &= ~z2.f[18]);
	//accum |= (f[19] &= ~z2.f[19]);
	//return accum != 0;
	bm128 accum;
	//accum.clear();
	ff[0] -= z2.ff[0]; accum = ff[0];
	ff[1] -= z2.ff[1]; accum |= ff[1];
	ff[2] -= z2.ff[2]; accum |= ff[2];
	ff[3] -= z2.ff[3]; accum |= ff[3];
	ff[4] -= z2.ff[4]; accum |= ff[4];
	return !accum.isZero();
}

//------
//int BFTAG::First() const {
//	for(int i = 0; i < /*puz.col + 2*/ BFTAG_BitSize; i++) {
//		if(On(i))
//			return i;
//	}
//	return 0;
//}

/* search in the area x cycle to xy chain
   1) a cycle can give no elimination.
   2) cycle search can be very long especially in Y mode

   To search cycles, 
   a) look first for the shortest elimination
   b) compute the max length for a shorter cycle 
      (max of the same slice in the "difficulty" scale)
   c) expand all and sort out tags eliminated and tags in loop
   d) Locate tags seing 2 tags in loop and try to expand that loop  

   Note, this is not a key process. Eliminations have been found
   It would be easy to speed it up using the even odd property
   Doing more is tougher but possible

*/
/* Look (no derived weak link) for the shortest way from start  to end
   the calling sequence provides an empty or partially filled situation
*/
int BFTAG::SearchChain(const BFTAG *to, USHORT start, USHORT end) {
	int npas = 0; 
	// to be safe in nested mode, dimension increased to 200
	USHORT tta[200], ttb[200], *told = tta, *tnew = ttb, itold, itnew;
	(*this).String(tta, itold);
	while(npas++ < 30) {
		tnew = (told == tta) ? ttb : tta; // new the second table
		itnew = 0;
		// EE.E("cycle");zpln.PrintListe(told,itold,1); 
		//int aig = 1; // to detect an empty pass
		for(int it = 0; it < itold; it++) {
			BFTAG x = to[told[it]];
			//x -= (*this);
			//if(x.IsNotEmpty()) {
			if(x.substract(*this)) {
				(*this) |= x; // flag it in the BFTAG and load in new
				// here tx dimension increased. In nested mode, could require it
				//USHORT tx[40], itx = 0;
				//x.String(tx, itx);
				//for(int i = 0; i < itx; i++)
				//	tnew[itnew++] = tx[i];
				USHORT itx;
				x.String(&tnew[itnew], itx);
				itnew += itx;
			}
		}

		if(On(end)) {
			//EE.E("cycle fin npas= ");EE.E(npas);zpln.PrintListe(tnew,itnew,1); 
			return npas; // eliminations found
		}
		if(!itnew)
			return 0;     // empty pass should never be
		itold=itnew;
		told=tnew; // new -> old
	}// end while
	// should send a warning message for debugging purpose
	return 0;
}
/* same process, but start == end  (cycle)
   and all tags of the path must belong to the loop
*/
int BFTAG::SearchCycle(const BFTAG *to, USHORT start, const BFTAG &loop) {
	int npas = 0; 
	USHORT tta[100], ttb[100], *told = tta, *tnew = ttb, itold, itnew;
	(*this).String(tta, itold);
	while(npas++ < 30) {
		tnew = (told == tta) ? ttb : tta; // new the second table
		itnew = 0;
		// EE.E("cycle");zpln.PrintListe(told,itold,1); 
		for(int it = 0; it < itold; it++) {
			//BFTAG x = (to[told[it]] - (*this)) & loop ;	  
			BFTAG x = to[told[it]];
			x -= (*this);
			x &= loop;
			if(x.IsNotEmpty()) {
				(*this) |= x; // flag it in the BFTAG and load in new
				//USHORT tx[20], itx = 0;
				//x.String(tx, itx);
				//for(int i = 0; i < itx; i++)
				//	tnew[itnew++] = tx[i];
				USHORT itx;
				x.String(&tnew[itnew], itx);
				itnew += itx;
			}
		}
		if(On(start))
			return npas; // eliminations found
		if(!itnew)
			return 0;     // empty pass should never be
		itold = itnew;
		told = tnew; // new -> old
	}// end while
	// should send a warning message for debugging purpose
	return 0;
}

/* same process, but partial
   we only go to the tag 'relay' that should come first but may be not
*/
int BFTAG::SearchCycleChain(const BFTAG *to, USHORT i, USHORT relay, const BFTAG &loop) {
	int npas = 0; 
	USHORT tta[100], ttb[100], *told = tta, *tnew = ttb, itold, itnew;
	(*this).String(tta, itold); 
	(*this).Set(i); // lock crossing back i
	while(npas++ < 20) {
		tnew = (told == tta) ? ttb : tta; // new the second table
		itnew = 0;
		for(int it = 0; it < itold; it++) {
			//BFTAG x = (to[told[it]] - (*this)) & loop;
			BFTAG x = to[told[it]];
			x -= (*this);
			x &= loop;
			if(x.IsNotEmpty()) {
				(*this) |= x; // flag it in the BFTAG and load in new
				//USHORT tx[20], itx = 0;
				//x.String(tx, itx);
				//for(int i = 0; i < itx; i++)
				//	tnew[itnew++] = tx[i];
				USHORT itx;
				x.String(&tnew[itnew], itx);
				itnew += itx;
			}    
		}
		if(On(relay))
			return npas; // target found
		if(!itnew)
			return 0;     // empty pass should never be
		itold = itnew;
		told = tnew; // new -> old
	}// end while
	// may be a warning here for debugging purpose
	return 0;
}
/* trackback gives back a path out of the search
   to make easier the split in modules, the path is given as a table of tags

   The forward path as such can not be used. It must be reworked step by step.
   The specific rules
     start with a weak link in loop mode
	 go thru in Y loop moe
   have to be applied again.

   This will be done giving a "relay" candidate (the first in X or XY loop mode)


   applied to  the result of the forward search (most often in test mode)
   end is the end tag.

   The process works for sure in elimination mode , but also in cycle mode 
   the candidate of the end step can not be in the first step ... 

   The process can be slightly improved considering the following

   a cycle here ends always with a strong link and has an odd number of steps
   a chain has always and even number of steps and ends with a weak link
   all that to be checked carefully

   to avoid circular path with strong links, each tag used is cleared

   not yet enough to be revised
   need really to build forward step by step before going backward
 */
int BFTAG::TrackBack(const BFTAG *to, USHORT start, USHORT end, USHORT * tt, USHORT & itt, USHORT relay) const {
	// first we have to build forward step by step 
	if(itt > 40) {
		/*  debugging infromation to be relocated in the calling sequence
		EE.E("trackback to many steps=");
		EE.E(itt);
		EE.E(" start=");
		zpln.ImageTag(start); 
		EE.E(" end=");
		zpln.ImageTag(end); 
		EE.E(" relay=");
		zpln.ImageTag(relay);
		EE.Enl();*/
		return 1;
	}
	BFTAG steps[50], allsteps; // surely never so many
	USHORT tx[50][200], itx[50], npas = 0;
	steps[0].SetAll_0();
	steps[0].Set(start); 
	allsteps = steps[0]; // one way to force weak link at the start in loop mode
	tx[0][0] = start;
	itx[0] = 1; // initial is start to go forward
	while(npas < (itt - 1)) { // must end in step itt
		BFTAG * step = &steps[npas+1];
		step->SetAll_0();
		USHORT *ta = tx[npas], *tb = tx[npas + 1],
			ita = itx[npas], itb = 0;
		for(int it = 0; it < ita; it++) {
			//BFTAG x = (to[ta[it]] - allsteps) & (*this);	  // still free and in the overall path
			BFTAG x = to[ta[it]];
			x -= allsteps;
			x &= (*this);	  // still free and in the overall path
			if(x.IsNotEmpty()) {
				(*step) |= x; // flag it in the BFTAG and load in new
				allsteps |= x; // and in the total 
				//USHORT ty[40], ity = 0;
				//x.String(ty,ity);
				//for(int i=0; i < ity; i++)
				//	tb[itb++] = ty[i];
				USHORT ity;
				x.String(&tb[itb], ity);
				itb += ity;
			}
		}
		if(step->On(end))
			break;
		if(step->On(relay)) { // if "relay" is reached, force tb to relay only
			for(int k = 0; k < itb; k++) { // clear allsteps
				USHORT xx = tb[k];
				if(xx-relay) allsteps.Clear(xx);
			}	
			if(start == end)
				allsteps.Clear(end);
			itb = 1;
			tb[0] = relay;  
		}

		npas++;
		itx[npas] = itb; 
		//   EE.E("passe npas=");EE.E(npas); EE.Esp();
		//   zpln.PrintListe(tb,itb,1);
	}// end while
	if((npas + 2) - itt) {
		/* debugging code to relocate
		if(1 && Op.ot) {
			EE.E("invalid trackback end phase 1 npas==");
			EE.E(npas);
			EE.E(" itt=");
			EE.Enl(itt);
			EE.E(" start=");
			zpln.ImageTag(start); 
			EE.E(" end=");
			zpln.ImageTag(end); 
			EE.E(" relay=");
			zpln.ImageTag(relay);
			EE.Enl();
			for(int iw = 0; iw <= npas; iw++) {
				EE.E("step iw=");
				EE.E(iw);
				steps[iw].Image("",0);
			}
		}
		 end of debugging code to relocate*/ 
		return 1;
	}
	//second phase, goback using the tx[] tables

	tt[0] = start;
	tt[itt - 1] = end; 
	for(int i = itt - 2; i > 0; i--) { // we go back from the end
		tt[i] = 0; 
		USHORT last = tt[i + 1],
			*ta = tx[i],
			ita = itx[i]; 
		for(int jj = 0; jj < ita; jj++) {
			int j = ta[jj]; // j must be in the forward way
			if(to[j].Off(last))
				continue; // must be parent of last tag
			tt[i] = j;
			break; // should always find one the first is ok
		}
		// error in the process this is a debugging message
		if(!tt[i]) {
			/* debugging code to relocate
			EE.E("invalid trackback step=");
			EE.E(i); 
			EE.E(" last=");
			zpln.ImageTag(last); 
			EE.E(" start=");
			zpln.ImageTag(start); 
			EE.E(" end=");
			zpln.ImageTag(end); 
			EE.E(" relay=");
			zpln.ImageTag(relay);
			EE.Enl();
			allsteps.Image("allsteps", 0);
			 end of debugging code to relocate */
			return 1;
		}
	}
	return 0;
}

// GP 2011 10 9  <<<<<<<<<<<<<<<<<<<<<<<  suggested to move that in PUZZLE
// this is more sensitive in performance that the previous ones
/* final expansion in nested mode of a specific BFTAG */
//void BFTAG::Expand(BFTAG * to, USHORT i) {
//	int n = 1;
//	while(n) {
//		n = 0;
//		for(int j = 2; j < /*puz.col + 2*/ BFTAG_BitSize; j++) {
//			if((j - i) && (*this).On(j)) {
//				BFTAG x = to[j];
//				x -= (*this);
//				if(x.IsNotEmpty()) {
//					(*this) |= x;
//					//n++;
//					n = 1;
//				}
//			}
//		} // end j
//	} // end while
//}