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
/*
#include "skfrtype.h"
#include "bitfields.h"
#include "_10f_tdb.h"
*/
/* an attempt to design an alternative process for the search 
in dynamic mode and later
*/

#define pasmax 70
class CANDGO {
public:
	USHORT aig,ret_code,npas;
	int opp;
	short tsets[640];
	USHORT tx[pasmax][200],itx[pasmax];
	USHORT tret[300],pasret[300],itret;
	BFTAG steps[pasmax],cumsteps[pasmax],allsteps, * to;  
	BFTAG  *cum ,* step; 
	USHORT *ta,*tb,ita,itb;
	CANDGO(){opp=0;} // print option in nested
	TDB dpn,dn; // in nested mode, dynamic set of primary links
	// look first for direct 

	int GoCand(USHORT tag);  // locate the contradiction
	int GoOne(USHORT tag, const BFTAG &tagse); // find all targets
	void GoSets();
	int GoBack(USHORT tag,int pr);  // compute the length for one chain 
	int GoBackNested(USHORT tag,int pr);  // compute the length for one chain 
	void GoNestedTag(USHORT tag,USHORT base);  // get expanded situation 
	//int GoNested(USHORT cand,BFTAG * tagnot,USHORT base);  // locate the contradiction
	int GoNestedCase1(USHORT cand,USHORT base);  // locate the contradiction with case 1
	int GoNestedCase2_3(USHORT base      // locate the contradiction  
		,USHORT tag,USHORT target);  // case 2 or case 3
	void GoNestedWhile(USHORT tag,USHORT base);
	void GoNestedWhileShort(USHORT tag,USHORT base);
	void Gen_dpn(USHORT tag);
	void Gen_dpnShort(USHORT tag);
	void NestedForcing(BFTAG & elims);
	void NestedForcingShort(BFTAG & elims);
	void NestedMulti(BFTAG & elims);
	void NestedMultiShort(BFTAG & elims);
};
