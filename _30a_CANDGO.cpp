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
#include "_30a_CANDGO.h"
#include "_30a_tcandgo.h"
#include "_11m_choix.h"
#include "ratingengine.h"

*/


//former _30a_candgo_gosets.cpp follows
//--------------------------
// subfunction for GoCand or GoOne, add sets valid


void CANDGO::GoSets() {
	for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		int n = 0, nni = chx.ncd, aig2 = 0; 
		switch (chx.type) {
		case SET_base:  // must be n-1 false or n false (then all is ok)
			for(int i = 0; i < nni; i++) { // max one free 
				if(cum->Off((chx.tcd[i] <<1 ) ^ 1)) {
					n++;
					if(n > 1)
						break;
				}
			}
			if(n > 1)
				break;	// if ok second round for action	
			for(int i = 0; i < nni; i++) {
				int j = (chx.tcd[i] << 1); 
				if(((!n) && allsteps.Off(j)) || (n && cum->Off(j ^ 1)) && allsteps.Off(j)) {
					(*step).Set(j);
					allsteps.Set(j); // and in the total 
					tb[itb++] = j;
					tsets[j] = ie;
					aig = 1;
					aig2 = 1;       
				}
			}
			if(0 && Op.ot && aig2) {
				EE.E("set derive actif ");
				chx.Image();
				EE.Enl();
			}

			break;
		case SET_set: // in a set all components must be on
			for(int i = 0; i < (nni - 1); i++) {
				if(cum->Off((chx.tcd[i] << 1) ^ 1)) {
					n++;
					if(n)
						break;
				}
			}
			if(n)
				break; // must all be there
			const EVENT &ev = tevent.t[chx.tcd[nni - 1] - event_vi];
			const EVENTLOT &evl = ev.evl;
			for(int i = 0; i < evl.itcd; i++) {
				int j = (evl.tcd[i] << 1) ^ 1; // false state on
				if(allsteps.On(j))
					continue;
				(*step).Set(j);
				allsteps.Set(j); // and in the total 
				tb[itb++] = j;
				tsets[j] = ie;
				aig = 1;
				aig2 = 1;       
			}
			if(0 && Op.ot&& aig2) {
				EE.E("set actif event");
				chx.Image();
				ev.Image();
			}
			break;	
		} // end switch
	}// end for ie
}

//former _30a_candgo_cases.cpp follows
/* 


//piece of _30a_CANDGO.h followed

/* here weknow that a contradiction must occur 
   if the path is small enough
   */

int CANDGO::GoCand(USHORT tag) {
	if(Op.ot && 1) {
		EE.E("gocand killing");
		zpln.ImageTag(tag);
		EE.Enl();
	}
	to = zcf.hdp_base.t; // we use the table without derivations
	for(int i = 0; i < 640; i++)
		tsets[i]=0;
	npas = 0;
	steps[0].SetAll_0();
	steps[0].Set(tag); 
	allsteps = cumsteps[0] = steps[0];  
	tx[0][0] = tag;
	itx[0] = 1; // initial is tag to go forward

	ret_code = 0;
	aig = 1;
	int maxpas = (tchain.maxlength > 52) ? 50 : tchain.maxlength - 2;
	while(aig && npas++ <= maxpas) {
		aig=0; 
		cum = &cumsteps[npas-1];
		step=&steps[npas];
		step->SetAll_0();
		ta = tx[npas-1];
		tb =tx[npas];
		ita = itx[npas-1];
		itb=0;
		// look first for direct 
		if(0) {
			EE.E("step");
			EE.Enl(npas);
			//cum->Image("cumstep",0);
			//steps[npas-1].Image("step precedent",0); 
		}

		for(int it=0; it < ita; it++) {
			BFTAG x = to[ta[it]];
			//x -= allsteps;	  // still free and in the overall path
			//if(x.IsNotEmpty()) {
			if(x.substract(allsteps)) {
				(*step)  |= x; // flag it in the BFTAG and load in new
				allsteps |= x; // and in the total 
				USHORT ty[30], ity = 0;
				x.String(ty, ity);
				for(int i = 0; i < ity; i++)
					tb[itb++] = ty[i];
				aig = 1;
			}    
		}
		GoSets();                    // look now for sets

		if(0 && Op.ot) {
			EE.E("fin step=");
			EE.E(npas);
			puz.Image((*step),"step ",0);
		}
		cumsteps[npas] = cumsteps[npas-1];
		cumsteps[npas] |= (*step);
		itx[npas] = itb;
		// check for a contradiction in that lot (stop at first)
		for(int i=0 ; i < itb; i++) {
			USHORT tgx = tb[i];
			if(allsteps.On(tgx) && allsteps.On(tgx^1)) {
				if(0) {
					EE.E("found active");
					zpln.ImageTag(tgx);
					EE.E(" step=");
					EE.Enl(npas);   
				}
				// we compute back the length and show the sequence 
				if(maxpas > npas)
					maxpas = npas; // limit the process to 1 more step
				int l1 = GoBack(tgx,0), l2 = GoBack(tgx^1,0);
				if((!l1) || (!l2)) continue; // should not be
				int ratch = tchain.GetRating(l1 + l2, tag >> 1);
				if(ratch) { // chain is accepted load it (more comments in test mode)
					if(Op.ot) { // this can be complex and will be developped later
						EE.E("chain plus killing ");
						zpln.Image(tag>>1);
						EE.Enl();	     
						EE.E("chain 1 ");
						GoBack(tgx,1) ;
						EE.E("chain 2 ");
						GoBack(tgx^1,1);
					}
					tchain.LoadChain(ratch, "chain plus", tag >> 1);
				}
			}
		}
		if(0 && !aig) {
			EE.E("fin aig=0 pour step=");
			EE.Enl(npas);
		}
	}// end while
	return ret_code;
}

//---------------------------------------------

int CANDGO::GoOne(USHORT tag, const BFTAG &tagse) {
	if(0 && Op.ot) {
		EE.E("goone for ");
		zpln.ImageTag(tag); 
		puz.Image(tagse ,"killing one of these", 0);
	}
	to = zcf.hdp_base.t; // we use the table without derivations
	for(int i = 0; i < 640; i++)
		tsets[i]=0;
	npas = 0;
	steps[0].SetAll_0();
	steps[0].Set(tag); 
	allsteps = cumsteps[0] = steps[0];  
	tx[0][0] = tag;
	itx[0] = 1; // initial is tag to go forward

	aig = 1;
	int maxpas = (tchain.maxlength > 52) ? 50 : tchain.maxlength - 2;
	while(aig && npas++ <= maxpas) {
		aig=0; 
		cum = &cumsteps[npas - 1];
		step = &steps[npas];
		step->SetAll_0();
		ta = tx[npas - 1];
		tb = tx[npas];
		ita = itx[npas - 1];
		itb = 0;
		// look first for direct 
		if(0) {
			EE.E("step");
			EE.Enl(npas);
			puz.Image((*cum),"cumstep", 0);
			puz.Image(steps[npas - 1],"step precedent", 0);
		}


		for(int it = 0; it < ita; it++) {
			//BFTAG x = to[ta[it]] - allsteps;
			BFTAG x(to[ta[it]]);
			//x -= allsteps;
			//if(x.IsNotEmpty()) {
			if(x.substract(allsteps)) {
				(*step) |= x; // flag it in the BFTAG and load in new
				allsteps |= x; // and in the total 
				USHORT ty[30], ity = 0;
				x.String(ty, ity);
				for(int i = 0; i < ity; i++)
					tb[itb++] = ty[i];
				aig = 1;
			}
		}
		GoSets();                    // look now for sets

		if(0 && Op.ot) {
			EE.E("fin step=");
			EE.E(npas);
			puz.Image((*step),"step ", 0);
		}

		cumsteps[npas] = cumsteps[npas - 1];
		cumsteps[npas] |= (*step);
		itx[npas] = itb;
		// check for all the target found
		if(0) {
			BFTAG ww = cumsteps[npas];
			ww &= tagse;
			if(ww.IsNotEmpty()) {
				EE.E("fin step=");
				EE.Enl(npas);
				puz.Image(ww,"trouvés ", 0);
			}
		}
		//BFTAG w = tagse - cumsteps[npas];
		BFTAG w(tagse);
		w -= cumsteps[npas];

		if(w.IsEmpty())
			return 1;
		if(0 && (!aig)) {
			EE.E("fin aig=0 pour step=");
			EE.Enl(npas);}
	}// end while
	return 0;
}

//--------------------------------------------------

int CANDGO::GoBack(USHORT tag, int pr) {
	if(0) {
		EE.E("goback");
		zpln.ImageTag(tag);
		EE.Enl();
	}
	USHORT itret1 = 0;
	itret = 0;
	BFTAG bf;
	tret[itret++] = tag;
	bf.Set(tag);
	while(itret1 < itret && itret < 150) { // solve each entry back
		USHORT x = tret[itret1], aig = 1; // first locate where x has been loaded
		if(0 && Op.ot) {
			EE.E("go back look for ");
			zpln.ImageTag(x);
			EE.E(" itret1=");
			EE.E(itret1);
			EE.E(" itret=");
			EE.Enl(itret);
		}
		for(int i = 0; i <= npas; i++)
			if(steps[i].On(x)) {
				aig=0;
				if(i)   // not initial assumption
					if(!tsets[x]) { // this is a direct step
						USHORT z = 0;
						for(int i2 = 0; i2 < i; i2++)
							for(int j = 0; j < itx[i2]; j++) { // look for the earliest  possible parents
								// take in priority one already there  
								USHORT y = tx[i2][j]; 
								if(to[y].On(x)) {
									if(!z)
										z = y;
									if(bf.On(y)) {
										z = y;
										break;
									}
								} 
							}
							if(z && bf.Off(z)) {
								tret[itret++] = z;
								bf.Set(z);
							}
					}
					else {  // it comes from a set, we know which one
						const SET &chx = zcx.zc[tsets[x]];
						if(0) {
							EE.E("set");
							chx.Image();
							EE.Enl();
						}
						int n = chx.ncd;
						if(chx.type == SET_set)
							n--; // for a set, forget the event
						for(int j = 0; j < n; j++) {
							USHORT y = chx.tcd[j] << 1;
							if(y == x)
								continue;
							y ^= 1;
							if(bf.Off(y)) {
								tret[itret++] = y;
								bf.Set(y);
							}
						}
					}
					i = 100;// force end of process
			}  
			if(aig || itret > 150)
				return 0; // not found, should never be
			itret1++;
			if(0 && Op.ot) {
				EE.E("go back end step   itret1=");
				EE.E(itret1);
				EE.E(" itret=");
				EE.Enl(itret);
			}
	}
	if(pr && Op.ot) {  // printing in increasing order of generation
		EE.E("seq ");
		for(int i = 0; i <= npas; i++)
			for(int j = 0; j < itret; j++) 
				if(steps[i].On(tret[j])) {
					zpln.ImageTag(tret[j]);
					EE.Esp();
				}
				EE.Enl();
	}
	return itret;
}








