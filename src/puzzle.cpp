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

#include "flog.h"				// relay for printing only in tests
#include "opsudo.h"				// storing and managing options
#include "puzzle.h"				// general class to solve a puzzle
#include "utilities.h"

const int chx_max =9; //max size  for an event set could be a variable parameter?
                   // limit could be 20 in zcx, but must be checked to go over 9

const char *orig[]={"row ","column ","box "," "};
const char *lc="ABCDEFGHI";
const char *orig1="RCB ";

#include "puzzle_ww.cpp"
#include "puzzle_globals.cpp"

GG::GG() {	// constructor
	pg = g[0]; 
	pg[81] = 0;
}

int GG::NBlancs() const {
	int i, n = 0;
	for(i = 0; i < 81; i++)
		if(pg[i] == '0')
			n++;   
	return n;
}

int GG::Nfix() const {
	int i, n = 0; 
	for(i = 0; i < 81; i++) 
		if(pg[i] - '0')
			n++;   
	return n;
}

void GG::Image(char * lib) const {
	EE.E(lib); 
	EE.Enl(); 
	char wc[10];
	for(int i=0;i<9;i++) {
		strncpy_s(wc, 10, g[i], 9);
		wc[9] = 0;
		EE.E(i + 1);
		EE.E("=");
		EE.E(wc);
		EE.Enl();
	}
}


PUZZLE::PUZZLE() {
	tp8N.SetParent(this,&EE);
	yt.SetParent(this,&EE);
	zpaires.SetParent(this);
	tchain.SetParent(this,&EE);
	//un_jeu.SetParent(this,&EE);
	solution = gsolution.pg;  
	T81 = &tp8N;
	T81C = &tp8N_cop;
	T81t = T81->t81;
	T81tc = T81C->t81;
}

/* added here routines preliminary in Bitfields
   images
   */
void PUZZLE::ImagePoints(BF81 & zz) const {
	char s0[5], s1[5];
	int ns = 0, mode = 0;
	for(int i = 0; i < 81; i++) {
		if(zz.On(i)) {
			strcpy_s(s1, 5, t81f[i].pt);
			switch (mode) {
				case 0:
					mode = 1;
					break;
				case 1: 
					if(s0[1]==s1[1]) {  // ligne r1c12 on empile colonnes
						EE.E(s0);
						EE.E(s1[3]);
						mode = 2;
					}
					else if(s0[3] == s1[3]) { // colonne r12c1
						EE.E(s0[0]);
						EE.E(s0[1]);
						EE.E(s1[1]);
						mode = 3;
					}
					else
						EE.E(s0);
					break;
				case 2:
					if(s0[1]==s1[1])EE.E(s1[3]);  //suite ligne
					else mode = 1;  break;
				case 3:
					if(s0[3]==s1[3]) EE.E(s1[1]);//suite colonne  insertion ligne
					else {mode = 1; EE.E(&s0[2]);}
					break;
			}  // end switch
			strcpy_s(s0, 5, s1);
		}  // end if(On(i))
	}
	switch (mode) { // finir le traitement
		case 1:
			EE.E(s0);
			break;
		case 3:
			EE.E(&s0[2]);
			break;
	} // end switch
}

//------
void PUZZLE::ImageCand(BFCAND &zz, char *lib) const {
	if(!Op.ot)
		return;
	EE.E(lib);   
	for(int i = 1; i < zpln.ip; i++) {
		if(zz.On(i)) {
			zpln.Image(i);
			EE.Esp();
		}
	}
	EE.Enl();
}



//------
void PUZZLE::GetCells(BFCAND & zz,BF81 &cells) const {
	for(int i = 1; i < zpln.ip; i++) {
		if(zz.On(i)) {
			cells.Set(zpln.zp[i].ig);
		}
	}
	EE.Enl();
}




//------
void PUZZLE::Image(const BFTAG & zz,char * lib, int mmd) const {
	if(!Op.ot)
		return;
	EE.E(lib);   
	if(mmd)
		zpln.ImageTag(mmd);
	EE.E(" : ");
	for(int i = 2; i <col ; i++) {
		if(zz.On(i)) {
			zpln.ImageTag(i);
			EE.Esp();
		}
	}
	EE.Enl();
}

void PUZZLE::Elimite(char * lib)
   {cerr << "stop elimite"<<endl;
	 stop_rating=1;
    if(!Op.ot) return;
	EE.Enl2();
	EE.E("table:"); 
	EE.E(lib); 
	EE.E("limite atteinte ");
	EE.Enl2(); }

void PUZZLE::Estop(char * lib)
   {cerr << "stop estop"<<endl;
    stop_rating=1;
    if(!Op.ot) return;
	EE.Enl();
	EE.Enl(lib); 
	EE.Enl(); }

int PUZZLE::is_ed_ep()   // at the start of a new cycle
	{
		if(cycle<2) return 0;
		c_ret = Is_ed_ep_go();
		return c_ret;
	}
void PUZZLE::SetEr()   // something found at the last difficulty level  
	{
		if((cycle==1)&& difficulty>edmax) edmax=difficulty;
		if(((!assigned)|| (!epmax))&& difficulty>epmax) epmax=difficulty;
		if(difficulty>ermax) ermax=difficulty;      
	}
void PUZZLE::Seterr(int x){  // an error condition has been found
ermax=0; epmax=0;edmax=x;
// add a message to cerr in some debugging cases
cerr << "rating error  " << x << " cycle" << cycle << " dif" << difficulty << endl;

} 

// filter at the start of a new cycle

int PUZZLE::Is_ed_ep_go()  // is the ed or  condition fullfilled
{switch(Op.o1)
  {case 0: return 0;       // nothing to do
 
     // if -d command, other filters ignored  
   case 1: if((ermax- edmax)>Op.delta)  
		      {ermax=0;epmax=0;return 1;}  // not diamond
		   return 0;// continue if still ed


     // if -p command, similar results
   case 2: if(!assigned) return 0;   // -p command
	       if((ermax- epmax)>Op.delta)
		        {ermax=0;return 1;} // not pearl
	       return 0;// continue if still ep
  }	
	
     // now, we have no -d no -p command but at least one filter is on	
     // give priority to the -n() command

if(Op.filters.On(3) )// -n() command
	if(ermax >= Op.miner) {ermax=0;return 3;} // finished
      else if(cycle>Op.edcycles) return 4;

	  //then all max conditions
if(edmax>=Op.maxed || epmax>=Op.maxep || ermax>=Op.maxer)
	     {ermax=0;return 3;} // finished

     // and finally min ED and min EP
if(edmax<=Op.mined)  {ermax=0;return 3;} // finished
if(assigned && epmax<=Op.minep)  {ermax=0;return 3;} // finished
if(!Op.os)return 0; // finish with split ok

     // that sequence should work for any combinaison of filters.
if((Op.filters.f&7) ==1) return 4; // ed ok for split   	
if(assigned && ((Op.filters.f&6) ==2))  return 4; // ep ok for split   	

return 0;}

// the next routine is called at each step of the process
// the searched rating is stored in "difficulty"
// a check of filters is made and if any special filter is active,
// the process is cancelled
// return code 0 nothing special
//             1 process cancelled
//             2 ignore (er<x.y) these routines

void PUZZLE::Step(SolvingTechnique dif) {   // analyse what to do at that level
	rating_ir = 0;
	difficulty = dif;
	if(Op.o1 < 2)
		return; //nothing to do for -d command
	// if -p command, stop if we pass maxep
	if(Op.o1 == 2) {
		if(assigned && difficulty > Op.maxep) {
			ermax = 0;
			rating_ir = 1;
			return;
		} 
		else
			return;
	}
	// now other special filters 
	// -n() active if we pass the limit
	if(Op.filters.On(3)) {    // -n() command
		if(difficulty >= Op.miner) {
			ermax = 0;
			rating_ir = 3;
			return;
		}  // finished
	}
	if(difficulty >= Op.maxer) {
		ermax = 0;
		rating_ir = 2;
		return;
	} // filter on maxer cancelling high rating

	/*
	if(difficulty>maxed || difficulty>maxep ){maxer=0; ir=1;return;}
	*/
}


void PUZZLE::Copie_T_c() {
	tp8N_cop = tp8N;
	for(int i = 0; i < 9; i++)
		c_cop[i]=c[i];
}

void PUZZLE::Actifs() {
	zactif.SetAll_0();
	T81->Actifs(zactif);
	//for(int i = 0; i < 27; i++)
	//	elza81[i] = divf.elz81[i] & zactif;
}

void PUZZLE::cInit(int un) {
	for(int i = 0; i < 9; i++)
		if(un)
			c[i].SetAll_1();
		else
			c[i].SetAll_0();
}

void PUZZLE::cFixer(int ich, int i8) {
	for(int i = 0; i < 9; i++)
		c[i].Clear(i8); // pas de candidat ici
	//c[ich].Clear(t81f[i8].z); //MD: replaced with -=
	c[ich] -= t81f[i8].z;
}   // ni en zone influence

void PUZZLE::cReport() {    // on charge cand de ztzch
	for(int i8 = 0; i8 < 81; i8++) {
		CELL *p8 = &tp8N.t81[i8];
		if(p8->v.typ)
			continue;
		p8->v.ncand = 0;
		p8->v.cand.f = 0;
		p8->scand[0] = 0;
		for(int i = 0; i < 9; i++)
			if(c[i].On(i8))
				p8->v.cand.Set(i);
		p8->v.ncand = p8->v.cand.CountEtString(p8->scand);
	}
}

//void PUZZLE::TReport() {    // on charge c de table en mode depart impose
//	for(int i = 0; i < 9; i++)
//		c[i].SetAll_0();
//	for(int i8 = 0; i8 < 81; i8++) {
//		CELL *p8 = &tp8N.t81[i8];
//		if(p8->v.typ)
//			continue;
//		for(int i = 0; i < 9; i++)
//			if(p8->v.cand.On(i))
//				c[i].Set(i8);
//		p8->v.ncand = p8->v.cand.CountEtString(p8->scand);
//	}
//}

int PUZZLE::Recale() {
	//cReport();
	nfix = 0;
	for(int i = 0; i < 81; i++) {
		fix[i] = '0';
		if(T81t[i].v.ncand == 0)
			return 0;
	}
	puz.alt_index.Genere();   
	for(int i = 0; i < 27; i++)
		for(int j = 0; j < 9; j++)
			if(puz.alt_index.tchbit.el[i].eld[j].n == 0)
				return 0;
	return 1;
}

int PUZZLE::Directs() { //en tete appliquer regle de base
	int ir = 0, i;
	for(i = 0; i < 81; i++) {
		if((!T81t[i].v.typ) && (T81t[i].v.ncand == 1)) {   // case 1 candidat
			FixerAdd(i, T81t[i].scand[0], 3);
			ir = 1;
		}
	}
	for(i = 0; i < 27; i++) {
		for(int j = 0; j < 9; j++) {  // chiffre une place
			if(puz.alt_index.tchbit.el[i].eld[j].n == 1) {
				int k = puz.alt_index.tchbit.el[i].eld[j].b.First(),
					i8 = divf.el81[i][k];
				if(!T81t[i8].v.typ)	{
					FixerAdd(i8,(char)('1' + j), i/9);
					ir = 1;
				}
			}
		}
	}
	return ir;
}

// doing assignments matching expected rating if any
//    LastCell=10,		 last cell in row column box
//    SingleBlock=12,    single box
//    Single_R_C=15,     single row column
//    NakedSingle=23,	 cell one candidate 

int PUZZLE::FaitDirects(int rating) {
	if(stop_rating)
		return 1; 
	int ir = 0;
	for(int i = 0; i < 81; i++) {
		char c1 = fix[i], c2 = fixt[i];
		if(c1 - '0') {     // donc fixée
			// filter on rating expected
			int ok = 0;
			CELL_FIX p = t81f[i];
			switch(rating) {
			case 10:
				if((divf.N_Fixes(gg.pg,p.el) == 8) 
					|| (divf.N_Fixes(gg.pg,p.pl+9) == 8)
					|| (divf.N_Fixes(gg.pg,p.eb+18) == 8))
					ok = 1;
				break;
			case 12:
				if(puz.alt_index.tchbit.el[p.eb + 18].eld[c1 - '1'].n == 1)
					ok = 10;
				break;
			case 15:
				if(puz.alt_index.tchbit.el[p.el].eld[c1 - '1'].n == 1)
					ok = 1;
				if(puz.alt_index.tchbit.el[p.pl + 9].eld[c1 -'1'].n == 1)
					ok = 1;
				break;
			case 23:
				if((gg.pg[i] == '0') && (T81t[i].v.ncand == 1))
					ok = 1;
				break;
			}
			if(ok)
				ir += FaitGo(i, c1, c2); 
		}
	}
	EE.Enl();
	if(ir)
		cReport();
	return ir;
}

int PUZZLE::FaitGo(int i, char c1, char c2) { // function also called if single forced
	EE.E(++assigned);
	EE.E(" ");
	EE.E(t81f[i].pt);
	EE.E("=");
	EE.E(c1);
	EE.E(" ");
	if(c2 < 4)
		EE.Enl(orig1[c2]);
	else EE.Enl(" assigned");
	if((solution[i] - c1) && (!stop_rating)) { // validite  fixation
		stop_rating=1;
		cerr << "stop fication invalide"<<endl;
		EE.E( "FIXATION INVALIDE");
		return 0;
	}
	T81->Fixer(c1 - '1', i, 1);
	gg.pg[i] = c1;
	return 1;
}

//----                     supprimer ch en elem sauf  pos
int PUZZLE::ChangeSauf(int elem, BF16 pos, BF16 chiffres) {
	int ir=0;
	for(int i = 0; i < 9; i++) {
		if(pos.On(i))
			continue;
		ir += T81t[divf.el81[elem][i]].Change(chiffres);
	}
	return ir;
}

//----                     garder  ch en elem   pos
int PUZZLE::Keep(int elem, BF16 pos, BF16 chiffres) {
	int ir=0;
	for(int i=0;i<9;i++) {
		if(pos.On(i))
			ir += T81t[divf.el81[elem][i]].Keep(chiffres);
	}
	return ir;
}


//------                  ou simple deux points quelconques
int PUZZLE::Keep(int ch1, USHORT p1, USHORT p2) {
	BF81 ze = t81f[p1].z & t81f[p2].z & c[ch1];
	if(ze.IsNotEmpty())
		return T81->Clear(ze,ch1);
	return 0;
}

//---------
int PUZZLE::NonFixesEl(int el) {
	int n = 0;
	for(int i = 0; i < 9; i++)
		if(gg.pg[divf.el81[el][i]] == '0')
			n++;
	return n;
}

//--------       verify that the puzzle is correct
//int PUZZLE::Check() { 
//	BF16 c[27];	// a 9 bitfield for each house
//	int i;
//	for(i = 0; i < 27; i++)
//		c[i].f = 0;
//	for(i = 0; i < 81; i++) {
//		int w = gg.pg[i]; 
//		CELL_FIX w8 = t81f[i]; // TO OPTIMIZE use a pointer to avoid copy of CELL _FIX
//		if((w < '1') || (w > '9'))
//			continue;	// empty cell
//		w -= '1';
//		if(c[w8.el].On(w))
//			return 0;	// row
//		c[w8.el].Set(w);
//		if(c[w8.pl+9].On(w))
//			return 0;	// column
//		c[w8.pl+9].Set(w);
//		if(c[w8.eb+18].On(w))
//			return 0; // box
//		c[w8.eb+18].Set(w);
//	}
//	return 1;
//}

int PUZZLE::CheckChange(int i, int ch) {
	if(stop_rating) return 1;
	if(solution[i]-(ch + '1'))
		return 0;
	cerr << "stop check change"<<endl;
	stop_rating = 1;
	EE.E("ELIMINATION INVALIDE ");
	EE.E(ch+1);
	EE.Enl(t81f[i].pt);
	return 1;
}

void PUZZLE::PointK() {
	couprem++;
	EE.E( "CREM=" );
	EE.E(couprem );
}




//former _12a_PUZZLE_ChainesNested.cpp follows

/* comments on the process
level 2 base 9.5   Forcing chain authorized
level 3 base 10.0  lev 2 + multiple chains authorized

*/

//==============================================
// 95 Quick is a partial processing for nested + forcing chains
//    and nested + multi chains
// can be used to reduce the number of puzzles when looking for hardest puzzles

void PUZZLE::InitNested() { // common part before starting nested processing
	// lock the start situation with fully expanded  hdp from step 90
	if(Op.ot) {
		EE.Enl("Init  nested levels  ");
	}
	opp=0;
	zcf.h.dp=zcf.hdp_dynamic; // restore the index in zcf 
	zcf.ExpandAll();// not derived weak links
	zcf.LockNestedOne();
	zcx.LockNestedOne();
	zcxb.LockNestedOne();
	//zcf.h_one.d.Image();
}

/// en cours travail

int PUZZLE::Rating_baseNest(USHORT base, int quick) {
	tchain.SetMaxLength(base);
	rbase=base;
	opdiag=0;
	if(Op.ot) {
		EE.E("start  nested levels base =");
		EE.Enl(base);
		if(quick)
			EE.Enl("Quick mode is On");
		else
			EE.Enl("Quick mode is Off");
		if(rbase>90){
			PointK(); // milestone for debugging
			EE.Enl();
	    }
	}


	d_nested=d_nested2=zcf.h_one.d;
	for(int i = 2; i < col; i++) {
		 
		// first step in the search for nested chains
        // fill zcf.h_nest.d.t[i]  with the full expansion
        // in that mode

		GoNestedTag(i);
	}   

	if(0 &&rbase>100 && Op.ot){
		EE.Enl("full expansion at the start");
        d_nested.Image();
	}

	// we have now fully expanded tags in d_nested2 or d_nested
	// we look for potential eliminations

	Rbn_Elims(d_nested2.t);
	if(rbn_elimt.IsEmpty())
		Rbn_Elims(d_nested.t);

	if(rbn_elimt.IsEmpty())
		return 0;

	/// here a test to locate discrepancies with final 
	  // produce a detailed analysis for first case in dual mode 
	// test neutral after bug fixed
	if(0 && rbn_it2) { // some case 2 to apply

		opdiag=1;

	    d_nested=d_nested2=zcf.h_one.d; // restore the start

		int i=rbn_t2[0];
		GoNestedTag(i);
		GoNestedTag(i^1);
		opdiag=0;
	}


	// if we are in quick mode, set all elims with rating base+ .3/.5
	// process as quick mode if achieved rating is high enough
	if(quick || (base+10+(base-95)/5)<ermax) {
		Image(rbn_elimt,"quick elim potential", 0);
		int j = 3;
		for(int i = 3; i < col; i += 2) { // first in tchain mode
			if(rbn_elimt.On(i)) {
				j = i;
				tchain.LoadChain(base + 3, "quick nested ", i >> 1);
				rbn_elimt.Clear(i);
				break;
			}
		}
		for(int i = j + 2; i < col; i += 2) { // others in direct mode
			if(rbn_elimt.On(i)) {
				zpln.Clear(i >> 1);
			}
		}
		return Rating_end(200);
	}
	// not quick mode, go step by step to find the lowest rating
	if(0 && Op.ot) {
		EE.E("action it2 =");
		EE.E(rbn_it2);
		EE.E(" itch =");
		EE.E(rbn_itch);
		EE.Enl(); 
	}



	if(rbn_elims1.IsNotEmpty()) {  // some case 1 to apply
		for(int i = 2; i < col; i += 2) {
			if(rbn_elims1.On(i)) {
				GoNestedCase1(i >> 1);
            if(stop_rating) return 1;// push back error code
			}
		}
	}

	if(rbn_it2) { // some case 2 to apply
		for(int i = 0; i < rbn_it2; i++) {
			BFTAG * ptg = & rbn_elimst2[i];
			USHORT ttt[] = {rbn_t2[i], rbn_t2[i] ^ 1};
			if(Op.ot && rbase>100){
				EE.Enl("call rat nest a ~a for");
				zpln.ImageTag(ttt[0]);EE.Esp();
				zpln.ImageTag(ttt[1]);EE.Enl();
				Image (rbn_elimst2[i],"potential",0);
				EE.Enl();
			}

			for(int j = 3; j < col; j++) {
				if(ptg->On(j)) {   ///

					Rating_Nested( ttt, 2 ,j);
                if(stop_rating) return 1;// push back error code

				}
			}
		}
	}

	if(rbn_itch) { // some case 3 through sets to apply
		for(int i = 0; i < rbn_itch; i++) {
			BFTAG * ptg = & rbn_tchte[i];
			SET chx = zcx.zc[rbn_tch[i]];
			if(rbase>100 && Op.ot) {
				EE.E("action itch pour i =");
				EE.E(i);
				EE.E(" set=");
				EE.E(rbn_tch[i]);
				chx.Image();
				EE.Enl(); 
			}
			int n = 0, nni = chx.ncd; 
			USHORT ttt[20];
			for(int j = 0; j < nni; j++)
				ttt[j] = chx.tcd[j] << 1;
			for(int j = 3; j < col; j++) {
				if(ptg->On(j)) {
					Rating_Nested( ttt, nni, j);
                if(stop_rating) return 1;// push back error code

				}
			}
		}
	}
	return Rating_end(200);
}

/* partial process for Rating_baseNest
   done either using d_nested2 or d_nested
   if d_nested gives results, take it
*/

void PUZZLE::Rbn_Elims(BFTAG * tsquare){
	// cases 1 and 2,{ a=>x a=> ~x } { x => ~a and ~x=> ~a}
	// case 2 includes the so called cell or region eliminations for bi values
	rbn_d_nested=tsquare;
	rbn_elims1.SetAll_0();
	rbn_elims2=rbn_elims3=rbn_elims1;
	rbn_it2=0;
	for(int i = 2; i < col; i += 2) {
		BFTAG tw (tsquare[i]); // full expansion for the tag
		BFTAG tw1 (tw);
		tw1 &= tw.Inverse();
		BFTAG tw2 ( tw);
		tw2 &= tsquare[i ^ 1];
		tw2 = tw2.FalseState();
		tw1 = tw1.TrueState();
		if(tw2.IsNotEmpty()) {  // case 2
			rbn_elimst2[rbn_it2] = tw2;
			rbn_t2[rbn_it2++] = i;
			rbn_elims2 |= tw2;
		}
		if(tw1.IsNotEmpty()) {  // case 1
			rbn_elims1.Set(i);
		}
	}

	// case 3, is there a set killing "a"
	rbn_itch = 0;
	for(int ie = 1; ie < zcx.izc; ie++) {
		SET chx = zcx.zc[ie];
		int n = 0, nni = chx.ncd; 
		if(chx.type - SET_base)
			continue;
		BFTAG bfw;
		bfw.SetAll_1();
		for(int i = 0; i < nni; i++)
			bfw &= tsquare[chx.tcd[i] << 1];
		if(Op.ot && 0) { //puz.couprem ==5)
			chx.Image();
			Image(bfw,"communs",0);
		}
		bfw = bfw.FalseState();	 
		if(bfw.IsNotEmpty()) {
			if(0&&Op.ot && rbase>100){
                chx.Image(); 
			    Image(bfw," actif for ",0);
			}
			rbn_tchte[rbn_itch] = bfw;
			rbn_tch[rbn_itch++] = ie;
			rbn_elims3 |= bfw;
		}
	}// end case 3

	rbn_elimt = rbn_elims1.Inverse();
	rbn_elimt |= rbn_elims2;
	rbn_elimt |= rbn_elims3;
	if(Op.ot && rbn_elimt.IsNotEmpty() ){ 
		EE.Enl("summary of first phase");
		EE.E("itch ="); EE.E(rbn_itch);
		EE.E(" it2 ="); EE.Enl(rbn_it2);
		Image(rbn_elimt,"elimt potential", 0);
		Image(rbn_elims1.Inverse(),"elims1 potential", 0);
		Image(rbn_elims2,"elims2 potential", 0);
		Image(rbn_elims3,"elims3 potential", 0);
	}


}


//former _12a_PUZZLE_Chaines.cpp follows
/* main routine to process chains
   entry at a specific basic value for min rating
   process is cancelled as soon as a solution is found with a shorter rating
   all chain with the minimum rating  are stored
*/
// chain call 1=biv 2= cell_bivalue 4=nishio 8 dynamic 16=multi_chain

void PUZZLE::Chaining(int opt, int level, int base) {
	TaggingInit();
	tchain.SetMaxLength(base);
	rbase=base;
	// long tta,ttc;// provisoire, pour test de temps
	int ir = 0;   
	if(Op.ot) {
		EE.E("entree chaining opt=");
		EE.E(opt);
		EE.E(" base=");
		EE.E( base);
		EE.E(" level=");
		EE.Enl( level);
	}

	if((opt&3) == 2) {  // "Y" mode slightly different
		zpln.CellStrongLinks();
		zpln.RegionLinks(0);
	}
	else {
		if(opt&2)
			zpln.CellLinks(); 
		zpln.RegionLinks(opt&1);
	}

	if(opt & 4) { // dynamic mode 
		if(opt&2)zpln.GenCellsSets(); 
		zpln.GenRegionSets();  
	}


	switch(base) {
		case 65:
		case 70:    // same process for X;Y;XY loops and chains
			zcf.Aic_Cycle(opt & 3);     // several tags
			break;
		case 75: //  once derived weak links from directs
			zcf.hdp_base=zcf.hdp_dynamic=zcf.h.dp; // save basic weak links
	        zcf.ExpandShort(3);
            zcf.DeriveCycle(3, 9, 0,3);
	        ChainPlus();
            if(tchain.IsOK(77)) break;   // most often very short
         	while(zcf.DeriveCycle(3, 9, 0))
      	    	;// and full expansion
	        ChainPlus();

			break;
	}
}

//========= end of a rating step, check if done or still to be searched

   /* next is the next rating to be searched.
      the search is over if a <= rating has been found.
      likely <= next+1 due to the minimum length adjustment 
      except for aligned triplet */


int PUZZLE::Rating_end(int next) {
	   // if some immediate action done, answer yes imediatly)
	   // nothing to do for filters
	if(stop_rating) return 1;// push back error as soon as possible
	if(tchain.elims_done) 		return 1;
	if(!tchain.IsOK(next))
		return 0;
	Step((SolvingTechnique)tchain.rating);
	if(rating_ir)
		return 0; 
	return tchain.Clean();
}

void PUZZLE::TaggingInit() {
	zgs.ReInit();   // 81 bits patterns in use 
	zcf.Init();     // elementary weak links
	zcx.Init();     // sets (choices) in use
	// tevent.Init();   // event
}

// chain call 1=biv 2= cell_bivalue 4=nishio 8dynamic 16=multi_chain
//==============================================
// this includes the search for x or y cycle
int PUZZLE::Rating_base_65() {
	Chaining(1,0,65);// x cycle  
	if(Rating_end(65))
		return 1;
	Chaining(2, 0, 65);// y cycle  
	return Rating_end(70);
} 

//=============================================
// this includes the search for xy cycle or chain
int PUZZLE::Rating_base_70() {
	Chaining(3, 0, 70); // xy  forcing chain or loop
	return Rating_end(75);
} 

//==============================================
// this does not includes the search for aligned triplet 
// only th search for nishio
int PUZZLE::Rating_base_75() {
	Chaining(1 + 4, 0, 75); 
	return Rating_end(200);
} 

//==============================================
// 80 is multi chains
// we enter now a new group of eliminations. all the work is done using
// all bi values, basic weak links, basic sets
int PUZZLE::Rating_base_80() {
	if(Op.ot)
		EE.Enl("start rating base 8.0 multi chains");
	TaggingInit();
	zpln.CellLinks();
	zpln.RegionLinks(1);
	zpln.GenCellsSets();
	zpln.GenRegionSets();
	zcf.hdp_base=zcf.hdp_dynamic=zcf.h.dp; // save basic weak links
	tchain.SetMaxLength(80);
	rbase=80;
	zcf.ExpandAll();
	if(zcx.Interdit_Base80())
		return 1;; 
	return Rating_end(200);
} 


/* 85 new process
   expand completely the tags, but try to do it to catch the shortest
   first steps are not standard
   finish with standard expansion
   then look for elimintaions
   */
//==============================================
// 85 is DynamicForcingChain


int PUZZLE::Rating_base_85() {
	if(Op.ot)
		EE.Enl("start rating base 8.5 dynamic forcing chain");
	tchain.SetMaxLength(85);
	rbase=85;
	zcf.h.dp=zcf.hdp_base; // restore the  basic weak links
//	zcx.DeriveDirect();  // start with only equivalence to pointing claiming
    zcf.ExpandShort(5);
	zcf.DeriveCycle(3, 4, 0,4); // one cycle short sets
	zcf.DeriveCycle(3, 4, 0,4); // one cycle short sets
	ChainPlus();
	if(tchain.IsOK(88))      //filter  short paths
        return Rating_end(200);
	zcf.DeriveCycle(3, 5, 0,4); // one cycle short sets
	zcf.DeriveCycle(3, 5, 0,4); // one more cycle 
	ChainPlus();
	if(tchain.IsOK(90))      //filter  short paths
        return Rating_end(200);

	while(zcf.DeriveCycle(3, 9, 0))
		;// and full expansion
	ChainPlus();
	return Rating_end(200);
}


/* as in dynamic forcing chains,
   first a full controlled expansion
   then the search for the shortest
   */
//==============================================
// 90 is DynamicForcingChainPlus
// all "events"  claiming, pointing, pair, hidden pair, XWing
// follows an empty step 85
// consider each false candidate as start
// search for new bi values. If none, skip it
// look for new false thru basic sets
int PUZZLE::Rating_base_90() {
	if(Op.ot)
		EE.Enl("start rating base 9.0 dynamic forcing chains plus");
		
	tchain.SetMaxLength(90);
	rbase=90;
	zcf.h.dp=zcf.hdp_base; // restore the index in zcf  
	tevent.LoadAll();
	zcf.hdp_dynamic=zcf.h.dp; // store it for next steps

	zcf.h.d.ExpandShort(zcf.h.dp, 2);
	zcf.DeriveCycle(3, 4, 7, 2); // one cycle;
	ChainPlus();
	if(tchain.IsOK(92))      //filter  short paths
        return Rating_end(200);
	zcf.DeriveCycle(3, 7, 7, 5); // one cycle;  
	ChainPlus();
	if(tchain.IsOK(94))      //filter  short paths
        return Rating_end(200);
	if(Op.ot)
		EE.Enl("rating  dynamic forcing chains plus empty after 2 cycles");
	while(zcf.DeriveCycle(3, 9, 7))
		;
	ChainPlus();
	return Rating_end(200);
}



/* looking for elimination in dynamic mode
   in SE, such eliminations are always worked out of 2 chains
   analyzed from the left to the right.

   That process works in the same way, so elimination is one of the followings

   a-> b  and a -> ~b  (the classical  form a->~a is included here)
   x-> ~a and ~x -> ~a   
   region or cell multi chain  a -> {~x ~y ~z);

   the process has been reshaped to fit with search at higher levels
   */

void PUZZLE::ChainPlus() {


	int godirect=((rbase +8)<=ermax);


	BFTAG *t = zcf.h.d.t, *tp = zcf.h.dp.t; 
	for(int i = 2; i < puz.col; i += 2) {
		int icand=i>>1;
		if(dynamic_form1.Off(i)){  
		   BFTAG zw1 = t[i];
		   zw1 &= (t[i].Inverse()).TrueState();
		    if(zw1.IsNotEmpty()) { // this is a a-> b  and a -> ~b
		        dynamic_form1.Set(i); 
				if(1 && Op.ot) {
				   EE.E("\n\nfound active a->x and a->~x  a=" );
				   zpln.ImageTag(i);
				   EE.Enl();
	    		   //puz.Image(zw1," elims",i);
			    }           
           if(godirect) 
			   tchain.ClearImmediate(icand);
		   else
			   GoNestedCase1(icand); 
			}

		}

		

		BFTAG zw2 = t[i];
		zw2 &= t[i ^ 1];
		zw2 = zw2.FalseState();
		zw2-=dynamic_form2[icand]; // less already processed in dynamic search


		// if we start from a bi value, SE does not say
		// ~x but goes immediatly to the bi-value  saving one step.
		// 
		if(zw2.IsNotEmpty()) { // this is x-> ~a and ~x -> ~a
			dynamic_form2[icand]|=zw2;  
			if(1 && Op.ot) {
				EE.E("\n\nfound active x->~a and ~x->~a");
				puz.Image(zw2,"elims", i);
			}
			if(godirect){
				for(int j = 3; j < puz.col; j += 2) if(zw2.On(j) )
					 tchain.ClearImmediate(j>>1);
			}
			else{
		      USHORT ttt[]={i,i^1};
              zcf.h.dp.Shrink(zw2,i);
			  Image(zw2,"elims solde", i);
              for(int j = 3; j < puz.col; j += 2) if(zw2.On(j) )	
				  Rating_Nested(ttt,2,j);
			}
			
		}// end if zw2
	} // end for i

	if(rbase < 85 || tchain.elims_done)
		return;

	godirect=((rbase +10)<=ermax);

	// now check multichains in a similar way but not for nishio
	int ie;
	for( ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		if(chx.type - SET_base)
			break; // only base sets can be used
		int n = chx.ncd;
		USHORT *tcd = chx.tcd,
			   ttt[20];
		BFTAG tbt, bfset;
		tbt.SetAll_1();
		tbt -=dynamic_sets[ie]; // already seen in dynamic mode
		for(int i = 0; i < n; i++){
			ttt[i]=(tcd[i] << 1);
			bfset.Set(ttt[i] ^ 1);
		}
		for(int i = 0; i < n; i++) {
			BFTAG tw = t[tcd[i] << 1];
			tw -= bfset;
			tbt &= tw;
		}
		if(tbt.IsEmpty())
			continue;

		dynamic_sets[ie] |= tbt;  


        for(int j = 3; j < puz.col; j += 2) if(tbt.On(j))	
			if(godirect)  
				tchain.ClearImmediate(j>>1);
			else
				Rating_Nested(ttt,n,j);

	}// end for ie


}

//! Search for aligned pair or aligned triplet
/**
	That proces is very close to SE's one.
	Find all cells with 2 or 3 candidates and 
	collect all cells seen by one of them (potential base cells)
	First choose 2 cells (in the same band)of the potential base cells.
*/
int PUZZLE::AlignedTripletN() {
	int debuga = 0;
	static const int combi32[3][2] = {{0,1},{0,2},{1,2}};
	BF81 z23,	// set of cells that have 2 or 3 candidates (could be excluding cells)
		zbase;	// set of cells that are visible by one potential excluding cells
	for(int i = 0 ;i < 81; i++) {
		CELL_VAR p = T81t[i].v;
		if(p.ncand < 2 || p.ncand > 3)
			continue;
		z23.Set(i); 
		zbase |= t81f[i].z;
	}
	zbase &= zactif; // possible base cells reduced to cells that have no value
	// loop on any combination of 2 potential base cells
	for(int i1 = 0; i1 < 80; i1++) {
		if(!zbase.On(i1))
			continue;
		// if((z23 & t81f[i1].z).Count()>=3) // optimisation non concluante 
		// il faudrait optimiser Count (qui balaye le tableau de bits !)
		for(int i2 = i1 + 1; i2 < 81; i2++) {
			if(!zbase.On(i2))
				continue;
			// if((z23 & t81f[i2].z).Count()>=3) // optimisation non concluante 
			// il faudrait optimiser Count
			BF81 basei(i1,i2), // first 2 base cells
				*zi1=&t81f[i1].z, 
				*zi2=&t81f[i2].z,
				zi1andi2=(*zi1) & (*zi2);
			if(zi1andi2.Count() < 2)
				continue; // must be same band
			// we need to add an other base cell + 2 excludig cells that are visible
			// by the 3 base cells => the 3 base cells must share a same band
			// this condition is not used by serate
			BF81 zi1ori2 = ((*zi1) | (*zi2)) - basei; // excluding already there
			BF81 z23ori1i2 = z23 & zi1ori2; // seen by at least one
			// z23ori1i2 is the "twinarea of serate"
			if(z23ori1i2.IsEmpty())
				continue; // need a third cell as base cell
			for(int i3 = 0; i3 < 81; i3++) { // try all possible third cell
				if(!z23ori1i2.On(i3))
					continue;
				if(i1 == 39 && i2 == 53 && i3 == 40) //TODO
					debuga = 1;
				BF81 basei3 = basei;
				basei3.Set(i3);	// set of 3 base cells
				BF81 z23f = (zi1andi2 & t81f[i3].z & z23) - basei3; // TODO verify but ' - basei3' is de trop
				if(z23f.Count() < 2)
					continue; // we need at least 2 potential excluding cells
				int z23fIndex[81];
				int z23fSize = z23f.String(z23fIndex);

				// TODO modify 

				// loop on permutation of potential candidate for the 3 base cells
				int iCell[3] = {i1, i2, i3};		// index of base cells 0-80
				int ch[3];				// candidate for the 3 base cells 8-0
				BF16 cands[3] = {T81t[i1].v.cand, T81t[i2].v.cand, T81t[i3].v.cand};	// set of candidates
				BF16 allowed[3];
				for(int k1 = 0; k1 < 3; k1++)
					allowed[k1].SetAll_0();
				// test all combination of candidates
				for(ch[0] = 0; ch[0] < 9; ch[0]++) {
					if(!cands[0].On(ch[0]))
						continue;
					for(ch[1] = 0; ch[1] < 9; ch[1]++) {
						if(!cands[1].On(ch[1]))
							continue;
						for(ch[2] = 0; ch[2] < 9; ch[2]++) {
							if(!cands[2].On(ch[2]))
								continue;
							bool allow = true;
							// verify if this combination is forbidden 
							// 2 base cells that are visible may not have the same candidate
							for(int combiIndex = 0; combiIndex < 3; combiIndex++) {
								int ch1 = ch[combi32[combiIndex][0]];
								int ch2 = ch[combi32[combiIndex][1]];
								if (ch1 == ch2) {
									int ic1 = iCell[combi32[combiIndex][0]];
									int ic2 = iCell[combi32[combiIndex][1]];
									if(T81t[ic1].f->ObjCommun(T81t[ic2].f)) {
										allow = false;
										break;
									}
								}
							}
							if(!allow)
								continue;
							// verify if the combination is forbidden by an excluding cell
							// the potential candidate clear all possibliites for the exlcuding cell
							// loop on potential excluding cell
							//for(int iec = 0; iec < 81; iec++) { //v0 by GP
							//	if(!z23f.On(iec))
							//		continue;
							//	BF16 cec = T81t[iec].v.cand;
							//	for(int ich = 0; ich < 3; ich++)
							//		cec.Clear(ch[ich]);
							//	if(cec.isEmpty()) {
							//		allow = false;
							//		break;
							//	}
							//}
							for(int ecIndex = 0; ecIndex < z23fSize; ecIndex++) { //v1 by MD
								BF16 cec = T81t[z23fIndex[ecIndex]].v.cand;
								for(int ich = 0; ich < 3; ich++)
									cec.Clear(ch[ich]);
								if(cec.isEmpty()) {
									allow = false;
									break;
								}
							}
							if(!allow)
								continue;
							// combination allowed => 3 candidates are allowed
							for(int cellIndex = 0; cellIndex < 3; cellIndex++)
								allowed[cellIndex].Set(ch[cellIndex]);
						} // end ch 3
					} // end ch 2
				} // end ch 1
				// verify for each potential candidates if there are no allowed possibility
				for(int ice2 = 0; ice2 < 3; ice2++) { // loop on base cells
					if(cands[ice2] == allowed[ice2])
						continue; // optimisation
					for(int icand = 0; icand < 9; icand++) { // loop on candidates
						if(!cands[ice2].On(icand))
							continue;
						if(!allowed[ice2].Off(icand))
							continue;
						// we have found one "aligned triplet exclusion"
						T81t[iCell[ice2]].Change(icand); 
						if(Op.ot) {
							EE.E(" aligned triplet exclusion for ");
							EE.E(icand+1);
							EE.E(t81f[iCell[ice2]].pt);
							EE.E(" using base cells ");
							EE.E(t81f[i1].pt);
							EE.E(" , ");
							EE.E(t81f[i2].pt);
							EE.E(" and ");
							EE.E(t81f[i3].pt);
							EE.E(" ; ");
							EE.Enl();
							EE.E(" using excluding cells ...");
							// TODO add set of excluding cells
							EE.Enl();
						}
						return 1;
					} // end icand
				} // end  ice2
			} // end for i3
		} // end for i2
	} // end for i1
	debuga = 1; //TODO
	return 0;
}

//similar process for aligned pair with 2 base cells only

//! Search for aligned pairs
int PUZZLE::AlignedPairN() {
	BF81 z2, zbase;
	// look for cells that have 2 candidates
	for(int i81 = 0; i81 < 81; i81++) {
		CELL_VAR p = T81t[i81].v;	// candidates of the cell
		if(p.ncand - 2)
			continue; // only cell with 2 candidates
		z2.Set(i81);			// put cell to set
		zbase |= t81f[i81].z;	// or of cells controled by these cells
	}
	zbase &= zactif;		// keep only cells that have not yet a value
	for(int i1 = 0; i1 < 80; i1++) {
		if(!zbase.On(i1))
			continue;
		for(int i2 = i1 + 1; i2 < 81; i2++) {
			if(!zbase.On(i2))
				continue;
			// a couple of cells  that see at minimum one bivalue cell
			BF81 
				basei(i1,i2),
				*zi1=&t81f[i1].z, // influence zone of first cell
				*zi2=&t81f[i2].z, // influence zone of second cell
				z2f=(*zi1) & (*zi2) & z2; // set of cell with 2 cand in both influence zones
			if(z2f.Count() < 2)
				continue;  
			// z2f is the set of excluding cells of serate
			// test combination of candidates for cell i1 and i2
			int ch1 = 0;					// candidate for cell i1
			BF16 cands1 = T81t[i1].v.cand;	// set of candidates
			int ch2 = 0;					// candidate for cell i2
			BF16 cands2 = T81t[i2].v.cand;	// set of candidates
			int excl1[9];	
			int excl2[9];
			for(int k = 0; k < 9; k++) {
				excl1[k] = excl2[k] = 0;
			}
			// test all combination of candidates
			for(ch1 = 0; ch1 < 9; ch1++) {
				if(!cands1.On(ch1))
					continue;
				for(ch2 = 0; ch2 < 9; ch2++) {
					if(!cands2.On(ch2))
						continue;
					// look if visible cells and same cand => excluding
					if(ch1 == ch2) {
						if(T81t[i1].f->ObjCommun(T81t[i2].f)) {
							excl1[ch1]++;
							excl2[ch2]++;
							continue;
						}
					}
					else {
						// look if there is an exclusion cell
						for(int i = 0; i < 81; i++) {
							if(!z2f.On(i))
								continue;
							if(T81t[i].v.cand.On(ch1) && T81t[i].v.cand.On(ch2)) {
								excl1[ch1]++;
								excl2[ch2]++;
								break;
							}
						}
					}
				}
			}
			// verify if there is an excluded candidate
			int n1 = T81t[i2].v.ncand;
			int n2 = T81t[i1].v.ncand;
			int cell8 = -1;
			int ch = -1;
			for(ch1 = 0; ch1 < 9; ch1++) {
				if(!cands1.On(ch1))
					continue;
				if(excl1[ch1] == n1) {
					cell8 = i1;
					ch=ch1;
					break;
				}
			}
			if(cell8 < 0) {
				for(ch2 = 0; ch2 < 9; ch2++) {
					if(!cands2.On(ch2))
						continue;
					if(excl2[ch2] == n2) {
						cell8 = i2;
						ch = ch2;
						break;
					}
				}
			}
			if(cell8 >= 0) { // we have found one pair exclusion
				T81t[cell8].Change(ch); // erase candidate
				if(Op.ot) {
					EE.E(" aligned pair exclusion for ");
					EE.E(ch+1);
					EE.E(t81f[cell8].pt);
					EE.E(" using base cells ");
					EE.E(t81f[i1].pt);
					EE.E(" and ");
					EE.E(t81f[i2].pt);
					EE.E(" ; ");
					EE.Enl();
					EE.E(" using excluding cells ");
					for(int i = 0; i < 81; i++) {
						if(z2f.On(i)) {
							EE.E(t81f[i].pt);
							EE.E(" ; ");
						}
					}
					EE.Enl();
				}
				return 1;
			}// ich
		} // i2
	} // i1
	return 0;
}







//former _03c_PUZZLE_traite_base.cpp follows

int PUZZLE::Traite(char * ze) {

	stop_rating = 0;
	cycle=assigned=c_ret=0;
	ermax=epmax=edmax=0;

	   // final location for the normalized puzzle in a global variable
	for(int i=0;i<81;i++) //get normalised puzzle in puz
		if(ze[i]-'.') gg.pg[i]=ze[i]; else  gg.pg[i]='0';

	// check if the puzzle is correct 
	//(no duplicate givens in a house)
	// one solution and only one

	//if ((!Check())     ||
	//	( un_jeu.Uniqueness(gg,& gsolution)-1)) {
	//		edmax=1;
	//		return 0;
	//}

	//MD: new brute force solver for validation
	//========
	char puz_zero_based[81];
	char solutions[2][81];
	for(int i = 0; i < 81; i++) {
		puz_zero_based[i] = (ze[i] >= '1' && ze[i] <= '9') ? ze[i] - '0' : 0;
	}
	if(1 != fsss(puz_zero_based, 2, solutions[0])) { //zero or more than one solutions
        edmax=1;
		return 0;
	}
	for(int i = 0; i < 81; i++) {
		solution[i] = solutions[0][i] + '0';
	}
	//========

		/* the solution is stored in an appropriate form 
	 * 9 81 bitfield indicating for each digit the positions
	 * to check later the validity of eliminations
	 * this is a debugging control
	 */
	//for(int i=0;i<9;i++) 
	//	csol[i].SetAll_0();
	//for(int i=0;i<81;i++) 
	//	csol[solution[i]-'1'].Set(i);


	//================== assign clues to start
	cInit(1);
	PKInit();
	//tdebut = GetTimeMillis();
	//long told = tdebut;
	T81->init();  //initial candidats au lieu de Traite_Init(); 
	for(int i = 0; i < 81; i++) { // first assignment from clues
		int w = gg.pg[i];
		if(w > '0' && w <= '9')
			T81->Fixer(w - '1', i, 2);
	}
	cReport(); // and preparing T81 from PM per digit

	EE.Enl(); // new line in case test is done

	//===========================================================
	while (cycle++ < 150) {
		if(cycle > 148) {
			Seterr(7);
			break;
		} // 
		if(stop_rating) {
			Seterr(5);
			break;
		} 

		int ir_ed = is_ed_ep();
		if(ir_ed > 2)
			return ir_ed;//split filter
		if(ir_ed)
			return 0; // pure ed ep filter
		if(!Recale()) {
			Seterr(6);
			break;
		}
		if(!gg.NBlancs())
			break; // finished
		// processing 1.0 to <6.2
		int ir_a = Traite_a();    

		if(!ir_a)
			return rating_ir;
		else if(ir_a < 2)
			continue;

		//  start of processes using the tagging infrastructure.
		// the table of candidates is loaded once per cycle

		zpln.Init();  // table of candidates  
		zgs.ReInit(); // 81 bits patterns in use      
		if(stop_rating)
			break; // in case we would exceed the TCAND limit
		//=========================
		tchain.NewCycle();

		Step(AlignedPairExclusion);
		if(rating_ir > 1)
			return rating_ir;
		else if(rating_ir)
			continue;
		if(AlignedPairN()) {
			SetEr();
			continue;
		}  //6.2

		if(ermax < 74) { // skip that if high ER already reached
			Step(AIC_X_cycle);
			if(rating_ir > 1)
				return rating_ir;
			else if(rating_ir)
				continue;
			if(Rating_base_65()) {
				SetEr();
				continue;
			}  //6.5 and 6.6 
			Step(AIC_XY);
			if(rating_ir > 1)
				return rating_ir;
			else if(rating_ir)
				continue;
			if(Rating_base_70()) {
				SetEr();
				continue;
			}  //70
		}
		else if(ermax < 77) { // just skip 6.5 6.6 as a first step
			if(Op.ot)
				EE.Enl("go direct to XY"); 
			Step(AIC_XY);
			if(rating_ir > 1)
				return rating_ir;
			else if(rating_ir)
				continue;
			if(Rating_base_70()) {
				SetEr();
				continue;
			}  //70
		}		
		else {
			if(Op.ot)
				EE.Enl("gofast");
			if(zcf.Fast_Aic_Chain())
				continue;
		} 

		Step(AlignedTripletExclusion);
		if(rating_ir>1)
			return rating_ir;
		else if(rating_ir)
			continue;
		if(AlignedTripletN()) {
			SetEr();
			continue;
		}  //7.5  

		  // at that point, we set to nill dynamic processing done
		  // this is done once for all the cycle
		dynamic_form1.SetAll_0();
		for(int i=0;i<320;i++){
			dynamic_form2[i].SetAll_0();
			dynamic_sets[i].SetAll_0();
		}

		if(Rating_base_75()) {  
			SetEr();
			continue;
		}  //7.5
		if(Op.oexclude-1) {
			Step(MultipleForcingChain);
			if(rating_ir > 1)
				return rating_ir;
			else if(rating_ir)
				continue;
			if(Rating_base_80()) {
				SetEr();
				continue;
			}  //8.0

			if(Op.oexclude-2) {	 	
				Step(DynamicForcingChain);
				if(rating_ir>1)
					return rating_ir;
				else if(rating_ir)
					continue;
				if(Rating_base_85()) {
					SetEr();
					continue;
				}  //8.5

				if(Op.oexclude-3) {	
					Step(DynamicForcingChainPlus);
					if(rating_ir > 1)
						return rating_ir;
					else if(rating_ir)
						continue;
					if(Rating_base_90()) {
						SetEr();
						continue;
					}  //9.0

					if(Op.oexclude-4) {
						InitNested(); 
						Step(NestedForcingChain);
						if(rating_ir>1)
							return rating_ir;
						else if(rating_ir)
							continue;
						if(Rating_baseNest(95, Op.oq)) {
							SetEr();
							continue;
						}  //9.5

						if(Op.oexclude-5) {	
							Step(NestedLevel3);
							if(rating_ir>1)
								return rating_ir;
							else if(rating_ir)
								continue;
							if(Rating_baseNest(100, Op.oq)) {
								SetEr();
								continue;
							}  //100
							if(Op.oexclude-6) {
								Step(NestedLevel4);
								if(rating_ir > 1)
									return rating_ir;
								else if(rating_ir)
									continue;
								if(Rating_baseNest(105, Op.oq)) {
									SetEr();
									continue;
								}  //105
							}// end level4
						}// end level 3 and more
					} // end level 2 and more
				} // end if >2
			} // end if >1
		} // end if oexclude
		if(Rating_end(200)) {
			SetEr();
			continue;
		} // clean the file
		if(Op.ot)
			T81->Candidats();
		stop_rating=2;
		break;
	}     
	//=========================================================================	 
	EE.E("fin traitement stop_rating=");
	EE.Enl(stop_rating );
	gg.Image("fin");
	return stop_rating;
}

int PUZZLE::Traite_a() {
	if (Directs ()) {
		Step(LastCell);
		if(rating_ir > 1)
			return 0;
		else if(rating_ir)
			return 1;
		if(FaitDirects(LastCell)) {
			SetEr();
			return 1;
		}   //1.0

		Step(SingleBox);
		if(rating_ir > 1)
			return 0;
		else if(rating_ir)
			return 1;
		if(FaitDirects(SingleBox)) {
			SetEr();
			return 1;
		}  //1.2

		Step(Single_R_C);
		if(rating_ir > 1)
			return 0;
		else if(rating_ir)
			return 1;
		if(FaitDirects(Single_R_C)) {
			SetEr();
			return 1;
		}  //1.5
	}
	Actifs(); // update of active cells must be done from here

	Step(Single_after_Locked);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(TraiteLocked(Single_after_Locked)) {
		SetEr();
		return 1;
	}  //1.7



	Step(HiddenPair_single);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(2,1,1)) {
		SetEr();
		return 1;
	}  //2.0

	if(Directs()) {
	  Step(NakedSingle);
	  if(rating_ir > 1)
		  return 0;
	  else if(rating_ir)
		  return 1;
	   if(FaitDirects(NakedSingle)) {
		   SetEr();
		   return 1;
	   }
	}  //2.3

    Step(HiddenTriplet_single);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(3,1,1)) {
		SetEr();
		return 1;
	}  //2.5

	Step(Locked_box);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(TraiteLocked(Locked_box)) {
		SetEr();
		return 1;
	}  //2.6

	Step(Locked_RC);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(TraiteLocked(Locked_RC)) {
		SetEr();
		return 1;
	}  //2.8

	Step(NakedPair);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(2,0,0)) {
		SetEr();
		return 1;
	}  //3.0

	Step(XWing);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.XW(2)) {
		SetEr();
		return 1;
	}  //3.2

	Step(HiddenPair);
	if(rating_ir > 1)return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(2,1,0)) {
		SetEr();
		return 1;
	}  //3.4

	Step(Naked_triplet);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(3,0,0)) {
		SetEr();
		return 1;
	}  //3.6

	Step(swordfish);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.XW(3)) {
		SetEr();
		return 1;
	}  //3.8

	Step(HiddenTriplet);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(3,1,0)) {
		SetEr();
		return 1;
	}  //4.0

    if(Op.ot)
		T81->Candidats();


    Copie_T_c(); // to be done now copie for UR same rating
    zpaires.CreerTable(T81t);  

	Step(XYWing);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(zpaires.XYWing()) {
		SetEr();
		return 1;
	}  //4.2

	Step(XYZWing);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(zpaires.XYZWing()) {
		SetEr();
		return 1;
	}  //4.4


	Step(UniqueRect1);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
	if(T81->RIN()) {
		SetEr();
		return 1;
	}  //4.5

	Step(UniqueRect2);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(urt.Traite(46)) {
		SetEr();
		return 1;
	}  //4.6 t
    if(zpaires.UL()) {
		SetEr();
		return 1;
	}    

	Step(UniqueRect3);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
	 if(urt.Traite(47)) {
		 SetEr();
		 return 1;
	 } //4.7
	 if(tult.Traite(47)) {
		 SetEr();
		 return 1;
	 } 

	Step(UniqueLoop1);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
	if(urt.Traite(48)) {
		SetEr();
		return 1;
	} //4.7
	if(tult.Traite(48)) {
		SetEr();
		return 1;
	}  //4.8
	                  	
	Step(UniqueLoop2);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
	if(tult.Traite(49)) {
		SetEr();
		return 1;
	}  //4.9

	Step(NakedQuad);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
 	if(tult.Traite(50)) {
		SetEr();
		return 1;
	}  //4.9
    if(yt.Tiroirs(4,0,0)) {
		SetEr();
		return 1;
	}  //5.0

	Step(UniqueLoop3);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
	if(tult.Traite(51)) {
		SetEr();
		return 1;
	}  //5.1
 
	Step(Jellyfish);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
	if(tult.Traite(52)) {
		SetEr();
		return 1;
	}  //5.2
    if(yt.XW(4)) {
		SetEr();
		return 1;
	}  //5.2

	Step(HiddenQuad);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(yt.Tiroirs(4,1,0)) {
		SetEr();
		return 1;
	}  //5.4

	Step(BUG);
	if(rating_ir > 1)
		return 0;
	else if(rating_ir)
		return 1;
    if(zpaires.BUG()) {
		SetEr();
		return 1;
	}  //5.6 a 5.8

	if(zpaires.aigun) {
		Step((SolvingTechnique)59);
		if(rating_ir > 1)
			return 0;
		else if(rating_ir)
			return 1;
		if(zpaires.Bug3a(59)) {
			SetEr();
			return 1;
		}  //5.9
		Step((SolvingTechnique)60);
		if(rating_ir > 1)
			return 0;
		else if(rating_ir)
			return 1;
		if(zpaires.Bug3a(60)) {
			SetEr();
			return 1;
		}  //6.0
		Step((SolvingTechnique)61);
		if(rating_ir > 1)
			return 0;
		else if(rating_ir)
			return 1;
		if(zpaires.Bug3a(61)) {
			SetEr();
			return 1;
		}  //6.1
	}
	return 2;
}

//former _04a_lock.cpp follows
// part of PUZZLE class methos processing locked candidates in a box, row,col

void messlock(int obj,int obj2,int ch) {
	if(!Op.ot)
		return;
	int it1 = obj2 / 9, it2 = obj / 9, ii1 = obj2 % 9, ii2 = obj % 9;
	char c1, c2;
	c1 = (it1 - 1) ? (char)(ii1 + '1') : lc[ii1];
	c2 = (it2 - 1) ? (char)(ii2 + '1') : lc[ii2];
	EE.E("-> ");
	EE.E(orig[it1]);
	EE.E(ii1 + 1);
	EE.E("  digit ");	
	EE.E(ch + 1);
	EE.E(" in ");
	EE.E(orig[it2]);
	EE.Enl(ii2 + 1);
}

/* rating difficulty linked to that include
    Single_after_Locked=17,		// locked in box  do only the fix
    Locked_box=26,				// locked in box  no fix
    Locked_RC=28,				// locked in row/col  no fix
  
*/

//<<<<<<<<<<<<<<<<<<<<
int PUZZLE::TraiteLocked(int rating) {
	if(rating == 26)
		return TraiteLocked2(18, 27); // box only no fix
	if(rating == 28)
		return TraiteLocked2(0, 18); // row col  no fix
	// now rating 1.7, process only boxes, 
	// must generate a new single in an attached box to elem2
	// clearing is done in the cell where the fix takes place
	int ir = 0;
	int ialt;
	for(int ich = 0; ich < 9; ich++) {
		BF81 wf = c[ich], wfel;
		for(int iel = 18; iel < 27; iel++) {
			wfel = divf.elz81[iel] & wf;
			if(wfel.IsEmpty())
				continue;
			if(divf.IsAutreObjet(wfel,iel,ialt)) {
				BF81 wa = wf&divf.elz81[ialt], wex = wa ^ wfel;
				if(wex.IsNotEmpty()) {
					// the search for singles is done only in boxes
					// intersecting with ialt , so it must be a hidden single
					int ok = 0;
					BF81 ww;
					for(int i = 18; i < 27; i++) { // must be a box
						if((i-ialt) && (divf.elz81[i] & divf.elz81[ialt]).IsNotEmpty()) {
							ww = (divf.elz81[i] & puz.c[ich]) - wex;
							if(ww.Count() == 1) {
								ok = 1;
								break;
							}
						}
					}
					if(ok) {  // clear others candidates in the cell to be fixed
						messlock(ialt,iel,ich);
						int i8 = ww.First();
						T81t[i8].Keep(ich);
						EE.Enl("lock assignment");
						return puz.FaitGoA(i8, ich + '1', 4);
					} // immediate return after assign  
				}
			}
		}
	}
	return ir;
}

//<<<<<<<<<<<<<<<<<<<<
int PUZZLE::TraiteLocked2(int eld, int elf) {
	int ir = 0;
	int ialt;
	for(int ich = 0; ich < 9; ich++) {
		BF81 wf = c[ich], wfel;
		for(int iel = eld; iel < elf; iel++) {
			wfel = divf.elz81[iel] & wf;
			if(wfel.IsEmpty())
				continue;
			if(divf.IsAutreObjet(wfel,iel,ialt)) {
				BF81 wa = wf&divf.elz81[ialt], wex = wa ^ wfel;
				if(wex.IsNotEmpty()) {
					messlock(ialt, iel, ich) ;
					T81->Clear(wex, ich);
					ir = 1;
					wf = c[ich] = wf ^ wex;
				}
			}
		}
	}
	return ir;
}

void TPAIRES::SetParent(PUZZLE * parent){
parentpuz=parent;
}



void TPAIRES::CreerTable(const CELL * tt) {
	ip = 0;
	ntplus = aigpmax = 0;
	zplus.SetAll_0();
	for(int i = 0; i < 81; i++) {
		int n = tt[i].v.ncand;
		if(n == 2) {
			zp[ip].Charge(tt[i]);
			zpt[ip] = zp[ip++];
		}
		else if(n > 2) {
			if(ntplus < 8) {
				tplus[ntplus] = i;//direct access to first plus cells
				ntplus++;
				zplus.Set(i);
			}
			if(n > aigpmax)
				aigpmax = n;
		}
	}
	// now sorting the table zpt for the search of UR/UL 
	for(int i = 0; i < ip - 1; i++) {
		for(int j = i + 1; j < ip; j++) {
			if(zpt[j].pa.f<zpt[i].pa.f ||(zpt[j].pa.f == zpt[i].pa.f && zpt[j].i8<zpt[i].i8)) {
				PAIRES temp = zpt[i];
				zpt[i] = zpt[j];
				zpt[j] = temp;
			}
		}
	}
	// and final entries in tp izpd izpf
	np = 0;
	if(!ip)
		return;
	tp[0] = zpt[0].pa;
	izpd[0] = 0;
	for(int i = 1; i < ip; i++) {
		if(zpt[i].pa.f == tp[np].f)
			continue;
		izpd[++np] = i;
		tp[np] = zpt[i].pa;
	}
	izpd[++np] = ip;
}
//====================================
int TPAIRES::UL() {
	int ir = 0;
	tult.Init();
	for(int i = 0; i < np; i++) {
		USHORT id = izpd[i], ie = izpd[i + 1];
		// EE.Enl("un depart paire");
		UL_SEARCH uls(tp[i], this, &zpt[id], ie - id); //build search 
		for(int j = id; j < ie - 1; j++) {
			for(int k = j + 1; k < ie; k++) {
				USHORT i1 = zpt[j].i8, i2 = zpt[k].i8;
				CELL_FIX p1 = t81f[i1], p2 = t81f[i2];
				if(!(p1.el==p2.el || p1.pl==p2.pl))
					continue; // start row or col
				//  EE.E(p1.pt); EE.E(p2.pt);EE.Enl("un depart lig/col");
				UL_SEARCH ulsn(&uls);
				ulsn.Set(i1);
				if(p1.el == p2.el)
					ulsn.el_used.Set(p1.el);  
				else
					ulsn.el_used.Set(p1.pl+9); 
				ir += ulsn.Add_Chain(i2);  // looking for all equivalent moves
			}// end k
		} // end j
	}// end i
	return ir;
}


//========================= insert a new cell after el_used correct
void UL_SEARCH::Set(int i8) { // el_used already ok if necessary
	cells.Set(i8);  
	CELL_FIX p = t81f[i8];
	last = i8; 
	CELL_VAR pv = T81tc[i8].v;
	parity.Inv(p.el);
	parity.Inv(p.pl + 9);
	parity.Inv(p.eb + 18);                   
	elcpt[p.el]++;
	elcpt[p.pl + 9]++;
	elcpt[p.eb + 18]++;
	cht |= pv.cand;
	int nc = pv.cand.QC(); 
	if(nc > 2)
		adds[nadds++] = i8;
	// EE.E("UL lc=");EE.E(line_count);EE.Esp();EE.Enl(p.pt);
	tcount[line_count++] = i8;
}

//====================================
// Check if the loop has more than 0 solutions by parity
bool UL_SEARCH::ParityCheck(void) {
	// Check for the "0 solutions" parity check
	unsigned int oddPos = 0, evenPos = 0;
	bool isOdd = false;
	for(int i = 0; i < (line_count - 1); ++i) {
		isOdd = !isOdd;
		unsigned int curRow = tcount[i]/9;
		unsigned int curCol = tcount[i] % 9;
		unsigned int curBox = I81::Boite(curRow, curCol);
		int newPos = 1 << curRow | 1 << (curCol + 9) | 1 << (curBox + 18);
		if(isOdd) {
			if(oddPos & newPos)
				return false;
			oddPos |= newPos;
		}
		else {
			if(evenPos & newPos)
				return false;
			evenPos |= newPos;
		}
	}
	if(oddPos != evenPos)
		return false;
	return true;
}


//============================= recurrent process to add next point
int UL_SEARCH::Add_Chain(int i8) {
	if(line_count > 20)
		return 0; // securite
	if(cells.On(i8)) { // terminé on élimine URs et fausses boucles
		//EE.E("UL end ");EE.E(t81f[i8].pt);EE.Esp();cells.ImagePoints();EE.Enl();
		if(line_count < 5 || (i8 - tcount[0]))
			return 0;  
		tcount[line_count++] = i8;
		return Loop_OK();
	}
	Set(i8);              // On met le point en place
	CELL_FIX f = t81f[i8];

	// a défaut une case avec additifs  ligne, puis col, puis bloc en paire
	// uniquement dans éléments non traités et si pas de double paire
	// not more than 3 adds except one digit
	if(nadds > 7 || (cht.QC() > 3 && nadds > 2))
		return 0; 
	if(El_Suite(f.el))
		return 1;
	if(El_Suite(f.pl + 9))
		return 1;
	if(El_Suite(f.eb + 18))
		return 1;
	return 0;
}

//====================================
int UL_SEARCH::El_Suite(USHORT ele) {
	if(el_used.On(ele))
		return 0;
	//EE.E("suite el=");EE.Enl(ele+1);
	BF16 wc = puz.alt_index.tchbit.el[ele].eld[c1].b & puz.alt_index.tchbit.el[ele].eld[c2].b;
	for(int i = 0; i < 9; i++) {
		if(wc.On(i)) { // cells with both digits
			int i8r = divf.el81[ele][i];
			//EE.E("essai i8=");EE.Enl(t81f[i8r].pt);
			if(ele > 17) { // in a box, only not row col
				CELL_FIX f = t81f[i8r], f2 = t81f[last];
				if(f.el == f2.el || f.pl == f2.pl)
					continue;
			}
			if(!Is_OK_Suite(i8r))
				continue;
			UL_SEARCH ulsn(this);
			ulsn.el_used.Set(ele); 
			if(ulsn.Add_Chain(i8r))
				return 1;
		}
	}// end for
	return 0;
}

//=================================================
int UL_SEARCH::Is_OK_Suite(USHORT i8) {
	if(i8 == last)
		return 0;
	if(i8 == tcount[0])
		return 1;
	if(cells.On(i8))
		return 0; // false loop  
	CELL_FIX f = t81f[i8]; 
	if(elcpt[f.el] > 1 || elcpt[f.pl + 9] > 1 || elcpt[f.eb + 18] > 1)
		return 0;
	// for the time being, I see no more exclusion
	return 1;
}
//  entry action 0 is the start 4.6.  store after
//       one digit in excess and 

//==================================================
int UL_SEARCH::Loop_OK(int action) {
	//UL_Mess("potential loop",1);
	if(parity.f)
		return 0; // must be even number everywhere
	if(!ParityCheck()) // check for more than 0 solutions
		return 0;
	if(!action) // split processing depending on size of the loop
		if(line_count>7) {
			tult.Store(*this);
			return 0;
		}
		else
			action++;
	// les deux ci-dessous sortent en 4.6 et 4.7; voir l'origine de l'écart (nb de pas???)
	if(action == 1 && nadds < 2) { //one cell with adds rating 4.6 revérifié, c'est bien 4.6
		USHORT iu = adds[0];
		if(T81t[iu].Change(chd)) {
			UL_Mess("one cell with extra digits ", 1);
			return 1;
		}
	} // nothing if redundancy with another loop

	// now one digit in excess ++++++++++++
	if(action == 1 && (cht.QC() == 3)) {
		BF81 zi;
		zi.SetAll_1();
		for(int i = 0; i < nadds; i++)
			zi &= t81f[adds[i]].z;
		if(T81->Clear(zi, (cht - chd).First())) {
			UL_Mess(" one digit in excess", 1);
			return 1;
		}         
	}

	// action 1, launching the process common to UR/UL
	if(nadds == 2) { // type 2;3;4 must be same object 
		if(!(t81f[adds[0]].ObjCommun(&t81f[adds[1]])))
			return 0;	
		int ir = ur.StartECbi(adds[0], adds[1], chd, action);
		if(ir == 1)
		{UL_Mess("action UL / 2 cells)", 1);
		return 1;
		}   
	}
	// store it if action 0 
	if(action < 2) {
		tult.Store(*this);
		return 0;
	}

	//UL_Mess("nothing",1);
	return 0;
}
//-----  
void UL_SEARCH::UL_Mess(char * lib,int pr) { // et flag des "faits"
	EE.Enl();
	EE.E("UL loop nadds=");
	EE.E(nadds);
	EE.E(" count=");
	EE.E(line_count - 1);
	EE.E(" rating=");
	EE.E(puz.difficulty);
	for(int i = 0; i < line_count; i++) {
		EE.Esp();
		EE.E(t81f[tcount[i]].pt);
	}
	EE.Enl();
	if(pr) {
		EE.E("active due to " );
		EE.Enl(lib);
	}
}
 
//former _04d_paires_bug.cpp follows
/* searching and processing all forms of bugs included in SE
   type 1 one extra cell  5.6
   type 2 2 extra cells with one same extra digit  5.7
   type 3 extra cells combined to a naked duo  5.8
   type 3 extra cells combined to a naked triplet 5.9
   type 3 extra cells combined to a naked quad 6.0 
   type 3 extra cells combined to a naked (5) 6.1
   type 4 extra cells (2) have a locked digit 5.7   */
//==============================
int TPAIRES::BUG() {
	EE.Enl("debut recherche bug");
	aigun = 0;
	if(ntplus > 6 || aigpmax > 4)
		return 0;  // maxi a vérifier 6 cases et 4 candidats
	// set the parity of digits for bivalue cells in all elements
	for(int i = 0; i < 27; i++)
		el_par_ch[i].f = 0;
	for(int i = 0; i < ip; i++) {
		CELL p = T81t[zp[i].i8];  
		el_par_ch[p.f->el] ^= p.v.cand; 
		el_par_ch[p.f->pl+9] ^= p.v.cand; 
		el_par_ch[p.f->eb+18] ^= p.v.cand;         }
	//EE.Enl("debut recherche bug 1");
	if(ntplus == 1)
		return Bug1();
	//EE.Enl("debut recherche bug 2");
	if(Bug2())
		return 1; // priority to bug 2
	//EE.Enl("debut recherche bug 3 4");
	if(Bug3a(58))
		return 1;
	return 0;
}
//===================== calls 
int TPAIRES::Bug3a(int rat) {
	brat = rat;  // maximum authorized in that step
	for(int i = 0; i < 27; i++) {
		if(zplus.EstDans(divf.elz81[i])) {
			ntpa = nwp = 0;// collect data in that row/col/box
			candp_or.f = candnp_or.f = nwp = 0;
			//candp_xor.f = 0;
			//candnp_xor.f = 0;
			for(int j = 0; j < 9; j++) {
				int i8 = divf.el81[i][j];
				CELL p = T81t[i8];
				if(zplus.On(i8)) { // wplus has the same order as tplus
					candnp_or |= p.v.cand;
					//candnp_xor ^= p.v.cand;
					//wplus[nwp] = p.v.cand;
					nwp++;
				}
				else if(p.v.ncand == 2) {
					candp_or |= p.v.cand;
					//candp_xor ^= p.v.cand;
					tpa[ntpa++] = i8;
				}
			}
			aigun = 1;
			if(Bug3(i))
				return 1;//bug3 and bug4
			continue;//only one row/col/box
		}
	}
	return 0;
}

//===========================
int TPAIRES::Bug1() {
	int i8 = zplus.First();
	CELL p = T81t[i8];
	BF16 wc = p.v.cand & el_par_ch[p.f->el],
		w = p.v.cand - wc;
	if(wc.QC() - 2)
		return 0; //must be 2 to reach parity
	T81t[i8].Keep(w); // eliminate the others
	BugMess(" 1");
	return 1;
}

//================================ cells in different objects or one digit
int TPAIRES::Bug2() { // any number of cells, but 6 seems very high
	if(ntplus > 6 || aigpmax > 3)
		return 0;  
	BF16 possible;
	possible.f = 0x1ff;
	BF32 b18;
	b18.f = 0; // find parity of cells in r/c
	for(int i = 0; i < ntplus; i++) {
		CELL_FIX p1 = t81f[tplus[i]];
		b18.f ^= 1 << p1.el;
		b18.f ^= 1 << (p1.pl + 9);
	}
	BF81 zw;
	zw.SetAll_1();
	for(int i = 0; i < ntplus; i++) { // analyse all cells
		CELL p1 = T81t[tplus[i]];
		zw &= p1.f->z;
		BF16 w1;
		w1.f = 0;
		if(b18.On(p1.f->el))
			w1 = p1.v.cand - el_par_ch[p1.f->el];
		if(!w1.f) { // if nothing comes in row odd cells, try the column
			if(b18.On(p1.f->el))
				return 0;
			else
				w1 = p1.v.cand - el_par_ch[p1.f->pl + 9];
		}
		else if(b18.On(p1.f->pl + 9)) { // check other direction
			BF16 w2 = p1.v.cand - el_par_ch[p1.f->pl + 9];
			if(w1.f - w2.f)
				return 0;
		}
		possible &= w1;
	}
	if(zw.IsEmpty())
		return 0;// must have a comon control on some cells
	if(possible.QC() - 1)
		return 0; // must be one common digit
	// last check, parity ok everywhere
	for(int i = 0; i < 27; i++)
		el_par2_ch[i] = el_par_ch[i];
	for(int i = 0; i < ntplus; i++) { // change parity for all cells
		CELL_FIX p1 = t81f[tplus[i]]; 
		BF16 wch = T81t[tplus[i]].v.cand - possible;
		el_par2_ch[p1.el] ^= wch;
		el_par2_ch[p1.pl+9] ^= wch;
		el_par2_ch[p1.eb+18] ^= wch;
	}
	for(int i = 0; i < 27; i++)
		if(el_par2_ch[i].f)
			return 0;

	// ok for bug type 2 clear the commonly controled  cells

	int ir = 0, ch = possible.First(); 
	for(int i = 0; i < 81; i++)
		if(zw.On(i))
			ir += T81t[i].Change(ch);
	if(ir) {
		BugMess("2 same digit");
		puz.SetDif(57);
	}
	return ir;
}
//===========================================
void TPAIRES::BugMess(const char * lib) const {
	EE.E("Bug type ");
	EE.Enl(lib);
	if(Op.ot)
		T81C->Candidats();
}

//former _04d_paires_bug3.cpp follows
//===================  all cells in the same  element(s )(can be type 2)
int TPAIRES::Bug3(int el) {
	if((ntplus == 2) && Bug_lock(el))
		return 1;
	if(el < 18)
		return Bug3_4_Nacked(el);
	EE.Enl("recherche en boite"); 

	// we have now a box and not 2 cells with a digit locked

	if(ntplus > 3)
		return 0; // would not work with that process
	// look first row and col with only one cell
	BF16 wrow, wcol;  // on cherche parite de row/col
	CELL *pp;
	USHORT elx, phx[5];
	for(int i = 0; i < ntplus; i++) { // first look for parity
		pp = &T81t[tplus[i]];
		phx[i] = 0;
		BF16 wr(pp->f->el), wc(pp->f->pl);
		wrow ^= wr;
		wcol ^= wc; // change parity
	}
	BF16 wcx, welim, annul, wpar; 

	EE.Enl("recherche en boite phase 1"); 

	for(int i = 0; i < ntplus; i++) {
		pp = &T81t[tplus[i]]; 
		if(wrow.On(pp->f->el))
			elx = pp->f->el;    
		else if(wcol.On(pp->f->pl))
			elx = pp->f->pl + 9; 
		else
			continue;// not processed the 
		phx[i] = 1;
		wcx = pp->v.cand & el_par_ch[elx];
		annul = pp->v.cand - wcx;
		wpar ^= wcx;  // to adjust parity for the last if needed
		if((wcx.QC() - 2))
			return 0;
		welim |= annul;
	}	 
	EE.E("recherche en boite phase 2"); 
	EE.E(" wpar");
	EE.Enl(wpar.String());

	wpar ^= el_par_ch[el];  // adjust parity in the box

	// finish the task if one has no row/col free
	for(int i=0;i<ntplus;i++) {
		if(phx[i])
			continue;// done
		pp = &T81t[tplus[i]]; 
		wcx = pp->v.cand & wpar;
		annul = pp->v.cand - wcx;
		EE.E(t81f[tplus[i]].pt);
		EE.Esp();
		EE.E(" wpar");
		EE.E(wpar.String());
		EE.E(" wcx");
		EE.Enl(wcx.String());
		if((wcx.QC() - 2))
			return 0;
		welim |= annul;
	}	 

	return Nacked_Go(welim);
}

int TPAIRES::Nacked_Go(BF16 welim) {
	//we look now for  "naked locked sets"
	EE.E("recherche  bug3_4 Nacked ok to go welim= ");
	EE.Enl(welim.String());
	int nelim = welim.QC(); // look for naked in increasing order
	if(nelim < 2 || nelim > 5)
		return 0;
	if(brat == 58 && nelim < 3) { //then search for naked 2
		for(int i = 0; i < ntpa; i++) {
			if(T81t[tpa[i]].v.cand.f == welim.f) { // we found it
				int ir = 0;
				for(int j = 0; j < ntpa; j++)
					if(j-i)
						ir += T81t[tpa[j]].Change(welim);
				if(ir) {
					BugMess("type 3/4 naked pair");
					puz.SetDif(58);
					return 1;
				}
			}
		}
	}
	if(brat == 59) {        // look for triplet
		for(int i1 = 0; i1 < ntpa - 1; i1++) {
			for(int i2 = i1 + 1; i2 < ntpa; i2++) {
				BF16 ww = welim | T81t[tpa[i1]].v.cand | T81t[tpa[i2]].v.cand;
				if(ww.QC() - 3)
					continue; // if not we got it
				int ir = 0;
				for(int j = 0; j < ntpa; j++)
					if((j - i1) && (j - i2))
						ir += T81t[tpa[j]].Change(ww);
				if(ir) {
					BugMess("type 3/4 naked triplet");
					puz.SetDif(59);
					return 1;
				}
			}
		}
	} // end triplet
	if(brat == 60) {                   // look for quad
		for(int i1 = 0; i1 < ntpa - 2; i1++) {
			for(int i2 = i1 + 1; i2 < ntpa - 1; i2++) {
				for(int i3 = i2 + 1; i3 < ntpa; i3++) {
					BF16 ww = welim | T81t[tpa[i1]].v.cand | T81t[tpa[i2]].v.cand | T81t[tpa[i3]].v.cand;
					if(ww.QC() - 4)
						continue; // if not we got it
					int ir = 0;
					for(int j = 0; j < ntpa; j++)
						if((j - i1) && (j - i2) && (j-i3))
							ir += T81t[tpa[j]].Change(ww);
					if(ir) {
						BugMess("type 3/4 naked quad");
						puz.SetDif(60);
						return 1;
					}
				}
			}
		}
	}// end quad
	if(brat == 61) {                   // look for (5)
		for(int i1 = 0; i1 < ntpa - 3; i1++) {
			for(int i2 = i1 + 1; i2 < ntpa - 2; i2++) {
				for(int i3 = i2 + 1; i3 < ntpa - 1; i3++) {
					for(int i4 = i3 + 1; i4 < ntpa; i4++) {
						BF16 ww = welim | T81t[tpa[i1]].v.cand | T81t[tpa[i2]].v.cand
							| T81t[tpa[i3]].v.cand | T81t[tpa[i4]].v.cand;
						if(ww.QC() - 5)
							continue; // if not we got it
						int ir = 0;
						for(int j = 0; j < ntpa; j++)
							if((j - i1) && (j - i2) && (j - i3) && (j - i4))
								ir += T81t[tpa[j]].Change(ww);
						if(ir) {
							BugMess("type 3/4 naked (5)");
							puz.SetDif(61);
							return 1;
						}
					}
				}
			}
		}
	}// end (5)
	return 0;
}

//former _04d_paires_bug4.cpp follows
//===================  all cells in the same  element(s )(can be type 2)
int TPAIRES::Bug_lock(int el) {
	EE.Enl("recherche  bug_lock"); 
	BF16 clock = candnp_or - candp_or;  // locked candidate
	EE.E(" clock=");
	EE.Enl(clock.String());
	if(!clock.f)
		return 0; 
	CELL p1 = T81t[tplus[0]], p2 = T81t[tplus[1]];
	int el1 = p1.f->el, el2 = p2.f->el; // use the row in priority
	if(el < 9)  {
		el1 = p1.f->pl + 9;
		el2 = p2.f->pl + 9;
	}  // and col if row is "el"
	if(el1==el2) {
		if(el>=9) return 0;
		el1 = p1.f->pl + 9;  // can not accept el1=el2 chaeck the other direction
		el2 = p2.f->pl + 9;
	}

	BF16 wc1 = (p1.v.cand & el_par_ch[el1]), wce1 = p1.v.cand - wc1,
		wc2 = (p2.v.cand & el_par_ch[el2]), wce2 = p2.v.cand - wc2;
	EE.E(p1.f->pt);
	EE.E(" el=");
	EE.E(el1 + 1);
	EE.E(" wc1=");
	EE.Enl(wc1.String());
	EE.E(p2.f->pt);
	EE.E(" el=");
	EE.E(el2 + 1);
	EE.E(" wc2=");
	EE.Enl(wc2.String());

	if(wce2.f-wce1.f) return 0; // must be the same digit
	if((wc1.QC() - 2) || (wc2.QC() - 2))
		return 0;	 

	T81t[tplus[0]].Keep(wce1 | clock);
	T81t[tplus[1]].Keep(wce2 | clock);
	BugMess("3/4 a digit locked");
	puz.SetDif(57);
	return 1;
}

//============================================
int TPAIRES::Bug3_4_Nacked(int el) {
	EE.Enl("recherche  bug3_4 Nacked"); 

	USHORT ctl = ntplus, aig = 1;  
	CELL *pp;
	USHORT elx;
	BF16 wcx, welim, annul; 

	for(int i = 0; i < ntplus; i++) {
		pp = &T81t[tplus[i]];
		elx = pp->f->el;
		if(el < 9)
			elx = pp->f->pl + 9; 
		wcx = pp->v.cand & el_par_ch[elx];
		annul = pp->v.cand - wcx;
		//EE.E(t81f[tplus[i]].pt); EE.E(" el=");EE.E(elx+1);
		//EE.E(" wc=");EE.Enl(wcx.String());
		if((wcx.QC() - 2))
			return 0;
		welim |= annul;
	}	
	return Nacked_Go(welim);
}

//former _04d_XYW.cpp follows

//<<<<<<<<<<<<<<<<<<<< // depart deux paires pas objet commun et trio
int TPAIRES::XYWing() { // troisieme par les isoles  objets  communs
	int ir = 0;
	for(int i = 0; i < ip - 1; i++) {
		for(int j = i + 1; j < ip; j++) {
			if(CommunPaires(i, j))
				continue;
			BF16 w = zp[i].pa|zp[j].pa;
			if(w.QC() - 3)
				continue;
			BF16 w1 = (zp[i].pa&zp[j].pa), w2=w1 ^ w;  // les deux non communs
			for(int k = 0; k < ip; k++) {
				if(zp[k].pa.f - w2.f)
					continue; // il faut =
				if(!CommunPaires(i, k))
					continue;
				if(!CommunPaires(j, k))
					continue;
				// on a un XYWing  potentiel
				int ich = w1.First(); // le chiffre 
				BF81 z1 = t81f[zp[i].i8].z & t81f[zp[j].i8].z,
					z2 = z1 & puz.c[ich];  // z2 est à supprimer
				if(z2.IsNotEmpty()) {
					if(Op.ot)
						CommunLib(i, j, zp[k].i8, "->XY WING pivot= ");
					T81->Clear(z2, ich);
					return 1;
				}
			} 
		}
	}
	return ir;
}

//<<<<<<<<<<<<<<<<<<<< // depart deux paires pas objet commun et trio
int TPAIRES::XYZWing() { // troisieme est le trio objets communs
	int ir = 0;  
	for(int i = 0; i < ip - 1; i++) {
		for(int j = i + 1; j < ip; j++) {
			if(CommunPaires(i, j))
				continue;
			BF16 w = zp[i].pa|zp[j].pa;
			if(w.QC() - 3)
				continue;
			BF16 w1 = (zp[i].pa&zp[j].pa);  // le chiffre
			for(int k = 0; k < 81; k++) {
				if(T81t[k].v.cand.f - w.f)
					continue; // il faut = trio
				if(!CommunTrio(k, i)) continue;
				if(!CommunTrio(k, j)) continue;
				// on a un XYZWing  potentiel
				int ich = w1.First(); // le chiffre
				BF81 z1 = t81f[zp[i].i8].z & t81f[zp[j].i8].z & t81f[k].z,
					z2 = z1 & puz.c[ich];  // z2 est à supprimer
				if(z2.IsNotEmpty()) {
					if(Op.ot)
						CommunLib(i, j, k, "->XYZ WING pivot= ");
					T81->Clear(z2, ich);
					return 1;
				}
			}   
		}
	}
	return ir;
}

void TPAIRES::CommunLib(int i, int j, int k, char * lib) {
	if(!Op.ot)
		return;
	EE.E(lib);
	EE.E(t81f[k].pt);
	EE.E(" ");
	EE.E(T81t[k].scand);	 
	EE.E(" (1)=");
	EE.E(t81f[zp[i].i8].pt);
	EE.E(" ");
	EE.E(T81t[zp[i].i8].scand);
	EE.E(" (2)=");
	EE.E(t81f[zp[j].i8].pt);
	EE.E(" ");
	EE.Enl(T81t[zp[j].i8].scand);	
}

int TPAIRES::CommunPaires(int i, int j) {
	return T81t[zp[i].i8].ObjCommun(&T81t[zp[j].i8]);
}
int TPAIRES::CommunTrio(int i, int j) {
	return T81t[i].ObjCommun(&T81t[zp[j].i8]);
}





CELL * SEARCH_UR::ta,*SEARCH_UR::tr; // ta action, tr recherche 
REGION_INDEX * SEARCH_UR::tchel;

SEARCH_UR::SEARCH_UR() {	 // constructor
	ta = T81t;
	tr = T81tc;
	tchel = puz.alt_index.tchbit.el;
}

int SEARCH_UR::GetElPlus() {
	return tp81f.GetLigCol(pp1,pp2);
} // assuming nplus=2

int SEARCH_UR::IsDiag() {
	if(divf.IsObjet(deux[0],deux[1]))
		diag=0;
	else
		diag = 1;
	return diag;
}

int SEARCH_UR::Jumeau(USHORT a,USHORT b,USHORT ch) {
	USHORT el = tp81f.GetLigCol(a,b);
	return Jum(el, ch);
}

void SEARCH_UR::ImageRI(char * lib,USHORT a) {
	EE.E(lib);
	EE.E(tr[a].f->pt);
	EE.E(" ");
	EE.E(tr[a].scand);
}

void SEARCH_UR::ImageRI(char * lib) {
	if(!Op.ot) return;
	EE.E( "->UR" );
	EE.E(lib);
	ImageRI(" P1=", ia);
	ImageRI(" P2=", ib);
	ImageRI(" P3=", ic);
	ImageRI(" P4=", id);
	EE.Enl();
}

//former _05a_rin_el.cpp follows

/* full processing for 2 cells with adds in one object UR or UL
   can be bivalue, "hidden locket set" or "locked set"
   always the lowest rating found
   the rule has been copied from SE code analysis adjusted to lksudokuffixed8 veersion.
   here "basis" is the rating entry for the specific object
       UR basi=4.5 
       UL up to 6 cells basis=4.6 (+0.1)  7_8 cells 4.7 (+0.2)  10_.. 4.8 (+0.3)
   basis        one digit bivalue
   basis +0.1 (n-1) hidden/nacked sets of "n" cells

   for hidden and naked sets, the lowest rating is taken depending on "n" values 

summary of rating having "equalled" hidden and naked as in lksudoku 1.2.5.0

 URUL 
 cells -->  4    6    8   >=10

 pair     4.6  4.7  4.8  4.9    action 2
 triplet  4.7  4.8  4.9  5.0    action 3
 quad     4.8  4.9  5.0  5.1    action 4


*/
int SEARCH_UR::T2(USHORT action) {
	int ir1 = 0, ir2 = 0, iel = GetElPlus();
	if(iel >= 0) {
		ir1 = T2_el(iel, action);
		if(ir1 == 1)
			return 1;
	}
	int eb = t81f[pp1].eb;
	if(eb == t81f[pp2].eb)
		ir2 = T2_el(eb + 18, action);
	if(ir2 == 1)
		return 1;
	if((ir1 + ir2) == 0)
		return 0;
	else
		return 2;
}

// same but el is now identified 
int SEARCH_UR::T2_el(USHORT el, USHORT action) {
	//look mode 1 for a bivalue of one of the common digits
	if(0) {
		ImageRI("");
		EE.E("UR/UL el ");
		EE.E(el + 1);
		EE.E(" action=");
		EE.E(action);
		EE.E(" pp1=");
		EE.E(t81f[pp1].pt);
		EE.E(" pp2=");
		EE.Enl(t81f[pp2].pt);
	}
	if(action == 1) { //   this is a "basis"  
		if(Jum(el, chc1)) {
			int ir1 = T81t[pp1].Change(chc2) + T81t[pp2].Change(chc2);
			if(ir1) {
				EE.E("UR/UL bivalue ");
				EE.Enl(chc1+1);
				return 1;
			}
		}
		else if(Jum(el,chc2)) {
			int ir1 = T81t[pp1].Change(chc1) + T81t[pp2].Change(chc1);
			if(ir1) {
				EE.E("UR/UL bivalue ");
				EE.Enl(chc2 + 1);
				return 1;
			}
		}
	}

	// first look  for cells having a potential for hidden/direct locked sets
	int ir = 0;
	aig_hid = 0;
	nth = ntd = nnh = 0;
	wh = wc;
	wd = wr;
	wnh = wr;
	wnd = wc;
	wdp.f = 0;
	zwel.SetAll_0(); // to prepare clearing of naked sets
	for(int i = 0; i < 9; i++) {
		int ppi = divf.el81[el][i];
		if((ppi == pp1) || (ppi == pp2))
			continue;
		CELL_VAR v = T81t[ppi].v;
		if(v.typ)
			continue; // given or assigned 
		// count and store cells including common digits 
		zwel.Set(ppi);//will contain all active cells out of the UR at the end
		BF16 ww = v.cand & wc;
		if(ww.f) {
			th[nth++] = ppi;
			wh |= v.cand;
			if(ww.f == wc.f)
				aig_hid = 1;
		} 
		else { // naked locked set  can not have common digits	  
			wnh |= v.cand; 
			tnh[nnh++] = ppi; // nnh to check at least one cell without common digits
			ww = v.cand & wr; 
			if(ww.f) {
				td[ntd++] = ppi;
				wd |= v.cand;
			}
			else
				wnd |= v.cand;
		}
	}
	//ImageRI(" ");EE.E(" nth=");EE.E(nth );EE.E(" wh=");EE.Enl(wh.String() );

	if(!nnh)
		return 0;//all cells have common digits nothing to do

	//EE.E(" nth=");EE.E(nth );EE.E(" nnh=");EE.E(nnh );EE.E(" nd=");EE.Enl(ntd );

	if(action < 2)
		return 2; // store it if not basic
	//  hidden pair 
	if(nth < 2 && (action == 2)) {  //  hidden pair if active
		if(T81t[th[0]].Keep(wc)) {
			EE.Enl("UR/UL hidden pair");
			return 1;
		}
	}

	// we now look  for  a hidden or nacked set of the appropriate length

	if(T2_el_set_hidden(action - 1))
		return 1;
	if(T2_el_set_nacked(action - 1))
		return 1;
	return 0;
}

//former _05a_rin_el2.cpp follows

// we are now looking for nacked or hidden sets not  pair
// we have not so many possible patterns, 
// we try to catch them per pattern
// it need more code, but it should be faster and easier to debug

// look for hidden  
int SEARCH_UR::T2_el_set_hidden(USHORT len)
{	// first pattern, take it if direct 
 if(nth==len)   // not the expected length
 { // identify extra digits in hidden locked set
 BF16 whh=wh-wnh;  // all digits of the assumed locked set
 if(whh.QC()-(nth+1)) return 0;
 //go for the a hidden set if active
  int ir1=0;
  for(int i=0;i<nth;i++)  ir1+=T81t[th[i]].Keep(whh); 
  if(ir1) { EE.E("UR/UL hls whh="); EE.Enl(whh.String());
	       EE.Enl("UR/UL hidden locked set"); return 1;}	
 }
 if(nth==2 && len==3) 
// second pattern with nth=2 but a hidden quad 
// adding another cell
 {	for(int i=0;i<nnh;i++)
   {BF16 wa=wh|T81t[tnh[i]].v.cand,wb,wx;
    for(int j=0;j<nnh;j++) if(j-i) wb|=T81t[tnh[j]].v.cand;
	wx=wa-wb-wr; // must not be an extra digit included in the UR
	if(wx.QC()==4) // we got it
	{int ir1=0; ir1+=T81t[tnh[i]].Keep(wx);
     for(int k=0;k<nth;k++)  ir1+=T81t[th[k]].Keep(wx); 
     if(ir1) { EE.E("UR/UL hls wx="); EE.Enl(wx.String());
	       EE.Enl("UR/UL hidden locked set"); return 1;}	
	}
   }
 }

return 0;}

// look for nacked sets n
int SEARCH_UR::T2_el_set_nacked(USHORT len)
 {if(ntd<(nautres-1)) return 0;  // minimum without extra digit

 if(ntd && (nautres==2)&&(len==1)) // look for a nacked pair
  {for(int i=0;i<ntd;i++) 
   {int i8=td[i];BF16 ww=T81t[i8].v.cand;  
    if( ww.f==wr.f) // we got it     
	{ BF81 zwel1=zwel; zwel1.Clear(i8);
	  if(T81->Clear(zwel1,wr  ))
		{ EE.Enl("UR/UL nacked pair");return 1;}
	}}
  }

 if(len<2) return 0;// nothing else if a pair is expected
if(0) { EE.E("look for nacked len ="); EE.E( len);
          EE.E(" nnh ="); EE.Enl( nnh);}
// first   pattern is nacked triplet no extra digit
if(nautres==3 && ntd>2 && len==2)
 { BF81 zwel1=zwel; int n=0;  
   for (int i=0;i<ntd;i++)      if(!(T81t[td[i]].v.cand-wr).f)  
	                           { n++; zwel1.Clear(td[i]);}
   if(n==2 && T81->Clear(zwel1,wr  ))
		       { EE.Enl("UR/UL nacked LS3");return 1;}
 }

// second  pattern : directly correct (may be with extra digit)
if(((ntd+1)==wd.QC())  && (len== ntd))
 { BF81 zwel1=zwel; 
   for (int i=0;i<ntd;i++)  zwel1.Clear(td[i]);
   if(T81->Clear(zwel1,wd  ))
		{ EE.E("UR/UL nacked LS");EE.Enl(ntd+1);return 1;}
 }
// third  pattern is  "one cell in excess"
//should cover several situations 
if(ntd>=nautres && (len==(ntd-1)))
 { for (int i=0;i<ntd;i++)
	{BF16 wdx=wr;	 // wr is the minimum fo the nacked lock set
	 for (int j=0;j<ntd;j++) if(j-i) wdx|=T81t[td[j]].v.cand;
	 // check if now correct
	      if(ntd==wdx.QC()) //  one cell less ntd must be number of digits
       { BF81 zwel1=zwel; 
        for (int j=0;j<ntd;j++)  if(j-i)zwel1.Clear(td[j]);
		if(T81->Clear(zwel1,wdx  ))
		   { EE.E("UR/UL nacked LS");EE.Enl(ntd);return 1;}
       }
     }
  }

// Fourth  pattern is  "2 cells in excess"
//could cover several situations 
if(ntd>=(nautres+1) && (len==(ntd-2)))
 { for (int i=0;i<(ntd-1);i++)for (int k=i+1;k<ntd;k++)
	{BF16 wdx=wr;	 // wr is the minimum fo the nacked lock set
	 for (int j=0;j<ntd;j++) if((j-i) && (j-k))
		    wdx|=T81t[td[j]].v.cand;
	 // check if now correct
	if(ntd==(wdx.QC()+1))  
       { BF81 zwel1=zwel; 
        for (int j=0;j<ntd;j++)  if((j-i) && (j-k))
			 zwel1.Clear(td[j]);
		if(T81->Clear(zwel1,wdx  ))
		   { EE.E("UR/UL nacked LS");EE.Enl(ntd-1);return 1;}
       }
     }
  }
// fifth pattern,  we look now for something with all extra cells "over all"
// have an example of naked quad, but can be simpler
if(nnh>=nautres && (len==nnh))
 {BF16 wdx; int ir=0;
  for (int i=0;i<nnh;i++) wdx|=T81t[tnh[i]].v.cand;
  if(nnh==(wdx.QC()-1) && ((wdx&wr).f == wr.f) ) 
    { for (int i=0;i<nth;i++)  ir+=T81t[th[i]].Change(wdx  );
      if(ir)  { EE.E("UR/UL nacked LS");EE.Enl(wdx.QC());return 1;}
    } 
 }

// fifth all extra cells except one
if(nnh>=nautres && (len==(nnh-1)))
 {for (int j=0;j<nnh;j++)
  {BF16 wdx; int ir=0;
	  for (int i=0;i<nnh;i++) if(j-i)  wdx|=T81t[tnh[i]].v.cand;
  if((wdx&wr).f-wr.f) continue;
  if(nnh==wdx.QC()) 
    { BF81 zwel1=zwel; 
      for (int i=0;i<nth;i++)  ir+=T81t[th[i]].Change(wdx  );
	  ir+=T81t[tnh[j]].Change(wdx  );
      if(ir)  { EE.E("UR/UL nacked LS");EE.Enl(wdx.QC());return 1;}
    } 
  }
 }
// sixth all extra cells except 2
if(nnh>=(nautres+1) && (len==(nnh-2)))
 { EE.E("look for all extra -2 nnh=");EE.Enl(nnh);
 for (int j=0;j<nnh-1;j++)for (int k=j+1;k<nnh;k++)
  {BF16 wdx; int ir=0;
	  for (int i=0;i<nnh;i++) if((j-i)  && (k-i))
		     wdx|=T81t[tnh[i]].v.cand;
   if((wdx&wr).f-wr.f) continue;
   if(nnh==wdx.QC()+1) 
    { BF81 zwel1=zwel; 
      for (int i=0;i<nth;i++)  ir+=T81t[th[i]].Change(wdx  );
	  ir+=T81t[tnh[j]].Change(wdx  ); ir+=T81t[tnh[k]].Change(wdx  );
      if(ir)  { EE.E("UR/UL nacked LS");EE.Enl(wdx.QC());return 1;}
    } 
  }
 }


return 0;}

/*

// sixth pattern,  we look now for something with 3 extra cells "over all"
// has been seen 
if(nnh>=(nautres+1)&& (len==(nnh-3)) )
 {EE.Enl("look for nacked LS + extra digit less 3 cells");
	 for (int i1=0;i1<nnh-2;i1++)for (int i2=i1+1;i2<nnh-1;i2++)
     for (int i3=i2+1;i3<nnh;i3++)
	{BF16 wdx=wr;	 // wr is the minimum fo the nacked lock set
	 for (int j=0;j<nnh;j++) 
		  if((j-i1) &&(j-i2)&&(j-i3))    wdx|=T81t[tnh[j]].v.cand;
	 // check if now correct
     if((nnh-2)==wdx.QC()) 
       { BF81 zwel1=zwel; 
        for (int j=0;j<nnh;j++)  
			 if((j-i1) &&(j-i2)&&(j-i3))zwel1.Clear(tnh[j]);
         if(T81->Clear(zwel1,wdx  ))
		   { EE.E("UR/UL nacked LS");EE.Enl(nnh-2);return 1;}
       }
     }
  }	 
*/

//a piece of former _05b_RI_2N.cpp follows

// one posible location for a UR;
// no assigned position, 2 and only 2 common digits

int SEARCH_UR::RID(int i1,int i2,int c1,int c2) {
	ia = I81::Pos(i1, c1);
	ib = I81::Pos(i1, c2);
	ic = I81::Pos(i2, c1);
	id = I81::Pos(i2, c2);
	char * gr = puz.gg.pg;
	if((gr[ia] - '0') || (gr[ib] - '0') || (gr[ic] - '0') || (gr[id] - '0'))
		return 0;
	if(Setw() - 2)
		return 0;
	CalcDeux();   
	if(ndeux == 3) {
		ta[pp1].Change(wc);
		ImageRI("1");
		return 1;
	}// type 1
	Setwou();
	GenCh(); 
	if((ndeux - 2) || IsDiag())
		if(ndeux == 1)
			return RID3();
		else
			return 0; // not processed

	// if one digit active, do it now 4.5
	if(nautres == 1) { // un chiffre en lig/col  ou diagonal
		if(puz.Keep(ch1, pp1, pp2)) {
			ImageRI(" one digit active");
			return 1;
		}
	}
	// if one of the common digit is a bivalue, do it as well it is 4.5
	while((tr[ia].v.ncand > 2) || (tr[ic].v.ncand > 2)) {
		USHORT pw = ia;
		ia = ib;
		ib = id;
		id = ic;
		ic = pw;
	}// sort cells 
	int ir = T2(1); // we  want only rating 4.5 as "good" so far
	if(ir == 1) {
		ImageRI(" action from object");
		return 1;
	} // it was ok
	if(ir)
		urt.Store(this); 
	return 0; //store if something else seen
}

int SEARCH_UR::RID3() {
	if(nautres - 1)
		return 0;
	BF81 zw;
	zw.SetAll_1();

	if(tr[ia].v.ncand == 3)
		zw &= t81f[ia].z;
	if(tr[ib].v.ncand == 3)
		zw &= t81f[ib].z;
	if(tr[ic].v.ncand == 3)
		zw &= t81f[ic].z;
	if(tr[id].v.ncand == 3)
		zw &= t81f[id].z;

	zw &= puz.c[ch1];

	if(zw.IsNotEmpty()) {
		ImageRI(" UR one digit active");	
		return T81->Clear(zw,ch1);
	}
	return 0;
}






void CHAINSTORE::Print(USHORT index) const {
	if(index>=ise2  || index<1){
		if(!index)
			EE.Enl("\n\nchainstore::print  index null not stored");
		else{
			EE.E("\n\nchainstore::print invalid entry index=");
			EE.Enl( index);
		}
		return;
	}
	int id = s2[index], ie = e2[index];
	for(int i = id; i <= ie; i++) {
		const USHORT * tx = &buf[starts[i]];
		USHORT 	n = ends[i] - starts[i];
		if(n>50) {
			EE.Enl("length too high forced to 5");
			EE.E("index ="); EE.E(index);
			EE.E( " id="); EE.E(id); 
			EE.E( " ie="); EE.E(ie); 
			EE.E( " i="); EE.E(i); 

			EE.E( " ends[i]="); EE.E(ends[i]); 
			EE.E( " starts[i]="); EE.Enl(starts[i]); 
			n=5;
		}
		if(n > 0)
			zpln.PrintImply(tx, n);
	}
}

/* first step in the search for nested chains
   fill zcf.h_nest.d.t[tag]  with the full expansion
      in that mode
*/
/// en cours de travail

void PUZZLE::GoNestedTag(USHORT tag) {
	opp =0; // opdiag;  
	//if((rbase== 105) && (tag>12 && tag<16) && (tag&1 )) opp=1; else opp=0;

	const BFTAG &tt (d_nested.t[tag]); 
    if(opp) {
			EE.E("start gonested tag ");
			EE.E(tag);
			zpln.ImageTag(tag);
			Image(tt,"depart nested",0); 
			EE.Enl(); 
		}	 	
	if(tt.IsEmpty())
		return;
	zcf.StartNestedOne();
	zcx.StartNestedOne();
	zcxb.StartNestedOne();
	to = zcf.h_one.dp.t; //forward and back tracking table 
	tcandgo.Init(); // intital storage for strong links and nested eliminations
	tstore.Init(); // start with an empty table of chains stored
	hdp_base_nested=zcf.hdp_base;

	//now go forward  till nothing or a contradiction

	for(int i = 0; i < 640; i++)
		tsets[i] = 0;
	npas = 0;
	steps[0] = tt; 
	tt.String(tx[0],itx[0]);
	steps[0].Set(tag); // add tag to known
	if(itx[0] >= 200)
		itx[0] = 200; // safety measure
	allsteps = cumsteps[0] = steps[0];  
	nested_aig = 1;
	int maxpas = 30;  // should be enough in that mode

	//--------------------- loop  forward
	while(nested_aig && npas++ <= maxpas) {
		if(opp) {
			EE.E("tag ");
			EE.E(tag);
			zpln.ImageTag(tag);
			EE.E(" npas= ");
			EE.Enl(npas); 
		}	 
		  // prepare the call to GoNestedWhileShort
		nested_aig = 0; 
		cum = &cumsteps[npas - 1];
		step = &steps[npas];
		ta = tx[npas - 1];
		ita = itx[npas - 1];

		if(opp) {
			Image((*cum),"cum debut",0);
		}
		GoNestedWhileShort(tag);                    // while cycle

		if(nested_aig){// something happenned
         
		  cumsteps[npas] = allsteps;   
		  (*step) = allsteps;
		  (*step)-=(*cum);  
		  (*step).String(tx[npas],itx[npas] );

		  if(opp) {
			Image((*step),"step", 0);
		  }
		   // store in d_nested2 if npas==2
		  if(npas==2)
			  d_nested2.t[tag] = allsteps;
		   // stop the process if direct elimination appears
		  BFTAG wt(allsteps);
		  wt&=wt.Inverse();
		  if(wt.IsNotEmpty()){
			  d_nested.t[tag] = allsteps;
			  return;
		  }
           
		}
	}// end while
	d_nested.t[tag] = cumsteps[npas - 1];// note:the last one is empty
}

/* while cycle  for GoNested first run, just find potential
   first apply new actions from direct implications and sets
   then check if new strong links appeared
   if new strong links, search for new indirect forcing chain
   if level, check anyway new multi chain eliminations 

*/
void PUZZLE::GoNestedWhileShort(USHORT tag) {
	USHORT aignl = 1;
	//const BFTAG &cum_here = *cum;
	BFTAG * tdpn = dpn.t,  // new set of direct links
	    * hdpb =hdp_base_nested.t; // to receive new strong links


	// look first for direct 
	for(int it = 0; it < ita; it++) {
		BFTAG x = to[ta[it]];
		if(x.substract(allsteps)) {
			if(opp)
				puz.Image(x,"applied std" ,ta[it]);	   
			allsteps |= x; // and in the total 
			nested_aig = 1;
		}    
	}

	// check now sets

	for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		int n = 0, nni = chx.ncd, aig2 = 0, toff[10]; 
		switch (chx.type) {
		case SET_base:  // must be n-1 false or n false (then all is ok)
			// check if active 0 to 2 cand unknown
			{
				BFTAG bfw;
				for(int i = 0; i < nni; i++) { // max one free 
					USHORT cd = chx.tcd[i], j = cd << 1; // candidate in tag form
					if(cum->On(j)) {
						n = 3;
						break;
					}// set assigned
					if(cum->Off(j ^ 1)) {
						toff[n++] = j;
						if(n > 2)
							break;
					} 
					else
						bfw.Set(j ^ 1);
				}

				USHORT cd1 = toff[0], cd2 = toff[1]; 
				if(n == 2) {
					if(hdpb[cd1].Off(cd2 ^ 1) || 
					   hdpb[cd2].Off(cd1 ^ 1) ||
					   hdpb[cd1 ^ 1].Off(cd2) || 
					   hdpb[cd2 ^ 1].Off(cd1)) {
						hdpb[cd1].Set(cd2 ^ 1);
						hdpb[cd2].Set(cd1 ^ 1);
						hdpb[cd1 ^ 1].Set(cd2);
						hdpb[cd2 ^ 1].Set(cd1);
						aignl = 0;
						tcandgo.AddStrong(cd1 >> 1, cd2 >> 1, bfw, nni - 2);
                        nested_aig=1;  // must force new loop
					}
					break;
				}
				if(!n){  // nothing valid switch to a contradiction on any candidate
                    for(int i = 0; i < nni; i++) { 
                         USHORT cd = chx.tcd[i], j = cd << 1;
						 if(allsteps.Off(j)) {
						      allsteps.Set(j);
						      tsets[j] = ie;
					          nested_aig = 1;
						      aig2 = 1;
						 }
					}
					break;
				}

				if(n - 1)
					break;	// if ok second round for action	
				USHORT j = toff[0]; // candidate in tag form
				if(allsteps.Off(j)) { // kep only the first one
						allsteps.Set(j);
						tsets[j] = ie;
						nested_aig = 1;
						aig2 = 1;       
					}
				
				if(opp && aig2) {
					EE.E("set derive actif ");
					chx.Image();
					EE.Enl();}
			}
			break;
		case SET_set: // in a set all components must be on
			for(int i = 0; i < (nni - 1); i++) {  
				if(cum->On(1 + 2 * chx.tcd[i]))
					continue;
				n = 1;
				break;
			}
			if(n)
				break; // must all be there
			const EVENT &ev = tevent.t[chx.tcd[nni - 1] - event_vi];
			const EVENTLOT &evl = ev.evl;
			for(int i = 0; i < evl.itcd; i++) {
				int j = (evl.tcd[i] << 1) ^ 1; // false state on
				if(allsteps.On(j))
					continue;
				allsteps.Set(j);
				tsets[j] = ie;
				nested_aig = 1;
				aig2 = 1;       
			}
			if(opp && aig2) {
				EE.E("set actif event");
				chx.Image();
				ev.Image();
			}

			break;	
		} // end switch
	}// end proc


	if(nested_aig)
		return;
	// we look for indirect hints
	// zcf.h_one.dp.Image();dpn.Image();
	Gen_dpnShort(tag);


	BFTAG elims; 
	NestedForcingShort(elims); 

	if(elims.substract(allsteps)) { // force false so use elims.inverse()
		if(opp)
			puz.Image(elims,"forcing chain elims" ,0);	
		allsteps |= elims; // and in the total 
		nested_aig = 1;
	}
	if(rbase < 100)
		return;

	BFTAG elims2; 
	NestedMultiShort(elims2); 
	if(opp && Op.ot)
		puz.Image(elims2,"multiforcing recap short ", 0);
	if(elims2.IsNotEmpty()) {
		allsteps |= elims2; //   in the total 
		nested_aig=1;
	}
}


/* dpnshort is pecial for level 4 (nested dynamic)
   in that level, derived weak link have to be considered 
     for the remaining valid candidates of the set

*/
///
void PUZZLE::Gen_dpnShort(USHORT tag) { // create the reduced set of tags check tagelim empty 
	dpn.Init(); 
	BFTAG * tdp = hdp_base_nested.t;
	USHORT tagc = tag;
	if(tagc & 1) tagc ^= 1;
	for(int j = 2; j < puz.col; j++) {
		if(j == tagc)
			continue; // don't process the start point
		if(allsteps.On(j))
			continue; // that tag is defined
		if(allsteps.On(j ^ 1))
			continue; // that tag is defined as well (to check)
		dpn.t[j] = tdp[j];
		dpn.t[j] -= allsteps; // reduce the primary weak links
	}
	if(0) {
		EE.Enl("image dpn de dpnshort");
		dpn.Image();
	}
	if(rbase>100){  //level 4 must find  derived weak links
		// this must be a specific process working on reduced sets;
		// only one step of derivation is made 
	    dn.ExpandAll(dpn);
        zcx.DeriveDynamicShort(allsteps,dpn,dn);
 	}
	dn.ExpandAll(dpn);

}


void PUZZLE::NestedForcingShort(BFTAG & elims) {
	for(int i=2;i< puz.col;i+=2){
		if(allsteps.Off(i^1) && dn.Is(i,i^1))  // a forcing chain found, find the length
			elims.Set(i^1); 
		if(rbase>100){
           // look  for contradiction chains ??
		   BFTAG tw(dn.t[i]);
           tw &= (dn.t[i].Inverse()).FalseState();
		   tw-=allsteps;
		   if(tw.IsNotEmpty()) elims.Set(i^1);

		   // and also for dual chains
	       // still  only for fresh eliminations

		   tw=dn.t[i];
           tw &= dn.t[i^1].FalseState();
		   tw-=allsteps;

		   elims|=tw;
		}

	}

}




/*

Now looking for multi chains eliminations if any
the bfs must contain all candidates killed by the main assumption
and the source of all new strong links used

*/

void PUZZLE::NestedMultiShort(BFTAG & elims) {
	for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		int nni = chx.ncd, aig2 = 1; 
		BFTAG zt;
		zt.SetAll_1();
		zt = zt.FalseState();
		zt -= allsteps;
		if(chx.type - SET_base)
			continue;
		// must be  n false 
		for(int i = 0; i < nni; i++) { // max one free 
			USHORT cd = chx.tcd[i], j = cd << 1; // candidate in tag form
			if(cum->On(j)) {
				aig2 = 0;
				break;
			}// set assigned
			if(cum->Off(j ^ 1))
				zt &= dn.t[j];
		}
		if(aig2)
			elims |= zt; 
	}// end ie
}
/*
    warning
	                  ================================
	that process and upstream search should be extended to dynamic forcing chain
	and dynamic nested forcing chain
	it seems this does not exist in serate so chould be an option 
	                   ==============================

Dynamic search in nested mode for a candidate
   do both contradiction, 
   direct  a -> x and a -> ~x so 'a' is false
   indirect x -> ~a   and ~x -> ~a  'a' is false
     (also x -> ~a and y -> ~a if xy is a bivalue
   and "set contradiction" 2 a for each start "true" of the set
   tagnot is
     0 if no indirect search has to be done
	 pointer to tag expanded to do a direct check

   zcf.h is refreshed for each new entry from zcf.h_one
   zcf.h.dp follows the dynamic forward process 
     each new link is loaded in the dp table
*/



int PUZZLE::GoNestedCase1(USHORT cand) {
	opp = 0; 
	//if(Op.ot && rbase>100)opp=1;
	USHORT tag = cand << 1; 
	if(rbase>90){
	   zcf.StartNestedOne();
	   zcx.StartNestedOne();
	   zcxb.StartNestedOne();
	}
	BFTAG tt = zcf.h.d.t[tag]; 

	if( opp) {
		    EE.E("go nested for cand ");
		    zpln.Image(cand);
		    EE.Enl();
	         }

	tcandgo.Init(); // intital storage for strong links and nested eliminations
	tstore.Init(); // start with an empty table of chains stored
	hdp_base_nested=zcf.hdp_base;

	//now go forward  till nothing or a contradiction

	to = zcf.hdp_dynamic.t; // we use the table without derivations
	for(int i = 0; i < 640; i++)
		tsets[i] = 0;
	npas = 0;
	steps[0].SetAll_0();
	steps[0].Set(tag); 
	allsteps = cumsteps[0] = steps[0];  
	tx[0][0] = tag;
	itx[0] = 1; // initial is tag to go forward

	//ret_code = 0;
	nested_aig = 1;
	int maxpas = pasmax;  // will be reduced to "one more" if something found

	//--------------------- loop  forward
	while(nested_aig && npas++ <= maxpas) {
		  // prepare the call to GoNestedWhileShort
		nested_aig = 0; 
		cum = &cumsteps[npas - 1];
		    // reasonnable stop for crazy cases
		    // could surely be set lower
//		if(cum->Count()>250) break; // nothing more to find /// en cours verif sur test
		step = &steps[npas];
		ta = tx[npas-1];
		ita = itx[npas - 1];


		GoNestedWhile(tag);                    // while cycle

		if(nested_aig){// something happenned
		  cumsteps[npas] = allsteps;   
		  (*step) = allsteps;
		  (*step)-=(*cum);  
		  (*step).String(tx[npas],itx[npas] );
		  }
	   
	    else 
			break; // nothing to do

		if( opp) {  
			EE.E("end step=");
			EE.E(npas);
			Image((*step),"step ", 0);
			Image(allsteps,"all", 0);
			EE.E(" tbuf.istore=");EE.E( tstore.ibuf);
			EE.E(" tbuf.ise=");EE.E( tstore.ise);
			EE.E(" tbuf.ise2=");EE.Enl( tstore.ise2);

		}



		// check for a contradiction in that lot 
		USHORT * tb=tx[npas];
		for(int i = 0; i < itx[npas]; i++) {
			USHORT tgx = tb[i];
			if(allsteps.On(tgx) && allsteps.On(tgx ^ 1)) {
				if(1 && opp) {
					EE.E("\n\nfound active a -> x and a -> ~x");
					zpln.ImageTag(tgx);
					EE.E(" step=");
					EE.Enl(npas);   
				}
				// we compute back the length and show the sequence 
				if(maxpas > npas + 2)
					maxpas = npas + 2; // limit the process to 2 more step
				if(maxpas > pasmax)
					maxpas = pasmax;
				nested_print_option=0;
				//if(couprem==25)nested_print_option=1; 
				int l1 = GoBackNested(tgx), 
					l2 = GoBackNested(tgx ^ 1);
				if(stop_rating) return 1;// push back error code 
				if((!l1) || (!l2))
					continue; // should not be
				int ratch = tchain.GetRating(l1 + l2, tag >> 1);
				if(ratch) { // chain is accepted load it (more comments in test mode)
					if(Op.ot) { // this can be complex and will be developped later
						nested_print_option=1;
						EE.E("\n\nchain plus killing ");
						zpln.Image(tag >> 1);
						EE.Enl();	     
						EE.E("chain 1 ");
						GoBackNested(tgx);
						EE.E("chain 2 ");
						GoBackNested(tgx ^ 1);
					}
					tchain.LoadChain(ratch, "chain plus", tag >> 1);
				}// end load chain

			} // end one source for elimination
		}// end for i
		if(0 && !nested_aig) {
			EE.E("fin aig=0 pour step=");
			EE.Enl(npas);
		}
	}// end while
	return 0; //ret_code;
}

/* 
Dynamic search in nested mode for a candidate
   cleared through a set  of  candidates
 
 ttags:  the sets or the 2 states in tag form
 ntags   the number of tags to process
 target  the tag to eliminate (candidate in false state)
 base     the base depending on the level 

 expand each tag of ttags till target is found
*/
/* find the target in dynamic mode for a set of tags

  pending optimisation on bi values with ntags==2
  if base = 75 (Nishio), nothing to do
  if base = 85, it is always a bi value
  if Base > 85 reduce by one if
    first step in chain only one candidate and a "true" form

*/

/// en cours travail

void PUZZLE::Rating_Nested( USHORT * ttags, USHORT ntags, USHORT target) {
	opp=0;
	//if(ntags==2) opp=1; else opp=0;    

	USHORT ctarg = target >> 1;

	     // forget if target already eliminated
	if(tchain.cycle_elims.On(ctarg)) return;

	     // filter if target if part of the set
	for(int i = 0; i < ntags; i++)
		if(ttags[i] >> 1 == ctarg)
			return;	



	USHORT length = 0;
	if((ntags==2) && (rbase==85) )length--;
	// to revise later cell or region bi values instead of that if valid


	nested_print_option= 0;
	for (int i = 0; i < ntags; i++) {
		USHORT lx = GoNestedCase2_3( ttags[i], target);

		if(stop_rating) 
			return ;// push back error code 
		if(!lx)
			return; //should never be
		length += lx;
		  // in next call use ls to avoid length <2
	    int ratch = tchain.GetRating(lx, target >> 1);
		if(!ratch) break; // stop as soon as possible
	}



	int ratch = tchain.GetRating(length, target >> 1);


	if(ratch) { // chain is accepted load it (more comments in test mode)
		if(Op.ot) { 
			EE.E("\n\nrating nested killing "); zpln.ImageTag(target);  
			EE.E( "  through  " );
			for(int it=0;it<ntags;it++){			 
		       zpln.ImageTag(ttags[it]);
		       EE.Esp();
			}
			EE.Enl();	  
			// we restart the process in print mode

	        nested_print_option=1;
	        for (int i = 0; i < ntags; i++) {		
		         GoNestedCase2_3( ttags[i], target);		 
	        }

		}
		tchain.LoadChain(ratch, "chain plus through set", target >> 1);
	}// end if
}

int PUZZLE::GoNestedCase2_3( USHORT tag, USHORT target) {
	if(rbase>90){
	  zcf.StartNestedOne();
	  zcx.StartNestedOne();
	  zcxb.StartNestedOne();
	}

	if(opp) {
		EE.E("go nested case 2_3for tag ");
		zpln.ImageTag(tag);
		EE.E(" target= ");
		zpln.ImageTag(target);
		EE.E(" print_option= ");
		EE.Enl(nested_print_option);
	}

	tcandgo.Init(); // intital storage for strong links and nested eliminations
	tstore.Init(); // start with an empty table of chains stored
	hdp_base_nested=zcf.hdp_base;

	//now go forward  till nothing or a contradiction

	to = zcf.hdp_dynamic.t; // we use the table without derivations
	for(int i = 0; i < 640; i++)
		tsets[i] = 0;
	npas = 0;
	steps[0].SetAll_0();
	steps[0].Set(tag); 
	allsteps = cumsteps[0] = steps[0];  
	tx[0][0] = tag;
	itx[0] = 1; // initial is tag to go forward

	nested_aig = 1;
	int maxpas = pasmax;  

	//--------------------- loop  forward
	while(nested_aig && npas++ <= maxpas) {
		nested_aig=0; 
		cum = &cumsteps[npas-1];
		step = &steps[npas];
		step->SetAll_0();
		ta = tx[npas-1];
		ita = itx[npas-1];

		GoNestedWhile(tag);                    // while cycle
		if(nested_aig){// something happenned

 		  cumsteps[npas] = allsteps;   
		  (*step) = allsteps;
		  (*step)-=(*cum);  
		  (*step).String(tx[npas],itx[npas] );
		  }
	   
	    else 
			break; // nothing to do


		if(opp){
			EE.E("fin step=");
			EE.E(npas);
			Image((*step),"step ", 0);
			Image(allsteps," allsteps",0);
		}

		// check for a contradiction in that lot (stop at first)
		if(allsteps.On(target)) {
			// we compute back the length and show the sequence 
			int l1 = GoBackNested(target);
			return l1;
		}

	}// end while
	return 0;
}


//former _30a_candgo_while_nested.cpp follows
/* while cycle  for GoNested 
   first apply new actions from direct implications and sets
   then check if new strong links appeared
   if new strong links, search for new indirect forcing chain
   if level, check anyway new multi chain eliminations 

*/
void PUZZLE::GoNestedWhile(USHORT tag) {
	USHORT aignl = 1;
	BFTAG  * hdpb = hdp_base_nested.t; // to receive new strong links
	// look first for direct 
	for(int it = 0; it < ita; it++) {
		BFTAG x = to[ta[it]];
		if(x.substract(allsteps)) {
			if(opp)
				puz.Image(x,"applied std", ta[it]);
			allsteps |= x; // and in the total 
			nested_aig=1;
		} 
        
	}

	// check now sets

	for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		int n = 0, nni = chx.ncd, aig2 = 0, toff[10]; 
		switch (chx.type) {
		case SET_base:  // must be n-1 false or n false (then all is ok)
			{
				// check if active 0 to 2 cand unknown
				BFTAG bfw;
				for(int i = 0; i < nni; i++) { // max one free 
					USHORT cd = chx.tcd[i], j = cd << 1; // candidate in tag form
					if(cum->On(j)) {
						n = 3;
						break;
					}// set assigned
					if(cum->Off(j ^ 1)) {
						toff[n++] = j;
						if(n > 2)
							break;
					} 
					else
						bfw.Set(j ^ 1);
				}

				USHORT cd1 = toff[0], cd2 = toff[1]; 
				if(n == 2) {    // this is a new strong link
                  if(rbase>90){  // only for nested
					if(hdpb[cd1].Off(cd2 ^ 1) || 
					   hdpb[cd2].Off(cd1 ^ 1) || 
					   hdpb[cd1 ^ 1].Off(cd2) || 
					   hdpb[cd2 ^ 1].Off(cd1)) {

						hdpb[cd1].Set(cd2 ^ 1);
						hdpb[cd2].Set(cd1 ^ 1);
						hdpb[cd1 ^ 1].Set(cd2);
						hdpb[cd2 ^ 1].Set(cd1);
						aignl = 0;
						tcandgo.AddStrong(cd1 >> 1, cd2 >> 1, bfw, nni - 2);
                        nested_aig=1;// force new loop if new strong link
					}
				  }
					break;
				}
				if(!n){  // nothing valid switch to a contradiction on any candidate
                    for(int i = 0; i < nni; i++) { 
                         USHORT cd = chx.tcd[i], j = cd << 1;
						 if(allsteps.Off(j)) {
						      allsteps.Set(j);
						      tsets[j] = ie;
					          nested_aig = 1;
						      aig2 = 1;
						 }
					}
					break;
				}

				if(n - 1)
					break;	// if ok second round for action	
				USHORT j = toff[0]; // candidate in tag form
				if(allsteps.Off(j)) {
						allsteps.Set(j);
						tsets[j] = ie;
						nested_aig = 1;
						aig2 = 1;
					}
				
				if(0 && aig2) {
					EE.E("set derive actif ");
					chx.Image();
					EE.E("  valid ");
					zpln.ImageTag(j);
					EE.Enl();
				}
			}
			break;
		case SET_set : // in a set all components must be on
			if(rbase<80) break; // not for Nishio

			for(int i = 0; i < (nni - 1); i++) {
				if(cum->Off((chx.tcd[i] << 1) ^ 1)) {
					n++;
					if(n)
						break;
				}
			}
			if(n)
				break; // must all be there
			const EVENT ev = tevent.t[chx.tcd[nni - 1] - event_vi];
			const EVENTLOT &evl = ev.evl;
			for(int i = 0; i < evl.itcd; i++) {
				int j = (evl.tcd[i] << 1) ^ 1; // false state on
				if(allsteps.On(j))
					continue;
				allsteps.Set(j);
				tsets[j] = ie;
				nested_aig = 1;
				aig2 = 1;       
			}
			if(0 && aig2) {
				EE.E("set actif event");
				chx.Image();
				ev.Image();
			}
			break;	
		} // end switch
	}// end proc

	   // stop if not nested mode or something found

	if((rbase<95) || nested_aig)
		return;    

	// we look for indirect hints
	Gen_dpn(tag);    // create a fresh reduced set of tags 
	// zcf.h_one.dp.Image();dpn.Image();
	BFTAG elims; 
	if(rbase<105)
	   NestedForcing(elims); 
	else
	    NestedForcingLevel4(elims); 
	if(opp && Op.ot){
		Image(allsteps,"allsteps",0);
		puz.Image(elims,"netforcing recap", 0);
	}
	//BFTAG x = elims;  // elims in false state
	if(elims.IsNotEmpty()) {
		allsteps |= elims; // and in the total 
         //		to study that puzzle generating a huge elims
		// 1....67...571.......9....1..4....3.......8..29..7...6......24..5..6...9.....3...8;11.40;10.70;10.00
		nested_aig = 1;
	}

	if(rbase < 100)
		return;

	BFTAG elims2; 
	if(rbase<105)
	   NestedMulti(elims2); 
	else
	   NestedMultiLevel4(elims2); 
	if(opp && Op.ot)
		Image(elims2,"multiforcing recap", 0);
	if(elims2.IsNotEmpty()) {
		allsteps |= elims2; //  in the total 
		nested_aig=1;
	}    
}

/* looking for fresh forcing chain
   we have to find
   the equivalent length
   all data necessary to track back the path, which means
   all candidates that must be eliminated to generate new strong links in use in the chain
 
  to explain the path
    go bakcward
    take all even steps (start=1) which are strong links
    check if this is a new link if yes take the equivalent BFTAG

*/
void PUZZLE::NestedForcing(BFTAG & elims) {
	if(opp){
		 EE.Enl("entry nested forcing");
		 if(allsteps.On(0) || allsteps.On(1))
		   EE.Enl("allsteps polluted");
	}

	for(int i = 2; i < puz.col; i += 2) {
		if( dn.Is(i, i ^ 1)) {  // a forcing chain found, find the length
			BFTAG wch = dpn.t[i], bfs; 
			int era=0;
			USHORT tt[100], itt ; 
			int npasch = wch.SearchChain(dpn.t, i, i ^ 1);
			if((!npasch)|| npasch > 40){
				era=1;
				continue; // never seen so far
			}
			
			else{// 
               itt = npasch + 2;
			   if(wch.TrackBack(dpn.t, i, i ^ 1, tt, itt, i ^ 1))
			       era=1;   // intercept error for debugging	
			}
			

			if(era){// never seen so far
				cerr << "error in multi chains to investigate"<<endl;
					continue; // just skip it
			}
			  //must add the source of the new strong links
			for(int j = 1; j < itt - 1; j++) { // all strong links 
				if((!(tt[j] & 1)) || (tt[j + 1] & 1)) continue;
				const CANDGOSTRONG * w = tcandgo.Get(tt[j], tt[j + 1]);
				if(w) bfs |= w->source;// it is a new strong link
			}
			USHORT ii = tstore.AddOne(tt, itt);
			tsets[i ^ 1] =- tcandgo.itt;
            elims.Set(i ^ 1); 

			tcandgo.tt[tcandgo.itt++].Add(ii, bfs, npasch + 1); 
		}// this is the final length

	}
}



/*
Now looking for multi chains eliminations if any
the bfs must contain all candidates killed by the main assumption
and the source of all new strong links used

*/
void PUZZLE::NestedMulti(BFTAG & elims) {
	for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		int nni = chx.ncd, aig2 = 0; 
		BFTAG zt;
		zt.SetAll_1();
		zt = zt.FalseState();
		BFTAG ttt = allsteps;
		ttt |= elims;
		zt -= ttt;
		if(chx.type - SET_base)
			continue;
		// must be  n false 
		for(int i = 0; i < nni; i++) { // max one free 
			USHORT cd = chx.tcd[i], j = cd << 1; // candidate in tag form
			if(cum->On(j)) {
				aig2 = 1;
				break;
			}// set assigned
			if(cum->Off(j ^ 1))
				zt &= dn.t[j];
		}
		if(aig2 || zt.IsEmpty())
			continue;	// if ok second round for action	
		for(int i = 3; i < puz.col; i += 2) {
			if(!zt.On(i))
				continue;
			BFTAG bfs;
			USHORT istored = 0, istoref = 0, tot_count = 0;
			int era=0; // one error somewhere debugging data
			for(int i2=0;i2<nni;i2++) {
				USHORT cd = chx.tcd[i2], j = cd << 1; // candidate in tag form
				if(cum->On(j ^ 1)) {
					bfs.Set(j ^ 1);
					continue;
				}// already false

				// here can be direct and this is not done in search chain
				// dummy cycle if direct to have common process

				BFTAG wch = dpn.t[j]; 
				USHORT tt[50], itt = 2; tt[0] = j; tt[1] = i;
				if(wch.Off(i)) {

					int erb=0,  // to catch an error in that sequence
					npasch = wch.SearchChain(dpn.t, j, i);	
					if((!npasch) || (npasch > 40))
						erb=1;  // should never happen  debug later

					else{

						itt = npasch + 2; 
					  if(wch.TrackBack(dpn.t, j, i, tt, itt, i)) 
						  erb=2;// intercept error for debugging
					}

					if(erb) 
						era=1;

					else {
				
				    // must add the source for the new strong links
				    for(int j = 1; j < itt - 1; j++) { // all strong links 
					   if((!(tt[j]&1)) || (tt[j+1]&1))
						   continue;
					   const CANDGOSTRONG * w = tcandgo.Get(tt[j], tt[j+1]);
					   if(w) bfs |= w->source;// it is a new strong link		
				    }
				    istoref = tstore.AddChain(tt, itt);
				    if(!istored)
					   istored = istoref;
				   tot_count += itt;

			       }// end else not erb
				}  //end if
			} // end i2

            if(era){ // stop and log message to see
              cerr <<"found failure multi nested"<<endl;
			  stop_rating=1;
			  elims.SetAll_0();
			  return;
			}

			 // here a case to be investigated deeper
			 //.2..5......71....668.......2..7....8..8...3....1..4.9..1.2....7....9.4.......3.5. ED=0.0/0.0/0.5
			 // no error in detailed chains
			 // but nothing stored, so all was defined ???
			 // good example to check the process

			if(istored) { //should always be
				USHORT ii = tstore.AddMul(istored, istoref);
				tsets[i] =- tcandgo.itt;
				elims.Set(i); 
				tcandgo.tt[tcandgo.itt++].Add(ii, bfs, tot_count); 

                if(opp)  // print it it test mode
	             {EE.E("new eliminated");   zpln.ImageTag(i); 
		          EE.E("  ichain="); EE.E(tstore.ise); 
		          EE.E("  stored as "); EE.Enl(tsets[i]); 
                 }
			}
			
		} // end tag i
	}// end ie
}




/*  create a reduced tdb without
    the candidate sutdied
	all candidates forced or cleared if the candidate studied is true
	including all new strong links generated
	and eliminations depending on the level

	at level 4 the process is dynamic
	so the chain must include derived links
*/
///
void PUZZLE::Gen_dpn(USHORT tag)
{          // create the reduced set of tags check tagelim empty 
 dpn.Init(); 
 BFTAG    * tdp=hdp_base_nested.t;


 for (int j=2;j< puz.col;j++) 
	{if(j==tag) continue; // don't process the start point
	 if(allsteps.On(j)) continue; // that tag is defined
	 if(allsteps.On(j^1))
		 continue; // that tag is defined as well (to check)
      dpn.t[j] = tdp[j];
	  dpn.t[j] -= allsteps; // reduce the primary weak links
	 }
  if(0)   {Image(allsteps,"allsteps at gen time",0);
		   dpn.Image();
          }
 if(rbase>100){  //level 4 must find  derived weak links
	// this must be a specific process working on reduced sets;
	// only one step of derivation is made to start
	// must be the same as in dpn short
	   chain4_dpn=dpn; //store dpn for nested dynamic expansion
       zcx.DeriveDynamicShort(allsteps,dpn,dn);

 }
  dn.ExpandAll(dpn);
 }

//--------------------------------------------------
int PUZZLE::GoBackNested(USHORT tag) {
	if(0 && nested_print_option ) {
		EE.E("goback");zpln.ImageTag(tag);
		EE.E(" npas=");EE.Enl(npas);
	}

	USHORT itret1=0,nestedlength=0;
	itret=0;
	BFTAG bf; 
	tret[itret++] = tag;
	bf.Set(tag);
	while(itret1 < itret && itret < 300) { // solve each entry back
		USHORT x = tret[itret1], aig = 1; // first locate where x has been loaded
		int index = tsets[x];
		if(0 && nested_print_option) {  
			EE.E("go back look for ");
			zpln.ImageTag(x);
			EE.E(" index= ");EE.E( index);
			EE.E(" itret1=");EE.E(itret1);EE.E(" itret=");EE.Enl(itret);
		}
		for(int i = 0; i <= npas; i++) {
			if(steps[i].On(x)) {
				aig=0; 
				if(i) {  // not initial assumption
	              if(!index){ // this is a direct step
	                 int z=0,ia=i-1;

		             // take a parent already there in priority
		             // note  this should be extended to SET_sets

	                 for(int i2=0;i2<=i;i2++){
		                for(int j=0;j<itx[i2];j++){
	                       USHORT y=tx[i2][j]; 
			               if(to[y].On(x) && bf.On(y)) {
					         z=y;break;
			               }	
			            }
			            if(z)break;
		             }
	                 if(!z )  { // none already there, take the earliest possible

	                  for(int i2=0;i2<=i;i2++)  {

				         // prospect that step

			            for(int j=0;j<itx[i2];j++){ 
	                        USHORT y=tx[i2][j]; 
			                if(to[y].On(x))  {
					           z=y; 
                               tret[itret++]=z;
			                   bf.Set(z);			          
					           break;			   
					         } // end if
	                    }// end step prospect

				        if(z)   break;  // stop at earliest stepfound

			            }// end loop on steps

			          } // end if(!z)

			          if(!z) {  // not found should never be
						  aig=1;
						  if(Op.ot){// debugging sequence
                             EE.E("debug for goback direct not found target ");
							 zpln.ImageTag(tag);EE.E(" npas=");EE.Enl(npas);
							 EE.E(" x= "); zpln.ImageTag(x);
							 EE.E(" istep");EE.Enl(i);

							 for (int iw=0;iw<=i;iw++) {
								  EE.E(iw); Image(steps[iw]," step",0);
							 }
                             for (int iw=0;iw<=i;iw++) {
								    EE.E(iw); EE.E(" step tx "); 
								    for(int j=0;j<itx[iw];j++){ 
								    zpln.ImageTag(tx[iw][j]);
								    EE.Esp();
									}
									EE.Enl();
							 }							 
							 zcf.h.dp.Image();
							 EE.Enl("\n tsets table");
							 for(int iw=2;iw<640;iw++) 
								 if(cumsteps[i].On(iw)){
                                 zpln.ImageTag(iw); EE.E(" tset ");								    EE.Esp();
								 EE.Enl(tsets[iw]);
								 }

						  }
					  }
		           } // end index=0

	   			   else if(index > 0) {
						// it comes from a set, we know which one
						//   but  may be a shorter path using anoter set
						// we take the shortest size giving that candidate 
						//   using found candidates (false)  (to code)
						SET chx = zcx.zc[tsets[x]];
						if(0 && nested_print_option) {
							EE.E("set");
							chx.Image();
							EE.Enl();
						}
						int n = chx.ncd; 
						if(chx.type == SET_set)
							n--; // for a set, forget the event
						else if(n > 3) { // if n>3 try to find a shorter set
							for(int j = tsets[x] + 1; j < zcx.izc; j++) {
								SET chxj = zcx.zc[j];
								if(chxj.type == SET_set)
									break; // only sets
								int nj = chxj.ncd; 
								if(nj >= n)
									continue; // keep the first lowest only
								int aig = 0;
								for(int k = 0; k < nj; k++) {
									USHORT y = chxj.tcd[k] << 1;
									if(cumsteps[i].On(y ^ 1))
										continue;
									if(y == x)
										aig = 1; // must be 'x' onece
									else
										aig = 0;
									    break;
								}
								if(aig) {
									n = nj;    // replace the set by the new one
									chx = chxj;
									if(n == 3)
										break;  // stop at first 3 cand reached
								}
							} // end j
						}  // end if n>3

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
					else {  // index <0 this is a nested elimination
						CANDGOFORWARD w = tcandgo.tt[-index];
						nestedlength += w.count;
						BFTAG bfn( w.source);
						bfn -= bf; // find new tags needed
						USHORT newCount;
						bfn.String(&tret[itret], newCount);// put them in the list "to explain"
						itret += newCount; // and adjust the count
						bf |= bfn;  // update the list of tags icluded
						if(0){///
							EE.Enl("nested elimination");	
                            tstore.Print(w.index);
							Image(bfn,"new tags needed",0);
							EE.Enl();
						}
						
					}
				}
			i = 100;// force end of process after it has been found

			}
		}  // end i
       if(aig || itret>300) {
		   if(aig)
		   cerr <<"stop goback pour aig=1  "<<endl;  
		   else
		   cerr <<"stop goback pour iret trop grand "<<endl;  
	       stop_rating=1;
	        if( Op.ot) {
				EE.E("go back nested invalid situation itret1=");
			    EE.E(itret1);
			    EE.E(" itret=");
			    EE.E(itret);
				EE.E(" x=");zpln.ImageTag(x);
				EE.E(" index=");EE.E(index);
				EE.E(" goback");zpln.ImageTag(tag);
		        EE.E(" npas=");EE.Enl(npas);
				for(int i=0;i<=npas;i++){
					EE.E(i);
					Image(steps[i],"step",0);
                    EE.Enl();				
				}

		    }

	       opp=0;
	       return 0; // not found, should never be
       }	
   	   itret1++;
		if(0 && Op.ot) {
			EE.E("go back end step   itret1=");
			EE.E(itret1);
			EE.E(" itret=");
			EE.Enl(itret);
		}
	}
	if(nested_print_option && Op.ot) { // printing in increasing order of generation
		EE.Enl(" eliminations justification ");
		for(int i = 0; i <= npas; i++) {
			for(int j = 0; j < itret; j++) { 
				if(steps[i].On(tret[j])) {
					USHORT wt = tret[j]; 
					zpln.ImageTag(wt); // print the tag and look for explanation
					int index = tsets[wt];
					if(!index)
						EE.Enl();  // direct no comment
					else if(index > 0) { // it comes from a set, we know which one
						SET chx = zcx.zc[index];
						EE.E(" through set ");
						chx.Image();
						EE.Enl(); 	
						if(chx.type == SET_set){// then print more
							int ev=chx.tcd[chx.ncd-1]- event_vi;
							tevent.t[ev].ImageShort();
						}

					}
					else {  // index <0 this is a nested elimination
						CANDGOFORWARD w = tcandgo.tt[-index];
						EE.E(" through nested chain(s)  count=");
						EE.Enl(w.count);
						tstore.Print(w.index);
					}
					EE.Enl();
				}
			}
		}
		EE.E("return itret=");
		EE.E(itret);
		EE.E(" nestedplus=");
		EE.Enl(nestedlength);
	}
	 
	 // this is the right place for a bi value adjustment
	 // bi value if start false
	 //   and "step one and result only one true in common"
	 // this is processsed by serate as a cell or region bivalue
	 int biv=0;
	 if(tx[0][0] ^1) {// this is a false start
	      BFTAG bw(bf);
	      bw &= steps[1];
		  if(bw.Count()==1 ){ // must be one used in the first step
			  USHORT tw[2],itw;
			  bw.String(tw,itw);
			  if(!(tw[0] ^1))  // must be a true
			     biv=-1;
		  }
	 }

	return itret + nestedlength ; //+ biv; waiting examples for validation
}




//former (r96) _03b_puzzle_chains.cpp start

void PATH::PrintPathTags() {
	zpln.PrintImply(pth,ipth);
}



ZGROUPE::ZGROUPE (PUZZLE * parent) {
	parentpuz = parent;
	int i,j;
	BF81 z0,z1; 
	z0.SetAll_0(); 
	z1.SetAll_1();
	for(i=0;i<81;i++)
	{
		z[i]=z0;
		z[i].Set(i);
	}  // les z de 81 cases
	int k,n=0,il=81;                    // puis les z de groupe
	for(i=0;i<18;i++)
		for(j=0;j<3;j++)
		{
			z[il].SetAll_0();
			if(i<9)
				for(k=0;k<3;k++)
					z[il].Set(n++);
			else
			{
				int c=i-9; 
				for(k=0;k<3;k++)
					z[il].Set(c+27*j+9*k);
			}
		il++; 
		}
	ReInit();
}

//------ pour les additions
USHORT ZGROUPE::Charge(const BF81 &ze)
{
	if(ze.IsEmpty())
	{
		parentpuz->Estop( "zgs groupe IsEmpty");
		return 0;
	}
	for(int i=0;i<iz;i++) 
		if(z[i]==ze) 
			return i;
	if(iz<zgs_lim) 
	{
		z[iz++]=ze ;
		return (iz-1);
	}
	parentpuz->Elimite( "ZGS");return 0;
}

//BF81 * CANDIDATE::GetZ() const {
//	return &zgs.z[ig];
//}     

void CANDIDATE::Clear(){T81t[ig].Change(ch); }
void CANDIDATE::Image(int no) const {EE.Esp();if(no)EE.E("~");
                     EE.E(ch+1);EE.E(t81f[ig].pt); }

/* creates the table of candidates
   and the direct index cell+ digit -> cand
   looks for true candidates
   init table zcf
   set up the usefull limit for BFCAND and CB1024 
 */


void CANDIDATES::Init() {
	ip=1;
	candtrue.SetAll_0();
	for(int i = 0; i < 9 * 81; i++)
		indexc[i]=0;
	for(UCHAR i = 0; i < 81; i++) {
		if(T81t[i].v.ncand < 2)
			continue;
		BF16 chs = T81t[i].v.cand;  
		for(UCHAR j = 0; j < 9; j++)
			if(chs.On(j)) {
				zp[0].Charge(i, j); 
				if(puz.solution[i] == (j + '1'))
					candtrue.Set(ip);
				indexc[81 * j + i] = Charge0();
			}
	}
	puz.col=2*ip;
}
/* just put zp[0] in the next position 
   check for free room just in case
   if no free room, puz.stop_rating is set to 1 thru Elimite
*/
USHORT CANDIDATES::Charge0()
{if(ip>=zpln_lim){  parentpuz->Elimite("TZPLN"); return 0;}
 USHORT ir=ip; zp[ip++]=zp[0]; return ir;}

/* send in INFERENCES all weak links
   if it is a bi value, then 2 weak links are created
      a - b  and   ~a - ~b
   one entry for 'X' mode and one entry for 'Y' mode
   in 'X' mode we check for no double entry

*/

// this is a process specific to 'Y' mode
// only cell bivalues + 
void  CANDIDATES::CellStrongLinks()
{ for(int i=0;i<81;i++) if(T81t[i].v.ncand==2) 
  {iptsch=0; 
   for(int ich=0,jp=i;ich<9;ich++,jp+=81)
	    {int j=indexc[jp];if(j)  ptsch[iptsch++]=j;  }
	 zcf.LoadBivalue(ptsch[0],ptsch[1]);
  }
}
/* generate weak and strong links from cells.
   if biv=1, no generation of strong links
   (if not, biv=3)
*/
void  CANDIDATES::CellLinks() 
{ el=0; // to shunt the filter in WeaklinksD
 for(int i=0;i<81;i++) if(T81t[i].v.ncand<2) continue;
else 
  {iptsch=0;  
   for(int ich=0,jp=i;ich<9;ich++,jp+=81)
    {int j=indexc[jp];if(j) ptsch[iptsch++]=j;   }
   if(iptsch==2) { zcf.LoadBivalue(ptsch[0],ptsch[1]);
                   zcf.LoadBase(ptsch[0],ptsch[1]);}
   else  if(iptsch)WeakLinksD();
 }
}
void  CANDIDATES::RegionLinks(USHORT ich,int biv)
{for (el=0;el<27;el++)
   { iptsch=0;  if(puz.alt_index.tchbit.el[el].eld[ich].n <2 )continue;
     for(int i=0;i<9;i++)
       {USHORT w=indexc[divf.el81[el][i]+81*ich]; 
		if(w) ptsch[iptsch++]=w;}
	if(iptsch==2 && biv) {zcf.LoadBivalue(ptsch[0],ptsch[1]);
	                      zcf.LoadBase(ptsch[0],ptsch[1]);}
	else    if(iptsch)WeakLinksD();
   }
}
void  CANDIDATES::WeakLinksD()
{for(int j=0;j<iptsch-1;j++)// look for all pairs
     {USHORT p1=ptsch[j],ig1=zp[p1].ig;
      for(int k=j+1;k<iptsch;k++)
       { USHORT p2=ptsch[k],ig2=zp[p2].ig;
	     // don't generate twice if box/row or box/column 
	    if(el>17 &&   // it is a box
		   (t81f[ig1].el==t81f[ig2].el  ||  t81f[ig1].pl==t81f[ig2].pl ) )continue;
	   zcf.LoadBase(p1,p2)    ;
       }  }}

//---------- gen sets of candidates in a row column box 
/* changed the order of generation for 2 reasons
   closer to serate mode
   better chance to have same rating for morphs

   first generate box sets
   and do it in increasing order of the set size

*/

void CANDIDATES::GenRegionSets()     
	// can only be one per row col box, only if more than 2 candidates
{  USHORT mch[10];    
    //  box row column increasing sise of sets
   for( int ncand=3;ncand<10;ncand++)
    for (int elx=0,el=18;elx<27;elx++,el++){
      if(el==27) el=0;
      for( int ich=0;ich<9;ich++) {
        USHORT nmch=puz.alt_index.tchbit.el[el].eld[ich].n,ipts=0;;
	    if(nmch-ncand) continue;  
	    BF81 zel=divf.elz81[el]&puz.c[ich];  
		for(int j=1;j<ip;j++){
            if(zp[j].ch-ich )continue;
            if(zel.On(zp[j].ig) ) mch[ipts++]=j; 
        }
       zcx.ChargeSet(mch,nmch,SET_base);
    }
   }

}
void  CANDIDATES::GenCellsSets()
{for(USHORT i=0;i<81;i++)
  {USHORT n= T81t[i].v.ncand; if(n<3 || n>chx_max) continue;    
   BF16 ch8=T81t[i].v.cand;  
   USHORT nm=0,tm[9];    for(int j=0,jp=i;j<9;j++,jp+=81) if(ch8.On(j))
     {tm[nm++]=indexc[jp];}
     zcx.ChargeSet(tm,nm,SET_base);  
    }  }

void CANDIDATES::Liste() const
{EE.Enl("Liste des candidats ");
 for(int j=1;j<ip;j++)  {zp[j].Image();   EE.Enl();}  }

void CANDIDATES::ListeIndexc()
{EE.Enl("Liste des candidats via index ");
 for(int i=0;i<81;i++) for(int ich=0;ich<9;ich++) 
 {USHORT j=indexc[ich*81+i];
 if(j)  {zp[j].Image();   EE.Enl();}  }
}

void  CANDIDATES::PrintPath(USHORT * tp,USHORT np) const
{if(!Op.ot) return; EE.Echem();
for(int i=0;i<np;i++)
{ImageTag(tp[i]);
if(i<np-1) if(i&1)EE.Esl(); else EE.Ewl(); } 
EE.Enl();}

void  CANDIDATES::PrintImply(const USHORT * tp,USHORT np) const {
	if(!Op.ot)
		return;
	EE.Echem();
	for(int i = 0; i < np; i++) {
		//const USHORT px = tp[i];
		ImageTag(tp[i]);
		if(i < np - 1)
			EE.E(" -> ");
	} 
	EE.Enl();
}

void  CANDIDATES::PrintListe(USHORT * tp,USHORT np,int modetag) const
{if(!Op.ot) return; EE.E("candidats");
 for(int i=0;i<np;i++)
  {USHORT px=tp[i];  
  if(modetag) ImageTag(px); else  zp[px].Image();
  EE.Esp();  } 
 EE.Enl();}




BFTAG SQUARE_BFTAG::done[50]; 
USHORT  SQUARE_BFTAG::ich, SQUARE_BFTAG::mode, SQUARE_BFTAG::length, SQUARE_BFTAG::mmfin, SQUARE_BFTAG::maxlength;
PATH   SQUARE_BFTAG::path,SQUARE_BFTAG::pathr;
USHORT SQUARE_BFTAG::maxichain,SQUARE_BFTAG::maxsearch,SQUARE_BFTAG:: parsediag,SQUARE_BFTAG:: parsecount;
BFCAND SQUARE_BFTAG::tbf_end,SQUARE_BFTAG::tbf_endok; 

void SQUARE_BFTAG::Parents(USHORT x) {
	parents.SetAll_0();
	for(int i=2;i< puz.col;i++)
		if(t[i].On(x))
			parents.Set(i);
}

/* the key routine expanding pieces of path out of a->b elementary components
   ExpandAll is the fast mode using in bloc what has already been done  
   partial mode are found in the BFTAG functions
   */ 

void SQUARE_BFTAG::ExpandAll(SQUARE_BFTAG & from) {
	(*this) = from; // be sure to start with the set of primary data
	BFTAG t1, t2;
	USHORT p[640], np;
	for(int i = 2; i < puz.col; i++) {
		t[i].String(p, np);
		while(np) {
			t2 = t[i];
			for(int j = 0; j < np; j++) {
				if(p[j] == i)
					continue;
				t[i] |= t[p[j]];
			} // j
			t1 = t[i]; //all bits
			t1 -= t2; //bits set on this pass = all bits excluding bits set on the previous passes
			t1.Clear(i);
			t1.String(p, np);
		}
	}
}// end i   proc


void SQUARE_BFTAG::ExpandShort(SQUARE_BFTAG & from ,int npas)
{(*this)=from; // be sure to start with the set of primary data
 for( int i=2;i< puz.col;i++)
  {if (t[i].IsEmpty())continue;   int n=1,pas=0;
   while(n && (++pas<npas)) {
	   n=0;
	   for(int j=2;j< puz.col;j++)
		   if((j-i) && t[i].On(j)) {
			   BFTAG x(from.t[j]);
			   if(x.substract(t[i])) {
				   t[i] |= x;
				   n++;
			   }
   }} // end j  while
  } }// end i   proc

/* that table is prepared for the derivation of weak links
   the "from" table is the table of implications
   */
void SQUARE_BFTAG::AllParents(const SQUARE_BFTAG & from) {
	t[0].SetAll_0();
	for(int i = 1; i < puz.col; i++)
		t[i] = t[0];
	for(int i = 2; i < puz.col; i++) {
		//for(int j = 2; j < puz.col; j++) { //v 0 by GP
		//	if(from.t[i].On(j)) {
		//		t[j].Set(i);
		//	}
		//}
		USHORT ind[640], maxInd; //v 1 by MD, 7x speed
		from.t[i].String(ind, maxInd);
		for(int j = 0; j < maxInd; j++) {
			t[ind[j]].Set(i);
		}
	}
	// EE.Enl("parents"); Image();
}


/* select tags having a chance to have the shortest path
   this is a filter for the contradiction chains of the form
   x -> ~a  and   ~x -> ~a
   for each tag set to 1 in "x" 
     compute the total of steps needed to get it with both starts
	 keep only tags below  minimal total + n (2) 
   */
void SQUARE_BFTAG::Shrink(BFTAG & x,USHORT it)
{USHORT tt[300],itt,     // tags in table
        ntt[300],min=200,j,lim,ir; 
 x.String(tt,itt);  // put tags in table	
 for(int i=0;i<itt;i++) {
    j=tt[i];
    lim=min+1;
    ir=ExpandToFind(it,j,lim)+ExpandToFind(it^1,j,lim);
    ntt[i]=ir;
    if(ir<min) min=ir;
 }
 lim=min+1;
 for(int i=0;i<itt;i++) {
    if(ntt[i]> lim)
      x.Clear(tt[i]);
 }
}
/* subroutine for Shrink
*/

int SQUARE_BFTAG::ExpandToFind(USHORT td,USHORT tf,USHORT lim){
  BFTAG tagw(t[td]);  // tag to expand
  int npas=0;
  while(1){  // endless loop till tf found or no more expansion or min +2 passed
      if(tagw.On(tf)) return npas;
      if(npas++>lim) return 100; // 
         // now expand one more step
      int n=0; // to check whether something is done
      for(int j=2;j< puz.col;j++)    if( tagw.On(j)) {
	       BFTAG x(t[j]);
	       if(x.substract(tagw)) {
				   tagw |= x;
				   n++;
			}
      }
  if(!n) return 100;

  }
}

/* That process is valid only if no derived weak link has been produced
   The search finds the lot of shortest eliminations same length
   return 0 if nothing
   return the length if elimination(s) have been found

   from is the SQUARE_BFTAG of elementary weak links
   elims is set to 1 for all tags found in that process

   only "true" state of non valid candidates are expanded
*/

int SQUARE_BFTAG::SearchEliminations(SQUARE_BFTAG & from, BFTAG & elims) {
	int npas=0, aigt=0;
	elims.SetAll_0();
	(*this) = from; // be sure to start with the set of primary data
	while(1) {
		int aig=1; // to detect an empty pass
		npas++;
		for(int i = 2; i < puz.col; i += 2)  // only "true" state
			if(zpln.candtrue.Off(i >> 1) &&  // candidate not valid
				t[i].IsNotEmpty()               // should always be
				)
			{
				for(int j = 2; j < puz.col; j++)
					if((j - i) && t[i].On(j)) {
						BFTAG x=from.t[j];
						//x -= t[i];	  
						//if(x.IsNotEmpty()) {
						if(x.substract(t[i])) {
							t[i]|=x;
							aig=0;
						}
					}
					if(t[i].On(i ^ 1)) { // an elimination is seen
						elims.Set(i);
						aigt=1;
					}
			}   
			if(aigt) return npas; // eliminations found
			if(aig) return 0;     // empty pass
	}// end while
}


//<<<<<<<<<<<<<<<<<<<<<<<
void SQUARE_BFTAG::Image() {EE.Enl( "Image zone tdb");
 for(int i=2;i< puz.col;i++)  if(t[i].IsNotEmpty())  puz.Image(t[i]," ",i);   }


int INFERENCES::DeriveCycle(int nd, int nf, int ns, int npas) {
	load_done=0;  // to check if something new comes
	hstart = h; // copy to have the situation at the start
	zcx.Derive(nd, nf, ns);
	if(!load_done) return 0;
	if(!npas)
		h.d.ExpandAll(h.dp);
	else
		h.d.ExpandShort(h.dp, npas);
	return 1;
}



// entry for basic weak link a - b
void INFERENCES::LoadBase(USHORT cd1 ,USHORT cd2) {
	Entrep(cd1 << 1, cd2 << 1);
}

// entry for bi value  a = b thru a - b and ã - ~b   
void INFERENCES::LoadBivalue(USHORT cd1, USHORT cd2) {
	load_done=1;
	Entrep((cd1 << 1) ^ 1, (cd2 << 1) ^ 1);  
	isbival.Set(cd1);
	isbival.Set(cd2);
}

// entry for weak link derived from a set  a -> b
void INFERENCES::LoadDerivedTag(USHORT tg1, USHORT cd2) {
	load_done=1;
	Plusp(tg1, cd2 << 1);  //  a -> b
}

// entry for event  a - b
void INFERENCES::LoadEventTag(USHORT tg1, USHORT cd2) {
	load_done=1;
	Plusp(tg1, (cd2 << 1) ^ 1); // a -> ~b
}

// entry for direct event  ~a - b
void INFERENCES::LoadEventDirect(USHORT cd1, USHORT cd2) {
	load_done=1;
	Plusp((cd1 <<1 ) ^ 1, (cd2 << 1) ^ 1); 
}

 /* In that process, we do in the same pass the search for
    x cycle;chain  or  y cycle;chain  or XY cycle;chain

	if nothing comes from  ?chain, there is no need to search the loop
	in case we have results from ?chain,
	we expand all and collect 
	  all eliminations ("true status"), 
	  all tags in loop
    
	for each elimination, we first look for the chain, then for the loop if any
    tchain takes care of the smaller ratings


  Note we look only in that process,  elimination found thru a "true"  state of a candidate
  nota : to get a "false" elimination we need
  ~a - ~b = b - ..... - x = ~x - ~a (only possible weak link ~x - ~y)
  but then  b - a = ~a - ~x = x ..... - b   always exists
*/

void INFERENCES::Aic_Cycle(int opx) {  // only nice loops and solve them
	// first collect tags in loop
	//if(opx==2) h.dp.Image();
	BFTAG elims;
	int npaselim = h.d.SearchEliminations(h.dp, elims);
	if(!npaselim) return; //nothing to find
	h.d.ExpandAll(h.dp); // 	
	BFTAG xi;
	xb.SetAll_0();
	xi.SetAll_0();// collect tags in loop ok and "to eliminate in true state"
	for(int i = 2; i < puz.col; i++) {
		if(h.d.Is(i, i) && (h.d.Is(i ^ 1, i ^ 1)))
			xb.Set(i);  
		if(h.d.Is(i, i ^ 1) && (!(i & 1)))
			xi.Set(i);
	}
	if(xi.IsEmpty())
		return;  
	if(0 && Op.ot) { 
		puz.Image(xi,"candidates potential  eliminations", 0);
	}

	// now check all eliminations for a chain or a cycle    
	for(int i = 2; i < puz.col; i += 2) {
		if(!xi.On(i))
			continue;
		if(0 && Op.ot) {
			EE.E("\n\nanalyzing ");
			zpln.ImageTag(i);
			EE.Enl();
		}
		// look for a chain
		if((opx - 2)) //  || (!Op.ocopybugs)) // skip Y chains in serate mode
		{
			BFTAG wch = h.dp.t[i];
			int npasch = wch.SearchChain(h.dp.t, i, i ^ 1);
			if(0 && Op.ot) {
				EE.E(" npasch= ");
				EE.Enl(npasch);
			}
			if(!npasch)
				continue; // should never happen
			int ratch = puz.tchain.GetRatingBase((opx == 3) ? 70 : 66, npasch + 1, i >> 1);
			if(ratch) { // chain is accepted load it (more comments in test mode)
				if(Op.ot) {
					ExplainPath(wch, i, i^1, npasch + 2, i ^ 1);
				}
				puz.tchain.LoadChain(ratch, "chain", i >> 1);	
			}
		}
		//--------------------------- now look for a loop 
		BFTAG w = h.dp.t[i];
		w &= xb; 
		if(0 && Op.ot)
			puz.Image(w,"loop contacts",i);
		if(w.Count() < 2)
			continue; // must have contact with 2 candidates ( false state )
		//in 'X' mode cycle eliminations are done only in the same region
		//first, collect all contacts in a table  and process all pairs
		USHORT tt[100], itt;
		w.String(tt,itt);
		for(int i1 = 0; i1 < itt - 1; i1++) {
			USHORT t1 = tt[i1] ^ 1; // t1 is turned to "true" 
			for(int i2 = i1 + 1; i2 < itt; i2++) {
				USHORT t2 = tt[i2];  // t2 kept to "false"
				if(h.dp.t[t1].On(t2)) {
					//  same cell or region
					// except in 'XY' mode, must be the same digit
					if(opx-3) {
						CANDIDATE candt1 = zpln.zp[t1 >> 1], candt2 = zpln.zp[t2 >> 1];
						if(candt1.ch-candt2.ch)
							continue; 
					}
					// now we want the xycle crossing t1 and t2.
					// by construction, this is a weak link, never a strong link
					BFTAG wstart;
					wstart.Set(t2);  //we start a search with t1 -> t2
					int npascycle = wstart.SearchCycle(h.dp.t, t1,xb);
					if(!npascycle)
						continue; // could  ben ??
					int rat = puz.tchain.GetRatingBase((opx == 3) ? 70 : 65, npascycle + 1, i >> 1);
					if(!rat)
						continue;// chain is accepted load it (more comments in test mode)
					if(Op.ot) {
						ExplainPath(wstart, t1, t1, npascycle + 2, t2);
					}
					puz.tchain.LoadChain(rat, "cycle", i >> 1); 
				}
				else if(opx == 2) // not allowed in 'X' mode and no interest in 'XY'mode
					Aic_Ycycle(t1, t2 ^ 1, xb, i >> 1); // both turned to true 
			}
		}// end for i1 i2
	}// end for i
}

/* we are looking in 'Y' mode for a loop crossing 2 points inside the loop.
   me must find the shortest loop
   that processs can be very long, so it must be designed carefully
   the general idea is the following
   look in priority for a candidate (t1 or t2) with only 2 possible starts in the loop.
   If this exists, take that point as start, if not, 
   take the smallest number of starts and try all of them

   in the search, the strategy is to take the shortest path.
   Doing that, we can kill all possibilities to go back.
   If there is no way back, we try the start from the second candidate in the cell
   
   That process can miss some possibilities, what is accepted.

*/
void INFERENCES::Aic_Ycycle(USHORT t1, USHORT t2, const BFTAG &loop, USHORT cand) {
	// forget if not same digit or t2 is ~t1
	if(cand == (t2 >> 1))
		return;

	CANDIDATE candt1 = zpln.zp[t1 >> 1];
	CANDIDATE candt2 = zpln.zp[t2 >> 1];
	if(candt1.ch - candt2.ch)
		return;

	//USHORT ct1 = (h.dp.t[t1] & loop).Count();
	BFTAG ttt = h.dp.t[t1];
	ttt &= loop;
	USHORT ct1 = ttt.Count();
	if(ct1 < 1)
		return; // should never be
	if(ct1 < 2) {
		Aic_YcycleD(t1, t2 ^ 1, loop, cand);
		return;
	}

	//USHORT ct2 = (h.dp.t[t2] & loop).Count();
	ttt = h.dp.t[t2];
	ttt &= loop;
	USHORT ct2 = ttt.Count();
	if(ct2 < 1)
		return; // should never be

	//if(ct1 < 1 || ct2 < 1)
	//	return; // should never be

	//if(ct1 < 2) {
	//	Aic_YcycleD(t1, t2 ^ 1, loop, cand);
	//	return;
	//}  

	if(ct2 < 2) {
		Aic_YcycleD(t2, t1 ^ 1, loop, cand);
		return;
	}  
	USHORT wt1 = t1, wt2 = t2;
	if(ct2 < ct1) {
		wt1 = t2;
		wt2 = t1;
	}
	Aic_YcycleD(wt1, wt2 ^ 1, loop, cand);
}

/* now the all possibilities with start t1 and crossing t2
   we must start with a weak link so we exclude as start the strong link
*/
void INFERENCES::Aic_YcycleD(USHORT t1,USHORT t2, const BFTAG &loop,USHORT cand) { // up to 4 starts
	USHORT tt[20], itt, lg = 200;
	PATH resf, resw;
	USHORT tagc = cand << 1, tagcn = tagc ^ 1;

	//(h.dp.t[t1] & loop).String(tt, itt); // starts in table
	BFTAG ttt = h.dp.t[t1];
	ttt &= loop;
	ttt.String(tt, itt); // starts in table

	for(int i = 0; i < itt; i++) {
		if(h.dp.t[tt[i]].On(t1))
			continue; // this is a strong link, skip it
		if(tt[i] == tagcn)
			continue; // don't start with the elimination
		int lw = Aic_Ycycle_start(t1, tt[i], t2, loop, resw);
		if(lw) {
			//if(resw.On(tagcn)) continue; // cand must not be in the loop
			//if(resw.On(tagc)) continue; // to be validated
			if(lw < lg) {
				lg=lw;
				resf = resw;
			}
		}
	}
	// lg final count, resf final result finish the task
	if(lg > 100) {
		Aic_YcycleD2(t1,t2,loop,cand);
		return;
	}// try the second way
	int rat = puz.tchain.GetRatingBase(65, lg, cand);
	if(!rat)
		return;
	// chain is accepted load it and in test mode, find more comments
	if(Op.ot) {
		EE.Enl("Y cycle out of the region");
		resf.PrintPathTags();
	}
	//  ExplainPath(resf,t1,t1,lg+2,t2);  }
	puz.tchain.LoadChain(rat, "Y cycle", cand); 
}

/* first process failed
   try starttin from the second candidate in the cell containing t1
*/

void  INFERENCES::Aic_YcycleD2(USHORT t1x, USHORT t2x, const BFTAG & loop, USHORT cand)// up to 4 starts
{if(0) {EE.E("Aic_Ycycle d2"); 
         EE.E(" t1x=");zpln.ImageTag(t1x);
         EE.E(" t2x"); zpln.ImageTag(t2x); 
         EE.E(" cand="); zpln.Image(cand);EE.Enl();
		 puz.Image(h.dp.t[t1x^1],"liens",0);EE.Enl();
        }
USHORT t2=t2x^1,t1=0; // new target is  "on"
 USHORT tt[20],itt,lg=200;   PATH resf,resw;
 h.dp.t[t1x^1].String(tt,itt); // starts in table
 if(itt<1) return;// should never be
 t1=tt[0];
 if(0) {EE.E("Aic_Ycycle d2 go"); 
         EE.E(" t1=");zpln.ImageTag(t1);EE.Enl();
        }
 USHORT tagc=cand<<1,tagcn=tagc^1;

 //(h.dp.t[t1] & loop).String(tt,itt); // starts in table
 BFTAG ttt = h.dp.t[t1];
 ttt &= loop;
 ttt.String(tt,itt); // starts in table

 for(int i=0;i<itt;i++) 
  {if(h.dp.t[tt[i]].On(t1)) continue; // this is a strong link, skip it
   if(tt[i]==tagcn) continue; // don't start with the elimination
   int lw=Aic_Ycycle_start(t1,tt[i],t2,loop,resw);
   if(lw)
    {//if(resw.On(tagcn)) continue; // cand must not be in the loop
     //if(resw.On(tagc)) continue; // to be validated
     if(lw<lg){lg=lw;resf=resw;}
    }
  }
 // lg final count, resf final result finish the task
 if(lg>100) return; // should never happen
 int rat=puz.tchain.GetRatingBase(65,lg,cand);
 if(!rat) return;
     // chain is accepted load it and in test mode, find more comments
 if(Op.ot){EE.Enl("Y cycle out of the region");
           resf.PrintPathTags();   }
	     //  ExplainPath(resf,t1,t1,lg+2,t2);  }
 puz.tchain.LoadChain(rat,"Y cycle",cand); 
}

/* process one start  t1->t1a looking for t1
   and get back the count
   go first to t2
   then track back the path to clean the "done" filtering the process
   and continue to t1. send back the count and the BFTAG
*/
int INFERENCES::Aic_Ycycle_start(USHORT t1, USHORT t1a, USHORT t2, const BFTAG & loop, PATH & path) {
	if(0) {
		EE.E("Aic_Ycycle_start"); 
		EE.E(" start=");
		zpln.ImageTag(t1);
		EE.E(" thru=");
		zpln.ImageTag(t2); 
		EE.E(" first=");
		zpln.ImageTag(t1a);
		EE.Enl();
		// loop.Image("loop",0);
		EE.Enl();
	}
	BFTAG wstart;
	wstart.Set(t1a);  //we start a search with t1 -> t1a
	int npascycle = wstart.SearchCycleChain(h.dp.t, t1, t2, loop);
	if(!npascycle)
		return 0;
	path.ipth = npascycle + 2;  // itt set to last tag in the path
	if(wstart.TrackBack(h.dp.t, t1, t2, path.pth, path.ipth, t1a))
		return 0; // bug collecting the path 

	if(0) {
		EE.Enl("Aic_Ycycle_start end of first track back"); 
		path.PrintPathTags();
		EE.Enl();
	}

	// we go back with a reduced loop, restarting from t2

	BFTAG wstart2;
	wstart2.Set(t2);  //we start next  search from t2 
	BFTAG loop2(loop); // we cancel the forward path in the loop
	for(int i = 1; i < path.ipth; i++)
		loop2.Clear(path.pth[i]); // BFTAG equivalent to tt

	int npas2 = wstart2.SearchCycleChain(h.dp.t, t2, t1, loop2); // and now we continue to the end

	if(0) {
		EE.E("Aic_Ycycle_start after  second npas2=");
		EE.Enl(npas2);
		puz.Image(loop2,"loop2",0);
		EE.Enl();
	}

	// if npas2=0 it can be due to the fact that the first path lock the way back
	// we then try a start from the other candidate in the cell containing t1

	if(!npas2)
		return 0;
	PATH path2;
	path2.ipth = npas2 + 1;  // itt set to last tag in the path
	if(wstart2.TrackBack(h.dp.t, t2, t1, path2.pth, path2.ipth, t1))
		return 0; // bug collecting the path 

	if(0) {
		EE.Enl("Aic_Ycycle_start end of second track back"); 
		path2.PrintPathTags();
		EE.Enl();
	}

	// expand path1 with path2
	for(int i = 1; i < path2.ipth; i++)
		path.Add(path2.pth[i]);
	return (path.ipth - 1);  
}

/* we have found something in a forward step
  in test mode, we have to publish an equivalent path
  */
void INFERENCES::ExplainPath(BFTAG & forward, int start, int end, int npas, USHORT relay) {
	if(npas > 40) {
		EE.E("path too long in Explainpath npas=");
		EE.Enl(npas);
		return;
	}

	USHORT tt[50], itt = npas; 
	// EE.E("explain start=");zpln.ImageTag(start);EE.E(" end=");zpln.ImageTag(end);
	// EE.E(" npas=");EE.Enl(npas);
	// forward.Image("forward",start);
	forward.TrackBack(h.dp.t, start, end, tt, itt, relay);
	// puz.PointK(); // put a milestone and print the chain
	zpln.PrintImply(tt, itt);
}
/* done when higher rating  already found
   just eliminate candidates (true) without looking for loops
   replacing X Y and XY search
   */
int INFERENCES::Fast_Aic_Chain() {
	int ir=0;
	puz.TaggingInit();
	zpln.CellLinks();
	zpln.RegionLinks(1); 	         
	h.d.ExpandAll(h.dp); // 	
	for(int i = 2; i < puz.col; i += 2)
		if(h.d.Is(i, i ^ 1)) {
			zpln.Clear(i >> 1); 	// just clear it if not test mode
			ir++;
			if(0 && Op.ot) {
				EE.E("clear ");
				zpln.ImageTag(i);
				EE.Enl();
			}
			BFTAG wch = h.dp.t[i];
			int npasch = wch.SearchChain(h.dp.t, i, i ^ 1);
			ExplainPath(wch, i, i ^ 1, npasch + 2, i ^ 1);
		}
	return ir;
}





void SETS_BUFFER::GetSpace(USHORT *(& ps),int n) {
	ps = &zs[izs];
	izs += n;
	if(izs >= setsbuffer_lim) {
		ps=0;
		parentpuz->Elimite("SETS_BUFFER");
	}
}

void SET::Image() const {  // liste of candidates in the set
	if(!Op.ot)
		return;
	EE.E(type);
	EE.E(" set: ");
	int lim = ncd;
	if(type)
		lim--;
	for(int i = 0; i < lim; i++) {
		zpln.Image(tcd[i]);
		EE.Esp();
	}
	if(type)
		EE.E(tcd[lim]);
}

int SET::Prepare (USHORT * mi,USHORT nmi,SET_TYPE ty,USHORT ixe) {
	 zcxb.GetSpace(tcd, nmi);
	 if(tcd == 0)
		 return 0;// limite atteinte
	 for(int i = 0; i < nmi; i++)
		 tcd[i] = mi[i]; 
	 ncd = nmi;
	 ix = ixe;
	 type = ty;
	 return 1;
 }




void SETS::Init() {
	izc = 1;
	zcxb.Init();
	nmmax = 0;
	nmmin = 10;
	direct = 0;
}

void SETS::Image() {
	EE.E("\nimage fichier choix izc=");
	EE.Enl(izc);
	for(int i = 1; i < izc; i++) {
		zc[i].Image();
		EE.Enl();
	}
}


int SETS::ChargeSet (USHORT * mi,USHORT nmi,SET_TYPE ty)
{if(nmi<2||puz.stop_rating) return 0;
 if(ty &&  nmi>(chx_max+1) ) return 0;
 if(!zc[0].Prepare(mi,nmi,ty,izc)) return 0;
if(izc<sets_lim) {zc[izc++]=zc[0];  
                 if(nmi>nmmax)nmmax=nmi;  
				 if(nmi<nmmin)nmmin=nmi; return 1;}
parentpuz->Elimite("ZCX");return 0;}

//int SETS::CopySet (int i)
//{if(izc<sets_lim) {zc[izc++]=zc[i];  return 1;}
//parentpuz->Elimite("ZCX");return 0;}

 // multi chains version
int SETS::Interdit_Base80() {
	t= zcf.h.d.t;
	int ir=0;
	for (int ie=1;ie<izc;ie++)     // all active sets 
	{if(zc[ie].type-SET_base) continue;
	int n=zc[ie].ncd; USHORT *tcd=zc[ie].tcd ; 
	BFTAG tbt; tbt.SetAll_1();
	for(int  i=0;i<n;i++)  tbt &= t[tcd[i]<<1];

	if(tbt.IsNotEmpty()) // candidate(s) to clear found
	{if(Op.ot&& 1){EE.E(" eliminations found in multi chain mode pour ");
	zc[ie].Image();EE.Enl();} 

	for(int  j=3;j< puz.col;j+=2)if(tbt.On(j)) // all tags assigned
	{int tot_length=0; USHORT jj=j^1;// skip from assigned to eliminated
	if(Op.ot && 0){EE.E(" Set killing "); zpln.ImageTag(jj); EE.Enl(); }
	if(puz.ermax>85+n-3) // gofast if already far above
	{zpln.Clear(jj>>1); ir++;
	if(Op.ot){EE.E(" Set fast killing "); zpln.ImageTag(jj); EE.Enl();}
	continue;}
	for(int i2=0;i2<n;i2++)  
	{   BFTAG wch=zcf.h.dp.t[jj]; 
	USHORT end=(tcd[i2]<<1)^1;
	if(wch.On(end))// this is a direct
	{tot_length+=2; continue;}
	int npasch=wch.SearchChain(zcf.h.dp.t,jj,end);
	if(!npasch) EE.Enl(" 0 partial length "); // debugging message
	tot_length+=npasch+2;
	}
	int ratch=puz.tchain.GetRatingBase(80,tot_length,jj>>1);
	if(ratch) // chain is accepted load it (more comments in test mode)
	{// in test mode  give the details for the chains
		// in that case, do it again and print
		if(Op.ot)for(int i2=0;i2<n;i2++) 
		{  BFTAG wch=zcf.h.dp.t[jj]; 
		USHORT end=(tcd[i2]<<1)^1;
		if(wch.On(end))// this is a direct
		{USHORT tt[2],itt=2; tt[0]=jj; tt[1]=end; 
		zpln.PrintImply(tt,itt);continue;}
		int npasch=wch.SearchChain(zcf.h.dp.t,jj,end);
		USHORT tt[50],itt=npasch+2; 
		wch.TrackBack(zcf.h.dp.t,jj,end,tt,itt,end);
		zpln.PrintImply(tt,itt);
		}
		puz.tchain.LoadChain(ratch,"chain",jj>>1);	
	}
	} // end  for j
	} // end if
	}// end ie
	return ir;
}
 
/* look for new weak links from sets
   in dynamic mode with part of the tags éliminated (allsteps)
   return code is o if nothing has been done
*/

/// en analyse

int SETS::DeriveDynamicShort(BFTAG & allsteps,SQUARE_BFTAG & dpn,SQUARE_BFTAG & dn){
	if(0)
	     parentpuz->Image(allsteps,"start derive allsteps",0); ///

	dn.ExpandAll(dpn);
	allparents.AllParents(dn);

	int ret_code=0;
	t = dn.t;
	for(int ie = 1; ie < izc; ie++) {
		SET chx=zc[ie];
		if(zc[ie].type- SET_base) break; // closed after SET_base
		 // find the reduced set using allsteps
    	USHORT * tcdo = chx.tcd, nno = chx.ncd ,
			     tcd[10],nni=0,aig=0;
		for(int i = 0; i < nno; i++){
			USHORT j=tcdo[i]<<1;
			if(allsteps.On(i)){
				aig=1;  // assigned nothing to do
				break;
			}
			if(allsteps.Off(j^1))
				tcd[nni++]=j;
		}
		if(aig || nni<2) 
			continue; // assigned or equivalent
		   // now the set has more than 2 valid candidates

		BFTAG tcf2, tcf2f, bfset;  
		tcf2.SetAll_1();
		// bfset is the set itself in bf form for the exclusion of the set itself
		for(int i = 0; i < nni; i++)
			bfset.Set(tcd[i] );
		for(int i = 0; i < nni; i++) {
			tce[i] = allparents.t[(tcd[i]) ^ 1];
			tce[i] -= bfset;
		}
		for(int i = 0; i < nni; i++) {
			USHORT tg=tcd[i];
			tcf2f = tcf2;
			if(i < nni) {
				for(int k = i + 1; k < nni; k++) {
					tcf2f &= tce[k];
				}
			}
			USHORT ind[640], maxInd,    // raw table
				   ind2[640],maxInd2=0;   // same not forcing a true
			tcf2f.String(ind, maxInd);
			for(int j = 0; j < maxInd; j++) { //clear tags forcing a true
				USHORT tgo=ind[j];
				BFTAG ww(dn.t[tgo]); ww&=bfset;
				if(ww.IsEmpty())
					ind2[maxInd2++] =tgo;
				else
					tcf2f.Clear(tgo);// just for debugging
			}
			if(0 &&maxInd2){///
				EE.E("target"); zpln.ImageTag(tg);
				EE.E(" derive actif"); chx.Image();
				EE.Enl();
				parentpuz->Image(tcf2f," coming from ",0);
				EE.Enl();

			}

			for(int j = 0; j < maxInd2; j++) {
				USHORT tgo=ind2[j];
				if(dn.t[tgo].Off(tg)){
					dpn.t[tgo].Set(tg);
					ret_code=1;
				}
			}
			tcf2 &= tce[i];
			if(tcf2.IsEmpty())
				break; 
		}// end i	

	}	
	return ret_code;
}




void SETS::Derive(int min,int max,int maxs) {
	if(max > nmmax)
		max = nmmax;
	if(min < nmmin)
		min = nmmin;
	if(maxs > nmmax)
		maxs = nmmax;
	int maxt = (max > maxs) ? max : maxs;

	if(Op.ot && 0) {
		EE.E("debut Derive izc= ");
		EE.E(izc);
		EE.E("  direct= ");
		EE.E(direct);
		EE.E("  min= ");
		EE.E(min);
		EE.E("  max= ");
		EE.E(max);
		EE.E("  maxs= ");
		EE.Enl(maxs);
	}  

	if(direct) {
		t = zcf.h.dp.t;
		allparents.AllParents(zcf.h.dp);
	}
	else {
		t = zcf.h.d.t;
		allparents.AllParents(zcf.h.d);
	}// usually direct=0
	for(int ie = 1; ie < izc; ie++) {
		int nnm = zc[ie].ncd;   
		switch(zc[ie].type) {
		case SET_base:
			if(nnm <= max)
				DeriveBase(zc[ie]);
			break;
		case SET_set:
			if(nnm <= maxs)
				DeriveSet(zc[ie]);
			break;	
		}   
	}
}
void SETS::DeriveBase(const SET & chx) { // each candidate can be the target
	if(0) {
		EE.E("on traite");
		chx.Image();
		EE.Enl();
	}
	USHORT * tcd = chx.tcd, nni = chx.ncd ; 
	BFTAG tcf2, tcf2f, tcft, bfset;
	tcf2.SetAll_1();
	tcft = tcf2;
	// bfset is the set itself in bf form for the exclusion of the set itself
	for(int i = 0; i < nni; i++)
		bfset.Set(tcd[i] << 1);
	for(int i = 0; i < nni; i++) {
		//tce[i]=allparents.t[(tcd[i]<<1)^1]-bfset;
		tce[i] = allparents.t[(tcd[i] << 1) ^ 1];
		tce[i] -= bfset;
	}
	for(int i = 0; i < nni; i++) {
		tcf2f = tcf2;
		if(i < nni) {
			for(int k = i + 1; k < nni; k++) {
				tcf2f &= tce[k];
			}
		}
		USHORT ind[640], maxInd; //v 1 by MD
		tcf2f.String(ind, maxInd);
		for(int j = 0; j < maxInd; j++) {
			if(zcf.IsStart(ind[j], tcd[i] << 1))
				continue; // skip if defined		    
			zcf.LoadDerivedTag(ind[j], tcd[i]);
		}

		tcf2 &= tce[i];
		if(tcf2.IsEmpty())
			return;
	}// end i
}

/* deriving a set creating an event
   the set must be in a dead state for a candidate in left to right mode
   then, the event is established and the event process is called
*/
void SETS::DeriveSet(SET & chx) { // only the "event" can be the target
	USHORT *tcd = chx.tcd, nni = chx.ncd - 1 ; 
	BFTAG tcft, bfset;
	tcft.SetAll_1();
	// bfset is the set itself in bf form for the exclusion of the set itself
	for(int i = 0; i < nni; i++)
		bfset.Set(tcd[i] << 1);
	for(int i = 0; i < nni; i++) {
		//tce[i]=allparents.t[(tcd[i]<<1)^1]-bfset;
		tce[i] = allparents.t[(tcd[i] << 1) ^ 1];
		tce[i] -= bfset;
	}
	for(int i = 0; i < nni; i++)
		tcft &= tce[i];

	if(tcft.IsNotEmpty()) { // event established for any 'true' in tcft
		for(USHORT j = 2; j < puz.col; j++) {
			if(tcft.On(j)) {
				if(tevent.EventSeenFor(j, tcd[nni])) { 
						if(!Op.ot) {// this just for diag
					       EE.E("diag choix");
					       chx.Image();
						}
				}
			}
		}// end j
	}// end IsNotEmpty
}




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

int EVENTLOT::GenForTag(USHORT tag, WL_TYPE type) const {
	for(int i = 0; i < itcd; i++) {
		USHORT cand2 = tcd[i], tag2 = (cand2 << 1) ^ 1;
		if(zcf.Isp(tag, tag2))
			continue;// only if not yet defined
		if(0 && (tag ==(tag2 ^ 1))) {
			EE.E("gen contraire");
			Image();
			return 1;
		}

		if(tag == tag2)
			continue;// redundancy
		if(type == wl_ev_direct)
			zcf.LoadEventDirect(tag >> 1, cand2); 
		else
			zcf.LoadEventTag(tag, cand2);
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

void EVENT::Load(USHORT tage, EVENT_TYPE evt, const EVENTLOT & evb, const EVENTLOT & evx) {
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
   if first set is more than one create entry in TSET
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
		if(0 && Op.ot) {
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
		parentpuz->Elimite("TEVENT limite atteinte");
		return;
	}
	USHORT evv = event_vi + (it);
	t[it++].Load(evv, evt, evb, evx); 
	eva.tcd[eva.itcd] = evv;
	zcx.ChargeSet(eva.tcd, eva.itcd + 1, SET_set); // set loaded
}

 /* the event has been created by  "tagfrom" for the event "tagevent"
    find the corresponding element in the talbe and generate the weak links
	*/
int TEVENT::EventSeenFor(USHORT tagfrom, USHORT tagevent) const {
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
void EVENT::ImageShort() const {
	char * tlib[]={"pointing rc","pointing b","naked pair","hidden pair","Xwingr","Xwingc"};
	if(!Op.ot)
		return;
	EE.E("\t event   ");
	EE.E(tlib[type]); 
	EE.E("\t\t");
	for(int i = 0; i < ntcand; i++) {
		EE.Esp();
		zpln.Image(tcand[i]);
	} 

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
	REGION_CELL el1d = puz.alt_index.tchbit.el[el1].eld[ch], el2d = puz.alt_index.tchbit.el[el2].eld[ch];
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
	CELL_FIX p1 = t81f[cell1], p2=t81f[cell2];
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
		CELL_VAR p = T81t[cell].v;
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
				BF81 chel = divf.elz81[iel] & puz.c[ich];  // the elem pattern for the ch
				if(chel.IsEmpty())
					continue; // nothing  
				BF81 chbcom = chel & divf.elz81[ib]; // common part with the block
				if(chel == chbcom)
					continue; //  already locked
				if(chbcom.Count() < 2)
					continue; // nothing to do I guess
				chel -= chbcom; // set in row col
				BF81 chb = (divf.elz81[ib] & puz.c[ich]) - chbcom; // set in box

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


/*  process to find a nested chain in dynamic mode (level 4)
    looking for forcing or contradiction
    only basic strong and weak links
	but dynamic search (using basic sets)

*/
int PUZZLE::CaseNestedLevel4Case1( USHORT tag )  {

	if(0) {
		EE.E("\n\nnested level 4 case 1 for tag ");
		zpln.ImageTag(tag);
		EE.Enl();
	}


	chain4_to = chain4_dpn.t; // we use the table  for nested
	for(int i = 0; i < 640; i++)
		chain4_tsets[i] = 0;
	chain4_npas = 0;
	chain4_steps[0]=(*cum);  // start with the known status
	chain4_steps[0].Set(tag);  // plus the tag
	chain4_allsteps = chain4_cumsteps[0] = chain4_steps[0];  
	chain4_buf[0]=tag;        // and expand the tag in cycle one
	chain4_tx[0] = chain4_buf;
	chain4_itx[0] = 1; // initial is tag to go forward

	nested_aig2 = 1;
	
	//--------------------- loop  forward
	while(nested_aig2 && chain4_npas++ <= pasmax) {
		nested_aig2=0; 
		chain4_cum = &chain4_cumsteps[chain4_npas-1];
		chain4_step = &chain4_steps[chain4_npas];
		chain4_ta = chain4_tx[chain4_npas-1];
		chain4_ita = chain4_itx[chain4_npas-1];
		chain4_tx[chain4_npas]=&chain4_ta[chain4_ita];

		NestedChainWhile(tag);                    // while cycle

		if(!nested_aig2)
			return 0;			// nothing happenned finished should not be
   
		chain4_cumsteps[chain4_npas] = chain4_allsteps;   
		(*chain4_step) = chain4_allsteps;
		(*chain4_step)-=(*chain4_cum);  
		(*chain4_step).String(chain4_tx[chain4_npas],chain4_itx[chain4_npas] );
        if(0){  
		  EE.E(chain4_npas);EE.E("pas ncase lvl4 ");Image((*chain4_step),"step",0);
		  Image(chain4_allsteps,"chain4_allsteps",0);
		  Image(allsteps,"allsteps",0);
		  EE.Enl();
		}

	    if((*chain4_step).IsEmpty() ||chain4_npas>30 ){
        EE.Enl("check if null or 30 pas");
		return 0;
		}

		 
		// stop at the first contradiction reached
		BFTAG tw(chain4_allsteps);
		tw &=(tw.Inverse()).TrueState();
		USHORT ttw[640],ittw;
		tw.String(ttw,ittw);
		if(!ittw)
			continue; 
		 
		     // now we have a contradicton or a forcing chain
		     // we don't look for more

		if(0){ 
               EE.Enl("contradiction reached step"); EE.Enl( chain4_npas);
			   Image(tw,"for targets",0);			  
			}
		   
		 // we have to take the shortest forcing or contradiction chain
		USHORT totlength=10000;
	    for(int iw=0;iw<ittw;iw++){
			USHORT y=ttw[iw]^1; // start with false for forcing chain
			USHORT itt=NestedChainGoBack(y),
				   *tt=chain4_result;
			if(!itt) continue; // should always be ok
			chain4_bf=back4_bfsource;

			y^=1;// now y is true and can be the tag
			if(y==tag){   //this is a forcing chain its ok
			   USHORT length=chain4_iresult-1;
			   if(length<totlength){ // take it if lower
				   totlength=length;
				   USHORT im = tstore.AddOne(chain4_result,chain4_iresult);
				   tsets[tag ^ 1] =- tcandgo.itt;
				   tcandgo.tt[tcandgo.itt++].Add(im, chain4_bf,length); 
			   }
				 //   elims.Set(i ^ 1); 
				continue;
			}
			USHORT length=chain4_iresult;
			USHORT  ii=tstore.AddChain(chain4_result,chain4_iresult);

			itt=NestedChainGoBack(y); // second chain
			if(!itt) continue; // should always be ok
			length+=chain4_iresult; // add second length
			chain4_bf|=back4_bfsource;  // and new source

			if(length<totlength){ // take it if lower
				totlength=length;
				USHORT  jj=tstore.AddChain(chain4_result,chain4_iresult);
			    USHORT im = tstore.AddMul(ii, jj);
			    tsets[tag^1] =- tcandgo.itt;
				tcandgo.tt[tcandgo.itt++].Add(im, chain4_bf,length); 
			}

		}
		if(0 && totlength<10000 ){
			EE.E("\n\nnested level 4 case 1 for tag ");
			zpln.ImageTag(tag);
			EE.E("cleared length ");
			EE.Enl(totlength);

		}
		return (totlength<10000);
	}// end while
return 0;}



/*  process to find a nested chain in dynamic mode (level 4)
    only basic strong and weak links
	but dynamic search (using basic sets)

*/
int PUZZLE::CaseNestedLevel4( USHORT tag,USHORT target)  {

	if(0) {
		EE.E("\n\nnested level 4 for tag ");
		zpln.ImageTag(tag);
		EE.E("  target ");
		zpln.ImageTag(target);
		//Image(allsteps,"allsteps depart",0);
		EE.Enl();
	}


	chain4_to = chain4_dpn.t; // we use the table  for nested
	for(int i = 0; i < 640; i++)
		chain4_tsets[i] = 0;
	chain4_npas = 0;
	chain4_steps[0]=(*cum);  // start with the known status
	chain4_steps[0].Set(tag);  // plus the tag
	chain4_allsteps = chain4_cumsteps[0] = chain4_steps[0];  
	chain4_buf[0]=tag;        // and expand the tag in cycle one
	chain4_tx[0] = chain4_buf;
	chain4_itx[0] = 1; // initial is tag to go forward

	nested_aig2 = 1;
	
	//--------------------- loop  forward
	while(nested_aig2 && chain4_npas++ <= pasmax) {
		nested_aig2=0; 
		chain4_cum = &chain4_cumsteps[chain4_npas-1];
		chain4_step = &chain4_steps[chain4_npas];
		chain4_ta = chain4_tx[chain4_npas-1];
		chain4_ita = chain4_itx[chain4_npas-1];
		chain4_tx[chain4_npas]=&chain4_ta[chain4_ita];

		NestedChainWhile(tag);                    // while cycle

		if(!nested_aig2)
			return 0;			// nothing happenned finished should not be
   
		chain4_cumsteps[chain4_npas] = chain4_allsteps;   
		(*chain4_step) = chain4_allsteps;
		(*chain4_step)-=(*chain4_cum);  
		(*chain4_step).String(chain4_tx[chain4_npas],chain4_itx[chain4_npas] );
        if(opdiag){  ///
		  EE.E(chain4_npas);EE.E("pas ncase lvl4 ");Image((*chain4_step),"step",0);
		  Image(chain4_allsteps,"chain4_allsteps",0);
		  Image(allsteps,"allsteps",0);
		  EE.Enl();
		}

	    if((*chain4_step).IsEmpty() ||chain4_npas>30 ){
        EE.Enl("check if null or 30 pas");
		return 0;
		}

		 
		// check if target has been reached
		if(chain4_allsteps.On(target)){ 

			if(0){ 
               EE.Enl("target reached step"); EE.Enl( chain4_npas);
			   EE.E(" target ="); zpln.ImageTag(target);EE.Esp();
			   if(chain4_npas==1){
				   Image(chain4_steps[1],"step1",0);
				   Image(chain4_steps[0],"step0",0);
				   Image(chain4_allsteps,"chain4_allsteps",0);
 				   Image(allsteps,"allsteps",0);
				   stop_rating=1;
 			   }
			}
			// we compute back the length and  the sequence 
			// we store the chain  if ok
			USHORT itt=NestedChainGoBack(target);
			if(!itt) return 0; // should always be ok

			chain4_bf|=back4_bfsource;		
			return itt;
		}
	}// end while
return 0;}

/*
  NestedChainWhile sub function of NestedLevel4(=
  */

void PUZZLE::NestedChainWhile(USHORT tag) {
	// look first for direct 
	for(int it = 0; it < chain4_ita; it++) {
		BFTAG x = chain4_to[chain4_ta[it]];
		if(x.substract(chain4_allsteps)) {
			chain4_allsteps |= x; //  in the total 
			nested_aig2=1;
		} 
        
	}

	// check now sets and only Set_base

	for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		if(chx.type-SET_base)return;  // finished
		int n = 0, nni = chx.ncd, joff=0; 

		for(int i = 0; i < nni; i++) { // max one free 
					USHORT cd = chx.tcd[i], j = cd << 1; // candidate in tag form
					if(chain4_allsteps.On(j)) {
						n=2;
						break;;  // set assigned force next
					}
					if(chain4_allsteps.Off(j ^ 1)) {
						if(n++) // only one, exit with 2
							break; 
						joff = j;
					} 
				}
		if(!n){ // if nothing set all
			for(int i = 0; i < nni; i++) { 
				USHORT cd = chx.tcd[i], j = cd << 1; 
				if(chain4_allsteps.Off(j )) {
		            chain4_allsteps.Set(j);
		            chain4_tsets[j] = ie;
		            nested_aig2 = 1;
				}

			}
			continue;
		}
	
		if((n - 1) || (!joff))continue;	// if ok second round for action	
		chain4_allsteps.Set(joff);
		chain4_tsets[joff] = ie;
		nested_aig2 = 1;
	}// end ie

}

//--------------------------------------------------
int PUZZLE::NestedChainGoBack(USHORT tag) {
	int waig=0;
	if(opdiag) {
		waig=1;
		EE.E("goback");zpln.ImageTag(tag);
		EE.E(" npas=");EE.E(chain4_npas);
		EE.E(" source");  zpln.ImageTag(chain4_tx[0][0]);  EE.Enl();
		Image(chain4_cumsteps[0],"depart",0);
        EE.Enl();
	}

	USHORT tret[300],itret=0,itret1=0;
	back4_bfcount.SetAll_0(); 
	back4_bfsource=back4_bfcount;
	back4_bfcount.Set(tag);
	tret[itret++] = tag;
	chain4_steps[0]=chain4_cumsteps[0]; // insert the full start
	while(itret1 < itret && itret < 300) { // solve each entry back
		USHORT x = tret[itret1], aig = 1; // first locate where x has been loaded
		int index = chain4_tsets[x];
		if(waig){
		    EE.E("ncgb step index=");EE.E(index);
		    EE.E(" pour tag ");		zpln.ImageTag(x);
		    EE.E(" itret=");EE.E(itret);
		    EE.E(" itret1=");EE.E(itret1);
		    EE.Enl();
		}
		for(int i = 0; i <= chain4_npas; i++) {
			if(chain4_steps[i].On(x)) {
				aig=0; 
				if(i) {  // not initial assumption
	              if(!index){ // this is a direct step
	                 int z=0,ia=i-1;
		             for(int j=0;j<chain4_itx[ia];j++){ 
	                        USHORT y=chain4_tx[ia][j]; 
			                if(chain4_to[y].On(x))  {
					           z=y; 
							   if(back4_bfcount.Off(z)){// not yet there
                                  tret[itret++]=z;
			                      back4_bfcount.Set(z);
								  // add source for new strong links
								  if((y&1) && zcf.hdp_base.t[y].Off(x)){
									  // this must be a new strong link
									const CANDGOSTRONG * w = tcandgo.Get(y, x);
				                    if(w) back4_bfsource |= w->source; 

								  }

							   }
					           break;			   
					         } // end if
	                 }// end step prospect

  		            if(!z)   // not found should never be
						  aig=1;				  
		           } // end index=0

	   			   else  {// it comes from a set, we take it as it is						
						SET chx = zcx.zc[chain4_tsets[x]];
						if(waig){
						    EE.E("set ");chx.Image();EE.Enl("set ");
							Image(back4_bfcount,"back4_bfcount",0);
						}

						int n = chx.ncd; 
						for(int j = 0; j < n; j++) {
							USHORT y = chx.tcd[j] << 1;
							if(waig){
								  EE.E("processing" );zpln.ImageTag(y);
								  EE.Enl( );
								}
							if(y == x)
								continue;
							y ^= 1;
							if((*cum).On(y)){
								back4_bfsource.Set(y);// add to source
								continue; // already false
							}
							if(back4_bfcount.Off(y)) {
								if(waig){
								  EE.E("plus1 itret" );EE.Enl(  itret);
								}
								tret[itret++] = y;
								back4_bfcount.Set(y);
							}
						}// end candidates of the set
					}  //end of the set

				}// end if (i)
//				break; // stop after the step 
			} // end found the tag
		}  // end i
       if(aig || itret>300) {
		   if(aig)
		  // cerr <<"stop nested chain goback   aig=1  "<<endl;  
	       stop_rating=1;
	        if( Op.ot) {
				EE.Enl("nested chain go back  invalid situation source");
				EE.E("goback");zpln.ImageTag(tag);
		        EE.E(" npas=");EE.Enl(chain4_npas);
				Image(chain4_cumsteps[0]," start point",0);
				for(int i = 0; i <= chain4_npas; i++) {
					Image(chain4_steps[i],"step",0);
				}

		    }   
	       return 0; // not found, should never be
       }	
   	   itret1++;
	}// end while

	// now prepare the chain in storing mode
	chain4_iresult=0;
    for(int i = 0; i <= chain4_npas; i++) 
		for(int j = itret-1; j >=0; j--)  
			if(chain4_steps[i].On(tret[j])) 
				chain4_result[chain4_iresult++] = tret[j]; 
	if(0   ){ ///
	  EE.E("nestedgoback end length"); EE.E(chain4_iresult); 
	  EE.E(" itret="); EE.Enl(itret);
	  Image(back4_bfsource,"back4_bf source ",0);
	//  Image(back4_bfcount,"back4_bf count ",0);
	  for(int i=0;i<chain4_iresult;i++){
		  zpln.ImageTag(chain4_result[i]);
		  EE.Esp();
	  }

      EE.Enl();
	}

	return chain4_iresult ; //+ biv; waiting examples for validation
}



/* this is dynamic mode
   and can be a contradiction chain or a dual chain


  still missing here : look for the best of
             forcing chain
			 contradiction forcing chain (not processed so far)

   */
///

void PUZZLE::NestedForcingLevel4(BFTAG & elims) {
	if(0){ ///
	    EE.E("\nentry nested forcing level 4 step="); EE.Enl(npas);
	    Image(allsteps,"allsteps",0);
	//    chain4_dpn.Image();
	    EE.Enl("\n");
	}
	for(int i = 2; i < puz.col; i += 2) {
		BFTAG tw(dn.t[i]);
		tw &=tw.Inverse();
		if(tw.IsNotEmpty() || dn.t[i].On(i^1)){
			if(CaseNestedLevel4Case1(i))
			  elims.Set(i ^ 1); 
			else{
				EE.Enl("caseNestedLevel4Case1 invalid return");
				Image(tw," assumed targets",0);
			}
		}
		
        // we look also for fresh dual chains 

		tw=dn.t[i^1];
		if(tw.On(i))  // filter if i^1 false
			tw.SetAll_0();
        tw &= dn.t[i].FalseState();
		tw-=allsteps;
		tw-=elims;   
		if(tw.IsNotEmpty())  {
			if(0){ ///
				EE.E("dual chain active source  ");zpln.ImageTag(i);
				Image(tw,"for targets",0);
				EE.Enl();
			}


            USHORT uw[200],iuw;
		    tw.String(uw,iuw);
		     for(int iw=0;iw<iuw;iw++) { // process all targets	
               USHORT j=uw[iw],length=0;
			   int ii=CaseNestedLevel4(i,j);
			   if(!ii)  // should always be	
			      continue;
			   length+=ii;
			   chain4_bf=back4_bfsource;
			   ii=tstore.AddChain(chain4_result,chain4_iresult);
			   int jj=CaseNestedLevel4(i^1,j);
			   if(!jj)  // should always be	
			      continue;
			   length+=jj;
			   chain4_bf|=back4_bfsource;
			   jj=tstore.AddChain(chain4_result,chain4_iresult);
			   USHORT im = tstore.AddMul(ii, jj);
			   tsets[j] =- tcandgo.itt;
				elims.Set(j); 
				tcandgo.tt[tcandgo.itt++].Add(im, chain4_bf,length); 
		    }
		}
	}
}


/*
Now looking for multi chains eliminations if any
the bfs must contain all candidates killed by the main assumption
and the source of all new strong links used

*/
void PUZZLE::NestedMultiLevel4(BFTAG & elims) {
   if(0){
	  EE.E("entry nested multi level 4 step ="); EE.Enl(npas);
	  Image((*cum),"cum debut",0);
	  Image(allsteps,"allsteps debut",0);
	 // dn.Image();
   }

   for(int ie = 1; ie < zcx.izc; ie++) {
		const SET &chx = zcx.zc[ie];
		int nni = chx.ncd, aig2 = 0; 
		BFTAG zt;
		zt.SetAll_1();
		zt = zt.FalseState();
		zt -= (*cum);
		if(chx.type - SET_base)
			   continue;
		   

		for(int i = 0; i < nni; i++) {  
			USHORT cd = chx.tcd[i], j = cd << 1; // candidate in tag form
			if(cum->On(j)) {
				aig2 = 1;
				break;
			}// set assigned
			zt.Clear(j^1); // set not in the target
			if(cum->Off(j ^ 1))
				zt &= dn.t[j];			
			
		}
		if(aig2 || zt.IsEmpty() )
			continue;	// if ok second round for action	
		if(0){
			EE.E("N Mul Lvl4 ");
			chx.Image();
			Image(zt," active for ",0);
			EE.Enl();
		}

		USHORT wtt[400],iwtt;
		zt.String(wtt,iwtt);
		for(int ii=0;ii<iwtt;ii++){
		    int i=wtt[ii];
			if(tsets[i])  // don't do if already found
				continue;
			if(1){
				EE.E("gen multi ");chx.Image();
				EE.E(" for ");
				zpln.ImageTag(i);
				EE.Enl();
			}
			chain4_bf.SetAll_0();
			USHORT istored = 0, istoref = 0, tot_count = 0;

			for(int i2=0;i2<nni;i2++) {
				USHORT cd = chx.tcd[i2], j = cd << 1; // candidate in tag form
				if(cum->On(j ^ 1)) {
					chain4_bf.Set(j ^ 1);
					continue;
				}// already false


			   int ii=2;
		//	   opdiag=1; ///
               if(dpn.t[j].On(i)){ // make it simple if direct
				   chain4_iresult=2;
				   chain4_result[0]=j;
				   chain4_result[1]=i;
			 	   back4_bfsource.SetAll_0();
			   }
			   else
				   ii=CaseNestedLevel4(j,i);
			//   opdiag=0; ///
			   if(ii){ // should always be	
				    chain4_bf|=back4_bfsource;
				    if(0){
						EE.E("return from "); zpln.ImageTag(j);
						EE.Esp(); zpln.ImageTag(i);EE.Enl();
						zpln.PrintImply(chain4_result,chain4_iresult);
					}
				    istoref = tstore.AddChain(chain4_result,chain4_iresult);
				    if(!istored)
					   istored = istoref;
				   tot_count += chain4_iresult;			       
				}  //end if

			   else { // debugging sequence
				   EE.E("m lvl4 failed for ");
				   zpln.ImageTag(j);
				   EE.Esp();
				   zpln.ImageTag(i);
				   chx.Image();
				   EE.Enl();


			   }
			} // end i2 

			if(istored) { //should always be
				USHORT ii = tstore.AddMul(istored, istoref);
				tsets[i] =- tcandgo.itt;
				elims.Set(i); 
				tcandgo.tt[tcandgo.itt++].Add(ii, chain4_bf, tot_count);  
				if(0){					
					EE.Enl("multi created");
					tstore.Print(tcandgo.tt[-tsets[i]].index);
				}
			}			
		} // end tag i
	}// end ie
}

