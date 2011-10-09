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
/*
#include "skfrtype.h"
#include "_20a_event.h"
#include "_10g_cf.h"
#include "bitfields.h"
*/
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
	int GenForTag(USHORT cand, WL_TYPE type);
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
	void Load(USHORT tage, EVENT_TYPE evt, EVENTLOT & evb, EVENTLOT & evx);
	int IsPattern (USHORT cand) {
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
	void LoadXWD(USHORT ch, USHORT el1, USHORT el2, USHORT p1, USHORT p2, EVENTLOT & eva, EVENTLOT & evx);
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

	int EventSeenFor(USHORT tagfrom, USHORT tagevent);
	void LoadFin(); // only for debugging purpose
}; //tevent;

