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
#include "_20a_event.h"
#include "ratingengine.h"
*/
USHORT event_vi = 1024;

EVENTLOT::EVENTLOT(BF81 &x, USHORT ch) {
	itcd = 0;
	for(int i = 0; i < 81; i++)
		if(x.On(i))
			AddCand(i, ch);
}

void EVENTLOT::AddCand(USHORT cell, USHORT ch) {
	tcd[itcd++] = zpln.indexc[ch*81 + cell];
}

int EVENTLOT::GenForTag(USHORT tag, WL_TYPE type) {
	for(int i = 0; i < itcd; i++) {
		USHORT cand2 = tcd[i], tag2 = (cand2 << 1) ^ 1;
		if(zcf.Isp(tag, tag2))
			continue;// only if not yet defined
		if(op1 && (tag ==(tag2 ^ 1))) {
			EE.E("gen contraire");
			Image();
			return 1;
		}

		if(tag == tag2)
			continue;// redundancy
		if(type == wl_ev_direct)
			zcf.LoadEventDirect(tag >> 1, cand2); 
		else zcf.LoadEventTag(tag, cand2);
	}
	return 0;
}

void EVENTLOT::Image() const {
	EE.E(" evenlot itcd=");
	EE.E(itcd);
	for(int i = 0; i < itcd; i++) {
		EE.Esp();
		zpln.Image(tcd[i]);
	}
	EE.Enl();
}

void EVENT::Load(USHORT tage, EVENT_TYPE evt, EVENTLOT & evb, EVENTLOT & evx) {
	tag = tage;
	type = evt;
	evl = evb;
	ntcand = evx.itcd;
	if(ntcand > 4)
		ntcand = 4;
	for(int i = 0; i < ntcand; i++)
		tcand[i] = evx.tcd[i];
}

/* we have found the 2 sets (EVENLOT) linked to the event
   create the entry in the table TEVENT
   if first set is more than one create entry in TCHOIX
   else generate directly conflicts for candidates in conflict
        if they don't belong to the pattern 
   evt event type
   eva lot of candidates creating the event
   evb lot of candidates not valid if the pattern is established
   evx candidates of the pattern
   */

void TEVENT::EventBuild(EVENT_TYPE evt, EVENTLOT & eva, EVENTLOT & evb, EVENTLOT & evx) {
	if(eva.itcd < 1 || evb.itcd < 1)
		return;
	// if only one in the set, go to gen and return
	if(eva.itcd == 1) {
		USHORT cw = eva.tcd[0]; 
		if(op0 && Op.ot) {
			EE.E("gen direct event type=");
			EE.E(evt);
			EE.Esp(); 
			eva.Image();
			evb.Image();
			EE.Enl();
		}
		evb.GenForTag((cw << 1) ^ 1, wl_ev_direct);
		return;
	}
	if(eva.itcd > chx_max)
		return; // forget if too big	 
	if(it >= event_size) {
		EE.Elimite("TEVENT limite atteinte");
		return;
	}
	USHORT evv = event_vi + (it);
	t[it++].Load(evv, evt, evb, evx); 
	eva.tcd[eva.itcd] = evv;
	zcx.ChargeSet(eva.tcd, eva.itcd + 1, CH_set); // set loaded
}

 /* the event has been created by  "tagfrom" for the event "tagevent"
    find the corresponding element in the talbe and generate the weak links
	*/
int TEVENT::EventSeenFor(USHORT tagfrom, USHORT tagevent) {
	USHORT candfrom = tagfrom >> 1, ind = tagevent - event_vi;  
	if(ind < 1 || ind >= it)
		return 0; // should never be
	if(t[ind].IsPattern(candfrom))
		return 0;// ?? do nothing if part of the pattern not 100% sure
	if(t[ind].evl.GenForTag(tagfrom, wl_event)) { // diag a supprimer
		t[ind].Image();
		return 1;
	} 
	return 0; 
}

void EVENT::Image() const {
	if(!Op.ot)
		return;
	EE.E(" event image tag=");
	EE.E(tag);
	EE.E(" type=");
	EE.E(type); 
	for(int i = 0; i < ntcand; i++) {
		EE.Esp();
		zpln.Image(tcand[i]);
	} 
	evl.Image();
}

/* process to locate all potential XW
   load the corresponding tags and sets
   
*/
void TEVENT::LoadXW() {
	for(int i1 = 0; i1 < 8; i1++) {
		for(int j1 = 0; j1 < 8; j1++) {  // first cell
			USHORT cell1 = I81::Pos(i1, j1);
			if(T81t[cell1].v.ncand < 2)
				continue;
			for(int i2 = i1 + 1; i2 < 9; i2++) {
				USHORT cell2 = I81::Pos(i2, j1);
				if(T81t[cell2].v.ncand < 2)
					continue;
				BF16 ch2 = T81t[cell1].v.cand & T81t[cell1].v.cand;
				if(!ch2.f)
					continue; // must have at least a common digit
				for(int j2 = j1 + 1; j2 < 9; j2++) {
					USHORT cell3 = I81::Pos(i1, j2);
					if(T81t[cell3].v.ncand < 2)
						continue;
					USHORT cell4 = I81::Pos(i2, j2);
					if(T81t[cell4].v.ncand < 2)
						continue;
					BF16 cht = ch2 & T81t[cell3].v.cand & T81t[cell4].v.cand;
					if(!cht.f)
						continue; // must have at least a common digit
					// we now explore all possible row and column XWings in that group
					for(int ich = 0; ich < 9; ich++) {
						if(cht.On(ich)) { // all possible digits
							// build the row  and the column sets
							EVENTLOT evr, evc, evx;
							LoadXWD(ich, i1, i2, j1, j2, evr, evx);
							if(!evr.itcd)
								continue; // pure XWing
							LoadXWD(ich, j1 + 9, j2 + 9, i1, i2, evc, evx); // now can not be a pure XWing
							EventBuild(evxwcol, evr, evc, evx);
							EventBuild(evxwrow, evc, evr, evx);
						}
					} //ich
				} // j2
			} // i2
		} // j1
	}// i1
}

/* we have identified an XW pattern
   generate if any the set or the direct event weak links  */
void TEVENT::LoadXWD(USHORT ch, USHORT el1, USHORT el2, USHORT p1, USHORT p2, EVENTLOT & eva, EVENTLOT & evx) {
	OBBITD el1d = aztob.tchbit.el[el1].eld[ch], el2d = aztob.tchbit.el[el2].eld[ch];
	for(int i = 0; i < 9; i++)
		if(el1d.b.On(i))
			if((i - p1) && (i - p2))
				eva.AddCand(divf.el81[el1][i], ch); 
			else
				evx.AddCand(divf.el81[el1][i], ch);
	for(int i = 0; i < 9; i++)
		if(el2d.b.On(i))
			if((i - p1) && (i - p2))
				eva.AddCand(divf.el81[el2][i], ch);
			else
				evx.AddCand(divf.el81[el2][i], ch);
}

void TEVENT::LoadPairs() { // all rows, columns, and boxes  
	for(int iel = 0; iel < 27; iel++) {
		// pick up 2 unknown cells in that region
		for(int i1 = 0; i1 < 8; i1++) {
			USHORT cell1 = divf.el81[iel][i1];
			if(T81t[cell1].v.ncand < 2)
				continue;
			for(int i2 = i1 + 1; i2 < 9; i2++) {
				USHORT cell2 = divf.el81[iel][i2];
				if(T81t[cell2].v.ncand < 2)
					continue;
				LoadPairsD(cell1, cell2, iel);
			}
		} // end i1;i2
	}// end iel
}
// work on a pair 
void TEVENT::LoadPairsD(USHORT cell1, USHORT cell2, USHORT iel) {
	BF16 ch2 = T81t[cell1].v.cand & T81t[cell2].v.cand,
		chor = T81t[cell1].v.cand | T81t[cell2].v.cand; // pour traiter le cas 1 commun
	if(ch2.QC() < 2)
		return; // non il faudrait aussi accepter 1 commun à revoir
	// nothing to do if box/row and box/col (already done)
	P81F p1 = t81f[cell1], p2=t81f[cell2];
	if(iel > 17 && ((p1.el == p2.el) || (p1.pl == p2.pl)))
		return;
	for(int i1 = 0; i1 < 8; i1++) {
		if(!ch2.On(i1)) 
			continue;
		for(int i2 = i1 + 1; i2 < 9; i2++) {
			if(!ch2.On(i2))
				continue;
			//all pairs must be processed
			// buid the set for nacked pair	 
			EVENTLOT nack, hid1, hid2, evx; 
			BF16 com(i1, i2), v1 = T81t[cell1].v.cand, v2 = T81t[cell2].v.cand;
			for(int j = 0; j < 9; j++) {
				if(v1.On(j))
					if(com.On(j))
						evx.AddCand(cell1, j);
					else
						nack.AddCand(cell1, j);
				if(v2.On(j))
					if(com.On(j))
						evx.AddCand(cell2, j);
					else
						nack.AddCand(cell2, j);
			}
			// build the set for hidden pair in el and generate the event
			PairHidSet(cell1, cell2, iel, com, hid1);
			if(nack.itcd <= chx_max)
				EventBuild(evpairnacked, nack, hid1, evx);
			if(hid1.itcd <= chx_max)
				EventBuild(evpairhidden, hid1, nack, evx);

			// if el is row col and same box, do it as well for the box
			if(iel > 17 || (p1.eb - p2.eb))
				continue;
			PairHidSet(cell1, cell2, p2.eb + 18, com, hid2);
			if(nack.itcd <= chx_max)
				EventBuild(evpairnacked, nack, hid2, evx);
			if(hid2.itcd <= chx_max)
				EventBuild(evpairhidden, hid2, nack, evx);
		}
	}
}

void TEVENT::PairHidSet(USHORT cell1, USHORT cell2, USHORT el, BF16 com, EVENTLOT & hid) {
	for(int i = 0; i < 9; i++) {
		int cell = divf.el81[el][i];
		if((cell == cell1) || (cell == cell2))
			continue;
		UNP p = T81t[cell].v;
		if(p.ncand < 2)
			continue;
		BF16 w = p.cand&com;
		if(!w.f)
			continue;
		for(int j = 0; j < 9; j++)
			if(w.On(j))
				hid.AddCand(cell, j);
	}
}

/* prepare all claiming pointing sets
   to be done in each row/box row/col if there is a digit not locked
*/

void TEVENT::LoadLock() {
	for(int iel = 0; iel < 18; iel++) {
		for(int ib = 18; ib < 27; ib++) {
			for(int ich = 0; ich < 9; ich++) {
				BF81 chel = divf.elz81[iel] & jdk.c[ich];  // the elem pattern for the ch
				if(chel.IsEmpty())
					continue; // nothing  
				BF81 chbcom = chel & divf.elz81[ib]; // common part with the block
				if(chel == chbcom)
					continue; //  already locked
				if(chbcom.Count() < 2)
					continue; // nothing to do I guess
				chel -= chbcom; // set in row col
				BF81 chb = (divf.elz81[ib] & jdk.c[ich]) - chbcom; // set in box

				// check what does SE if evrc,evb,evx all one candidate ?? 

				EVENTLOT evrc(chel, ich), evb(chb, ich), evx(chbcom, ich);
				EventBuild(evlockrc, evrc, evb, evx);
				EventBuild(evlockbox, evb, evrc, evx);
			}
		}
	}
}

/* code for preliminary tests
 status of table after having loaded events
 */
void TEVENT::LoadFin() {
	if(!Op.ot)
		return;

	EE.E("check after having loaded events nb events=");
	EE.Enl(it);
	EE.E("zcf ic=");
	EE.Enl(zcf.ic);
	EE.E("zcx izc=");
	EE.Enl(zcx.izc);
	for(int i = 1; i < it; i++) {
		EVENT ev = t[i]; 
		EE.E("ev type=");
		EE.E(ev.type);
		EE.E(" tag=");
		EE.E(ev.tag);
		for(int j = 0; j < ev.ntcand; j++) {
			EE.Esp();
			zpln.Image(ev.tcand[j]);
		}
		ev.evl.Image();
	}
	zcx.Image();
}
