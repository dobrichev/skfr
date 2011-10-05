/*
Copyright (c) 2011, OWNER: G�rard Penet
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
#include "_03a_jdk.h"
#include "_30a_CANDGO.h"
#include "ratingengine.h"
*/
JDK::JDK() {
	solution = un_jeu.ggf.pg;  
	T81 = &tp8N;
	T81C = &tp8N_cop;
	T81t = T81->t81;
	T81tc = T81C->t81;
}


void JDK::Copie_T_c() {
	tp8N_cop = tp8N;
	for(int i = 0; i < 9; i++)
		c_cop[i]=c[i];
}

void JDK::Actifs() {
	zactif.SetAll_0();
	T81->Actifs(zactif);
	for(int i = 0; i < 27; i++)
		elza81[i] = divf.elz81[i] & zactif;
}

void JDK::cInit(int un) {
	for(int i = 0; i < 9; i++)
		if(un)
			c[i].SetAll_1();
		else
			c[i].SetAll_0();
}

void JDK::cFixer(int ich, int i8) {
	for(int i = 0; i < 9; i++)
		c[i].Clear(i8); // pas de candidat ici
	c[ich].Clear(t81f[i8].z);
}   // ni en zone influence

void JDK::cReport() {    // on charge cand de ztzch
	for(int i8 = 0; i8 < 81; i8++) {
		P81 *p8 = &tp8N.t81[i8];
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

void JDK::TReport() {    // on charge c de table en mode depart impose
	for(int i = 0; i < 9; i++)
		c[i].SetAll_0();
	for(int i8 = 0; i8 < 81; i8++) {
		P81 *p8 = &tp8N.t81[i8];
		if(p8->v.typ)
			continue;
		for(int i = 0; i < 9; i++)
			if(p8->v.cand.On(i))
				c[i].Set(i8);
		p8->v.ncand = p8->v.cand.CountEtString(p8->scand);
	}
}

int JDK::Recale() {
	//cReport();
	nfix = 0;
	for(int i = 0; i < 81; i++) {
		fix[i] = '0';
		if(T81t[i].v.ncand == 0)
			return 0;
	}
	aztob.Genere();   
	for(int i = 0; i < 27; i++)
		for(int j = 0; j < 9; j++)
			if(aztob.tchbit.el[i].eld[j].n == 0)
				return 0;
	return 1;
}

int JDK::Directs() { //en tete appliquer regle de base
	int ir = 0, i;
	for(i = 0; i < 81; i++) {
		if((!T81t[i].v.typ) && (T81t[i].v.ncand == 1)) {   // case 1 candidat
			FixerAdd(i, T81t[i].scand[0], 3);
			ir = 1;
		}
	}
	for(i = 0; i < 27; i++) {
		for(int j = 0; j < 9; j++) {  // chiffre une place
			if(aztob.tchbit.el[i].eld[j].n == 1) {
				int k = aztob.tchbit.el[i].eld[j].b.First(),
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

int JDK::FaitDirects(int rating) {
	if(aigstop)
		return 1; 
	int ir = 0;
	for(int i = 0; i < 81; i++) {
		char c1 = fix[i], c2 = fixt[i];
		if(c1 - '0') {     // donc fix�e
			// filter on rating expected
			int ok = 0;
			P81F p = t81f[i];
			switch(rating) {
			case 10:
				if((divf.N_Fixes(gg.pg,p.el) == 8) 
					|| (divf.N_Fixes(gg.pg,p.pl+9) == 8)
					|| (divf.N_Fixes(gg.pg,p.eb+18) == 8))
					ok = 1;
				break;
			case 12:
				if(aztob.tchbit.el[p.eb + 18].eld[c1 - '1'].n == 1)
					ok = 10;
				break;
			case 15:
				if(aztob.tchbit.el[p.el].eld[c1 - '1'].n == 1)
					ok = 1;
				if(aztob.tchbit.el[p.pl + 9].eld[c1 -'1'].n == 1)
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

int JDK::FaitGo(int i, char c1, char c2) { // function also called if single forced
	EE.E(++Op.assigned);
	EE.E(" ");
	EE.E(t81f[i].pt);
	EE.E("=");
	EE.E(c1);
	EE.E(" ");
	if(c2 < 4)
		EE.Enl(orig1[c2]);
	else EE.Enl(" assigned");
	if((un_jeu.ggf.pg[i] - c1) && (!aigstop)) { // validite  fixation
		aigstop=1;
		EE.E( "FIXATION INVALIDE");
		return 0;
	}
	T81->Fixer(c1 - '1', i, 1);
	gg.pg[i] = c1;
	return 1;
}

//----                     supprimer ch en elem sauf  pos
int JDK::ChangeSauf(int elem, BF16 pos, BF16 chiffres) {
	int ir=0;
	for(int i = 0; i < 9; i++) {
		if(pos.On(i))
			continue;
		ir += T81t[divf.el81[elem][i]].Change(chiffres);
	}
	return ir;
}

//----                     garder  ch en elem   pos
int JDK::Keep(int elem, BF16 pos, BF16 chiffres) {
	int ir=0;
	for(int i=0;i<9;i++) {
		if(pos.On(i))
			ir += T81t[divf.el81[elem][i]].Keep(chiffres);
	}
	return ir;
}


//------                  ou simple deux points quelconques
int JDK::Keep(int ch1, USHORT p1, USHORT p2) {
	BF81 ze = t81f[p1].z & t81f[p2].z & c[ch1];
	if(ze.IsNotEmpty())
		return T81->Clear(ze,ch1);
	return 0;
}

//---------
int JDK::NonFixesEl(int el) {
	int n = 0;
	for(int i = 0; i < 9; i++)
		if(gg.pg[divf.el81[el][i]] == '0')
			n++;
	return n;
}

//--------       verify that the puzzle is correct
int JDK::Check() { 
	BF16 c[27];	// a 9 bitfield for each house
	int i;
	for(i = 0; i < 27; i++)
		c[i].f = 0;
	for(i = 0; i < 81; i++) {
		int w = gg.pg[i]; 
		P81F w8 = t81f[i]; // TO OPTIMIZE use a pointer to avoid copy of P81F
		if((w < '1') || (w > '9'))
			continue;	// empty cell
		w -= '1';
		if(c[w8.el].On(w))
			return 0;	// row
		c[w8.el].Set(w);
		if(c[w8.pl+9].On(w))
			return 0;	// column
		c[w8.pl+9].Set(w);
		if(c[w8.eb+18].On(w))
			return 0; // box
		c[w8.eb+18].Set(w);
	}
	return 1;
}

int JDK::CheckChange(int i, int ch) {
	if(aigstop) return 1;
	if(solution[i]-(ch + '1'))
		return 0;
	aigstop = 1;
	EE.E("ELIMINATION INVALIDE ");
	EE.E(ch+1);
	EE.Enl(t81f[i].pt);
	return 1;
}

void JDK::PointK() {
	couprem++;
	EE.E( "CREM=" );
	EE.E(couprem );
}

void JDK::UsePK(USHORT i) {
	EE.E(" UREM=");
	EE.E(i);
	EE.Enl();
	if(i<=couprem )
		pointK.Set(i);
}


//former _12a_jdk_ChainesNested.cpp follows

/* comments on the process
level 2 base 9.5   Forcing chain authorized
level 3 base 10.0  lev 2 + multiple chains authorized

*/

//==============================================
// 95 Quick is a partial processing for nested + forcing chains
//    and nested + multi chains
// can be used to reduce the number of puzzles when looking for hardest puzzles

void JDK::InitNested() { // common part before starting nested processing
	// lock the start situation with fully expanded  hdp from step 90
	if(Op.ot) {
		EE.Enl("Init  nested levels  ");
	}
	zcf.ResetOne(); // restore the index in zcf 
	zcf.ExpandAll();// not derived weak links
	zcf.LockNestedOne();
	zcx.LockNestedOne();
	zcxb.LockNestedOne();
	//zcf.h_one.d.Image();
}

int JDK::Rating_baseNest(USHORT base, int quick) {
	if(Op.ot) {
		EE.E("start  nested levels base =");
		EE.Enl(base);
		if(quick)
			EE.Enl("Quick mode is On");
		else
			EE.Enl("Quick mode is Off");
	}
	tchain.SetMaxLength(base);
	zcf.h_nest = zcf.h_one; // create the start for that nested level

	for(int i = 2; i < col; i++) {
		CANDGO candgo;
		candgo.GoNestedTag(i,base);
	}   

	//if(Op.ot && jdk.couprem==5)  zcf.h_nest.d.Image();//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// we have now fully expanded tags in zcf.h_nest.d
	// we look for potential eliminations

	// cases 1 and 2,{ a=>x a=> ~x } { x => ~a and ~x=> ~a}
	// case 2 includes the so called cell or region eliminations for bi values
	EE.Enl("start eliminations investigations");
	BFTAG elims1, elims2, elims3, elimst2[300], tchte[500]; 
	USHORT t2[300], it2 = 0;
	for(int i = 2; i < col; i += 2) {
		BFTAG tw = zcf.h_nest.d.t[i],
			tw1 = tw & tw.Inverse(),
			tw2 = tw & zcf.h_nest.d.t[i ^ 1];
		tw2 = tw2.FalseState();
		tw1 = tw1.TrueState();
		if(tw2.IsNotEmpty()) {  // case 2
			elimst2[it2] = tw2;
			t2[it2++] = i;
			elims2 |= tw2;
		}
		if(tw1.IsNotEmpty()) {  // case 1
			elims1.Set(i);
		}
	}

	// case 3, is there a set killing "a"
	USHORT tch[500], itch = 0;
	for(int ie = 1; ie < zcx.izc; ie++) {
		ZCHOIX chx = zcx.zc[ie];
		int n = 0, nni = chx.ncd; 
		if(chx.type - CH_base)
			continue;
		BFTAG bfw;
		bfw.SetAll_1();
		for(int i = 0; i < nni; i++)
			bfw &= zcf.h_nest.d.t[chx.tcd[i] << 1];
		if(Op.ot && 0) { //jdk.couprem ==5)
			chx.Image();
			bfw.Image("communs",0);
		}
		bfw = bfw.FalseState();	 
		if(bfw.IsNotEmpty()) {
			tchte[itch] = bfw;
			tch[itch++] = ie;
			elims3 |= bfw;
		}
	}// end case 3

	BFTAG elimt = elims1.Inverse() | elims2 | elims3;
	if(Op.ot)
		elimt.Image("elim potential", 0);
	if(elimt.IsEmpty())
		return 0;
	// if we are in quick mode, set all elims with rating base+ .3/.5

	if(quick) {
		elimt.Image("quick elim potential", 0);
		int j = 3;
		for(int i = 3; i < col; i += 2) { // first in tchain mode
			if(elimt.On(i)) {
				j = i;
				tchain.LoadChain(base + 3, "quick nested ", i >> 1);
				elimt.Clear(i);
				break;
			}
		}
		for(int i = j + 2; i < col; i += 2) { // others in direct mode
			if(elimt.On(i)) {
				zpln.Clear(i >> 1);
			}
		}
		return Rating_end(200);
	}
	// not quick mode, go step by step to find the lowest rating
	if(op1 && Op.ot) {
		EE.E("action it2 =");
		EE.E(it2);
		EE.E(" itch =");
		EE.E(itch);
		EE.Enl(); 
	}

	if(elims1.IsNotEmpty()) {  // some case 1 to apply
		for(int i = 2; i < col; i += 2) {
			if(elims1.On(i)) {
				CANDGO candgo;
				candgo.GoNestedCase1(i >> 1, base);
			}
		}
	}

	if(it2) { // some case 2 to apply
		for(int i = 0; i < it2; i++) {
			BFTAG * ptg = &elimst2[i];
			USHORT ttt[] = {t2[i], t2[i] ^ 1};
			for(int j = 3; j < col; j++) {
				if(ptg->On(j)) {
					Rating_Nested(base, ttt, 2 ,j);
				}
			}
		}
	}

	if(itch) { // some case 3 through sets to apply
		for(int i = 0; i < itch; i++) {
			if(op1 && Op.ot) {
				EE.E("action itch pour i =");
				EE.E(i);
				EE.E(" set=");
				EE.E(tch[i]);
				EE.Enl(); 
			}
			BFTAG * ptg = &tchte[i];
			ZCHOIX chx = zcx.zc[tch[i]];
			int n = 0, nni = chx.ncd; 
			USHORT ttt[20];
			for(int j = 0; j < nni; j++)
				ttt[j] = chx.tcd[j] << 1;
			for(int j = 3; j < col; j++) {
				if(ptg->On(j)) {
					Rating_Nested(base, ttt, nni, j);
				}
			}
		}
	}
	return Rating_end(200);
}

/* find the target in dynamic mode for a set of tags
*/
void JDK::Rating_Nested(USHORT base, USHORT * ttags, USHORT ntags, USHORT target) {
	// filter if target if part of the set
	USHORT ctarg = target >> 1;
	for(int i = 0; i < ntags; i++)
		if(ttags[i] >> 1 == ctarg)
			return;	
	if(op1 && Op.ot) {
		EE.E("entry rating nested ntags =");
		EE.E(ntags);
		EE.E(" target ");
		zpln.ImageTag(target);
		EE.E( "  through" );
		zpln.ImageTag(ttags[0]);
		EE.Esp();
		zpln.ImageTag(ttags[1]);
		EE.Enl(); 
	}

	USHORT length = 0;
	for (int i = 0; i < ntags; i++) {
		CANDGO candgo;
		USHORT lx = candgo.GoNestedCase2_3(base, ttags[i], target);
		if(!lx)
			return; //should never be
		length += lx;
	}

	int ratch = tchain.GetRating(length, target >> 1);
	if(ratch) { // chain is accepted load it (more comments in test mode)
		if(Op.ot) { // this can be complex and will be developped later
			EE.E("chain plus killing "); zpln.ImageTag(target);   EE.Enl();	     
		}
		tchain.LoadChain(ratch, "chain plus through set", target >> 1);
	}// end if
}

//former _12a_jdk_Chaines.cpp follows
/* main routine to process chains
   entry at a specific basic value for min rating
   process is cancelled as soon as a solution is found with a shorter rating
   all chain with the minimum rating  are stored
*/
// chain call 1=biv 2= cell_bivalue 4=nishio 8 dynamic 16=multi_chain

void JDK::Chaining(int opt, int level, int base) {
	TaggingInit();
	tchain.SetMaxLength(base);
	// long tta,ttc;// provisoire, pour test de temps
	int ir = 0, iw;   
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

	BFCAND bf0; // init to no candidate found 

	switch(base) {
		case 65:
		case 70:    // same process for X;Y;XY loops and chains
			zcf.Aic_Cycle(opt & 3);     // several tags
			break;
		case 75: //  once derived weak links from directs
			zcf.CloseOne(); // save basic weak links
			zcx.DeriveDirect();   
			zcf.ExpandShort(5); 
			zcf.ChainPlus(bf0);	
			if(tchain.IsOK(78))
				return; // the shortest should come here

			zcf.DeriveCycle(3, 6, 0, 10); // one cycle;
			if(tchain.IsOK(78))
				return;  // in case multi chain found
			// zcf.ListeForIdPhase(wl_set,zcf.iphase);
			zcf.ChainPlus(bf0);	
			if(tchain.ichain)
				break; // final?? , stop if found

			// tout ce qui suit est provisoire en cours de calage
			// pour trouver un �quilibre efficacit� performance

			TDB::InitParsing();
			EE.E("contr�le parsing mini ");
			EE.E(tchain.ParsingMini(75));
			EE.E(" rating =");
			EE.Enl(tchain.rating);

			if(!zcf.DeriveCycle(3,7,0,10))
				break; // one cycle;  
			if(tchain.ichain)
				break;
			TDB::InitParsing();
			zcf.ChainPlus(bf0);	
			if(tchain.ichain)
				break;
			iw = 1;
			while(iw++ < 3) {
				if(!zcf.DeriveCycle(3, 9, 0))
					break;
				TDB::InitParsing();
				zcf.ChainPlus(bf0);
				if(tchain.ichain)
					break;// will see later if ok
			}
			break;
	}
}

//========= end of a rating step, check if done or still to be searched

   /* next is the next rating to be searched.
      the search is over if a <= rating has been found.
      likely <= next+1 due to the minimum length adjustment 
      except for aligned triplet */


int JDK::Rating_end(int next) {
	if(!tchain.IsOK(next))
		return 0;
	Op.Step((SolvingTechnique)tchain.rating);
	if(Op.ir)
		return 0; 
	return tchain.Clean();
}

//former _12a_jdk_Chaines2.cpp follows
void JDK::TaggingInit() {
	zgs.ReInit();   // 81 bits patterns in use 
	zcf.Init();     // elementary weak links
	zcx.Init();     // sets (choices) in use
	// tevent.Init();   // event
	BFTAG::SetMax();
}

// chain call 1=biv 2= cell_bivalue 4=nishio 8dynamic 16=multi_chain
//==============================================
// this includes the search for x or y cycle
int JDK::Rating_base_65() {
	Chaining(1,0,65);// x cycle  
	if(Rating_end(65))
		return 1;
	Chaining(2, 0, 65);// y cycle  
	return Rating_end(70);
} 

//=============================================
// this includes the search for xy cycle or chain
int JDK::Rating_base_70() {
	Chaining(3, 0, 70); // xy  forcing chain or loop
	return Rating_end(75);
} 

//==============================================
// this does not includes the search for aligned triplet 
// only th search for nishio
int JDK::Rating_base_75() {
	Chaining(1 + 4, 0, 75); 
	return Rating_end(200);
} 

//==============================================
// 80 is multi chains
// we enter now a new group of eliminations. all the work is done using
// all bi values, basic weak links, basic sets
int JDK::Rating_base_80() {
	if(Op.ot)
		EE.Enl("start rating base 8.0 multi chains");
	TaggingInit();
	zpln.CellLinks();
	zpln.RegionLinks(1);
	zpln.GenCellsSets();
	zpln.GenRegionSets();
	tchain.SetMaxLength(80);
	zcf.ExpandAll();
	if(zcx.Interdit_Base80())
		return 1;; 
	return Rating_end(200);
} 

/* killed for the new one
//==============================================
// 85 is DynamicForcingChain
	// at least the derived weak links from direct weak links
	// we do that once for ever till the end
int JDK::Rating_base_85()
{ if(Op.ot) EE.Enl("start rating base 8.5 dynamic forcing chain");
  if(op0&& Op.ot){   long tw=GetTimeMillis();
             int dt=tw-tdebut;
		     EE.E("time =");EE.Enl(dt);}
 tchain.SetMaxLength(85);
 BFCAND bf0; // init to no candidate found 
 zcf.CloseOne(); // store the index and basic weak links
 zcx.DeriveDirect(); 
 zcf.ChainPlus(bf0);	
 if(tchain.IsOK(87) ) return Rating_end(87); 
 zcf.DeriveCycle(3,3,0,4); // one cycle;
 zcf.ChainPlus(bf0);	
 if(tchain.IsOK(87) ) return Rating_end(87); // the shortest should come here
 // zcf.ListeForIdPhase(wl_set,zcf.iphase);
// tout ce qui suit est provisoire et � pr�ciser

  if(!zcf.DeriveCycle(3,7,0,10)) return Rating_end(100);
  if(tchain.IsOK(90) ) return Rating_end(90); // the shortest should come here
  zcf.ChainPlus(bf0);	
  //if(tchain.ichain) return Rating_end(100);

   int iw=1;
   while(iw++<8)
	   {if(!zcf.DeriveCycle(3,9,0)) return Rating_end(100);
	    zcf.ChainPlus(bf0);
        if(tchain.IsOK(93) ) return Rating_end(93); // the shortest should come here
 	   }

 return Rating_end(200);}
 */ 

/* 85 new process
   expand completely the tags, but try to do it to catch the shortest
   first steps are not standard
   finish with standard expansion
   then look for elimintaions
   */
//==============================================
// 85 is DynamicForcingChain


int JDK::Rating_base_85() {
	if(Op.ot)
		EE.Enl("start rating base 8.5 dynamic forcing chain");
	tchain.SetMaxLength(85);
	BFCAND bf0; // init to no candidate found 
	zcf.CloseOne(); // store the index and basic weak links
	zcx.DeriveDirect();  // start with only equivalence to pointing claiming
	zcf.DeriveCycle(3, 3, 0, 4); // one cycle short sets
	zcf.DeriveCycle(3, 7, 0, 10); // one more limited to set of 7 candidates
	while(zcf.DeriveCycle(3, 9, 0))
		;// and full expansion
	zcf.ChainPlus(bf0);
	return Rating_end(200);
}

/* old process cancelled
//==============================================
// 90 is DynamicForcingChainPlus
// all "events"  claiming, pointing, pair, hidden pair, XWing
// follows an empty step 85
// consider each false candidate as start
// search for new bi values. If none, skip it
// look for new false thru basic sets
int JDK::Rating_base_90()
{if(Op.ot) EE.Enl("start rating base 9.0 dynamic forcing chains plus");
 tchain.SetMaxLength(90);
  zcf.ResetOne(); // restore the index in zcf  
  tevent.LoadAll();
  zcf.CloseOne(); // store the index and basic weak links

  zcf.h.d.ExpandShort(zcf.h.dp,2);
  BFCAND bf0; // init to no candidate found 
  zcf.DeriveCycle(3,4,7,2); // one cycle;
  zcf.ChainPlus(bf0);	
  if(tchain.IsOK(95) ) return Rating_end(95); // the shortest should come here
  zcf.DeriveCycle(3,7,7,10); // one cycle;  
  zcf.ChainPlus(bf0);	
  if(tchain.IsOK(97) ) return Rating_end(97);
  int iw=0;
  while(iw++<20  && zcf.DeriveCycle(3,9,7)  )
	   {zcf.ChainPlus(bf0);
		if(tchain.IsOK(97+iw) )return Rating_end(97+iw);
	   }
 return Rating_end(200);} 
*/

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
int JDK::Rating_base_90() {
	if(Op.ot)
		EE.Enl("start rating base 9.0 dynamic forcing chains plus");
	tchain.SetMaxLength(90);
	zcf.ResetOne(); // restore the index in zcf  
	tevent.LoadAll();
	zcf.CloseOne(); // store the index and basic weak links

	zcf.h.d.ExpandShort(zcf.h.dp, 2);
	BFCAND bf0; // init to no candidate found 
	zcf.DeriveCycle(3, 4, 7, 2); // one cycle;
	zcf.DeriveCycle(3, 7, 7, 10); // one cycle;  
	while(zcf.DeriveCycle(3, 9, 7))
		;
	zcf.ChainPlus(bf0);
	return Rating_end(200);
}

//former _12a_jdk_AlignedTriplet.cpp follows

//! Search for aligned pair or aligned triplet
/**
	That proces is very close to SE's one.
	Find all cells with 2 or 3 candidates and 
	collect all cells seen by one of them (potential base cells)
	First choose 2 cells (in the same band)of the potential base cells.
*/

int JDK::AlignedTripletN() {
	int debuga = 0;
	static int combi32[3][2] = {{0,1},{0,2},{1,2}};
	BF81 z23,	// set of cells that have 2 or 3 candidates (could be excluding cells)
		zbase;	// set of cells that are visible by one potential excluding cells
	for(int i = 0 ;i < 81; i++) {
		UNP p=T81t[i].v;
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
							for(int iec = 0; iec < 81; iec++) {
								if(!z23f.On(iec))
									continue;
								BF16 cec = T81t[iec].v.cand;
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
int JDK::AlignedPairN() {
	BF81 z2, zbase;
	// look for cells that have 2 candidates
	for(int i81 = 0; i81 < 81; i81++) {
		UNP p = T81t[i81].v;	// candidates of the cell
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







//former _03c_jdk_traite_base.cpp follows

int JDK::Traite() {
	//================== assign clues to start
	cInit(1);
	PKInit();
	tdebut = GetTimeMillis();
	//long told = tdebut;
	T81->init();  //initial candidats au lieu de Traite_Init(); 
	for(int i = 0; i < 81; i++) { // first assignment from clues
		int w = gg.pg[i];
		if(w > '0' && w <= '9')
			T81->Fixer(w - '1', i, 2);
	}
	cReport(); // and preparing T81 from PM per digit

	aigstop = 0;
	Op.Init();
	EE.Enl(); // new line in case test is done

	//===========================================================
	while (Op.cycle++ < 150) {
		if(Op.cycle > 148) {
			Op.Seterr(7);
			break;
		} // 
		if(aigstop) {
			Op.Seterr(5);
			break;
		} 
		long tcycle = GetTimeMillis();
		//	if(tcycle-told>3000) {Op.Seterr(6); break; }  
		if(op0) {
			if(tcycle - tdebut > 20000) { //TODO: don't stop at breakpoint in your debbuger nor change the system time!?!?
				Op.Seterr(6);
				break;
			}
			int dt = tcycle - tdebut;
			EE.E("time =");
			EE.Enl(dt);
		}
		//told = tcycle;
		int ir_ed = Op.is_ed_ep();
		if(ir_ed > 2)
			return ir_ed;//split filter
		if(ir_ed)
			return 0; // pure ed ep filter
		if(!Recale()) {
			Op.Seterr(6);
			break;
		}
		if(!gg.NBlancs())
			break; // finished
		// processing 1.0 to <6.2
		int ir_a = Traite_a();    
		//cout<<"cycle="<<Op.cycle<<"retour a="<<ir_a<<" Op.ir="<<Op.ir<<endl;

		if(!ir_a)
			return Op.ir;
		else if(ir_a < 2)
			continue;

		//  start of processes using the tagging infrastructure.
		// the table of candidates is loaded once per cycle

		zpln.Init();  // table of candidates  
		zgs.ReInit(); // 81 bits patterns in use      
		if(aigstop)
			break; // in case we would exceed the TCAND limit
		//=========================
		tchain.Init();

		Op.Step(AlignedPairExclusion);
		if(Op.ir > 1)
			return Op.ir;
		else if(Op.ir)
			continue;
		if(jdk.AlignedPairN()) {
			Op.SetEr();
			continue;
		}  //6.2

		if(Op.ermax < 74) { // skip that if high ER already reached
			Op.Step(AIC_X_cycle);
			if(Op.ir > 1)
				return Op.ir;
			else if(Op.ir)
				continue;
			if(jdk.Rating_base_65()) {
				Op.SetEr();
				continue;
			}  //6.5 and 6.6 
			Op.Step(AIC_XY);
			if(Op.ir > 1)
				return Op.ir;
			else if(Op.ir)
				continue;
			if(jdk.Rating_base_70()) {
				Op.SetEr();
				continue;
			}  //70
		}
		else if(Op.ermax < 77) { // just skip 6.5 6.6 as a first step
			if(Op.ot)
				EE.Enl("go direct to XY"); 
			Op.Step(AIC_XY);
			if(Op.ir > 1)
				return Op.ir;
			else if(Op.ir)
				continue;
			if(jdk.Rating_base_70()) {
				Op.SetEr();
				continue;
			}  //70
		}		
		else {
			if(Op.ot)
				EE.Enl("gofast");
			if(zcf.Fast_Aic_Chain())
				continue;
		} 

		Op.Step(AlignedTripletExclusion);
		if(Op.ir>1)
			return Op.ir;
		else if(Op.ir)
			continue;
		if(jdk.AlignedTripletN()) {
			Op.SetEr();
			continue;
		}  //7.5
		if(jdk.Rating_base_75()) {
			Op.SetEr();
			continue;
		}  //7.5
		if(Op.oexclude-1) {
			Op.Step(MultipleForcingChain);
			if(Op.ir > 1)
				return Op.ir;
			else if(Op.ir)
				continue;
			if(jdk.Rating_base_80()) {
				Op.SetEr();
				continue;
			}  //8.0

			if(Op.oexclude-2) {	 	
				Op.Step(DynamicForcingChain);
				if(Op.ir>1)
					return Op.ir;
				else if(Op.ir)
					continue;
				if(jdk.Rating_base_85()) {
					Op.SetEr();
					continue;
				}  //8.5

				if(Op.oexclude-3) {	
					Op.Step(DynamicForcingChainPlus);
					if(Op.ir > 1)
						return Op.ir;
					else if(Op.ir)
						continue;
					if(jdk.Rating_base_90()) {
						Op.SetEr();
						continue;
					}  //9.0

					if(Op.oexclude-4) {
						InitNested(); 
						Op.Step(NestedForcingChain);
						if(Op.ir>1)
							return Op.ir;
						else if(Op.ir)
							continue;
						if(jdk.Rating_baseNest(95, Op.oq)) {
							Op.SetEr();
							continue;
						}  //9.5

						if(Op.oexclude-5) {	
							Op.Step(NestedLevel3);
							if(Op.ir>1)
								return Op.ir;
							else if(Op.ir)
								continue;
							if(jdk.Rating_baseNest(100, Op.oq)) {
								Op.SetEr();
								continue;
							}  //100
							if(Op.oexclude-6) {
								Op.Step(NestedLevel4);
								if(Op.ir > 1)
									return Op.ir;
								else if(Op.ir)
									continue;
								if(jdk.Rating_baseNest(105, Op.oq)) {
									Op.SetEr();
									continue;
								}  //105
							}// end level4
						}// end level 3 and more
					} // end level 2 and more
				} // end if >2
			} // end if >1
		} // end if oexclude
		if(Rating_end(200)) {
			Op.SetEr();
			continue;
		} // clean the file
		if(Op.ot)
			T81->Candidats();
		aigstop=2;
		break;
	}     
	//=========================================================================	 
	EE.E("fin traitement aigstop=");
	EE.Enl(aigstop );
	gg.Image("fin");
	return aigstop;
}

int JDK::Traite_a() {
	if (Directs ()) {
		Op.Step(LastCell);
		if(Op.ir > 1)
			return 0;
		else if(Op.ir)
			return 1;
		if(FaitDirects(LastCell)) {
			Op.SetEr();
			return 1;
		}   //1.0

		Op.Step(SingleBox);
		if(Op.ir > 1)
			return 0;
		else if(Op.ir)
			return 1;
		if(FaitDirects(SingleBox)) {
			Op.SetEr();
			return 1;
		}  //1.2

		Op.Step(Single_R_C);
		if(Op.ir > 1)
			return 0;
		else if(Op.ir)
			return 1;
		if(FaitDirects(Single_R_C)) {
			Op.SetEr();
			return 1;
		}  //1.5
	}
	Actifs(); // update of active cells must be done from here

	Op.Step(Single_after_Locked);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(TraiteLocked(Single_after_Locked)) {
		Op.SetEr();
		return 1;
	}  //1.7

	Op.Step(HiddenPair_single);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(2,1,1)) {
		Op.SetEr();
		return 1;
	}  //2.0

	if(Directs()) {
	  Op.Step(NakedSingle);
	  if(Op.ir > 1)
		  return 0;
	  else if(Op.ir)
		  return 1;
	   if(FaitDirects(NakedSingle)) {
		   Op.SetEr();
		   return 1;
	   }
	}  //2.3

    Op.Step(HiddenTriplet_single);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(3,1,1)) {
		Op.SetEr();
		return 1;
	}  //2.5

	Op.Step(Locked_box);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(TraiteLocked(Locked_box)) {
		Op.SetEr();
		return 1;
	}  //2.6

	Op.Step(Locked_RC);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(TraiteLocked(Locked_RC)) {
		Op.SetEr();
		return 1;
	}  //2.8

	Op.Step(NakedPair);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(2,0,0)) {
		Op.SetEr();
		return 1;
	}  //3.0

	Op.Step(XWing);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.XW(2)) {
		Op.SetEr();
		return 1;
	}  //3.2

	Op.Step(HiddenPair);
	if(Op.ir > 1)return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(2,1,0)) {
		Op.SetEr();
		return 1;
	}  //3.4

	Op.Step(Naked_triplet);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(3,0,0)) {
		Op.SetEr();
		return 1;
	}  //3.6

	Op.Step(swordfish);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.XW(3)) {
		Op.SetEr();
		return 1;
	}  //3.8

	Op.Step(HiddenTriplet);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(3,1,0)) {
		Op.SetEr();
		return 1;
	}  //4.0

    jdk.Copie_T_c(); // to be done now copie for UR same rating
    zpaires.CreerTable();  

	Op.Step(XYWing);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(zpaires.XYWing()) {
		Op.SetEr();
		return 1;
	}  //4.2

	Op.Step(XYZWing);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(zpaires.XYZWing()) {
		Op.SetEr();
		return 1;
	}  //4.4

    if(Op.ot)
		T81->Candidats();

	Op.Step(UniqueRect1);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
	if(T81->RIN()) {
		Op.SetEr();
		return 1;
	}  //4.5

	Op.Step(UniqueRect2);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(urt.Traite(46)) {
		Op.SetEr();
		return 1;
	}  //4.6 t
    if(zpaires.UL()) {
		Op.SetEr();
		return 1;
	}    

	Op.Step(UniqueRect3);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
	 if(urt.Traite(47)) {
		 Op.SetEr();
		 return 1;
	 } //4.7
	 if(tult.Traite(47)) {
		 Op.SetEr();
		 return 1;
	 } 

	Op.Step(UniqueLoop1);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
	if(urt.Traite(48)) {
		Op.SetEr();
		return 1;
	} //4.7
	if(tult.Traite(48)) {
		Op.SetEr();
		return 1;
	}  //4.8
	                  	
	Op.Step(UniqueLoop2);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
	if(tult.Traite(49)) {
		Op.SetEr();
		return 1;
	}  //4.9

	Op.Step(NakedQuad);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
 	if(tult.Traite(50)) {
		Op.SetEr();
		return 1;
	}  //4.9
    if(yt.Tiroirs(4,0,0)) {
		Op.SetEr();
		return 1;
	}  //5.0

	Op.Step(UniqueLoop3);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
	if(tult.Traite(51)) {
		Op.SetEr();
		return 1;
	}  //5.1
 
	Op.Step(Jellyfish);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
	if(tult.Traite(52)) {
		Op.SetEr();
		return 1;
	}  //5.2
    if(yt.XW(4)) {
		Op.SetEr();
		return 1;
	}  //5.2

	Op.Step(HiddenQuad);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(yt.Tiroirs(4,1,0)) {
		Op.SetEr();
		return 1;
	}  //5.4

	Op.Step(BUG);
	if(Op.ir > 1)
		return 0;
	else if(Op.ir)
		return 1;
    if(zpaires.BUG()) {
		Op.SetEr();
		return 1;
	}  //5.6 a 5.8

	if(zpaires.aigun) {
		Op.Step((SolvingTechnique)59);
		if(Op.ir > 1)
			return 0;
		else if(Op.ir)
			return 1;
		if(zpaires.Bug3a(59)) {
			Op.SetEr();
			return 1;
		}  //5.9
		Op.Step((SolvingTechnique)60);
		if(Op.ir > 1)
			return 0;
		else if(Op.ir)
			return 1;
		if(zpaires.Bug3a(60)) {
			Op.SetEr();
			return 1;
		}  //6.0
		Op.Step((SolvingTechnique)61);
		if(Op.ir > 1)
			return 0;
		else if(Op.ir)
			return 1;
		if(zpaires.Bug3a(61)) {
			Op.SetEr();
			return 1;
		}  //6.1
	}
	return 2;
}

//former _04a_lock.cpp follows
// part of JDK class methos processing locked candidates in a box, row,col

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
int JDK::TraiteLocked(int rating) {
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
							ww = (divf.elz81[i] & jdk.c[ich]) - wex;
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
						return jdk.FaitGoA(i8, ich + '1', 4);
					} // immediate return after assign  
				}
			}
		}
	}
	return ir;
}

//<<<<<<<<<<<<<<<<<<<<
int JDK::TraiteLocked2(int eld, int elf) {
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



//<<<<<<<<<<<<<<<<<<<<<   On cherche tiroir dans element  // boucle recurrente sur i
int OBBIEL::tiroir(BF16 fd,int iold,int irang) { // on progresse sur indice et on regarde si fusion n-1  ok
	int i,id=(yt.non.f && (irang-1))? 0:iold+1;  // debut 0 avec pseudos et rang 1
	for (i=id;i<(11-yt.rangc+irang);i++)  // il doit rester des cases
	{ 
		if(eld[i].n<2 ||eld[i].n>yt.rangv) continue;
		if( yt.non.On(i))continue; //pour pseudo
		BF16 wf=eld[i].b|fd; 
		if (wf.QC() >yt.rangv) continue;
		if(irang==(yt.rangc-2))
		{yt.wf=wf;yt.wi.Set(i);
		if(wf.QC() ==yt.rangv)return 1; else return 0; }
		// une erreur a tester    on poursuit si <
		// il peut manquer des tiroirs!!!!! curieux que jamais d�tect�!!!!
		if(tiroir(wf,i,irang+1)){yt.wi.Set(i); return 1; }
	}
	return 0;
}

int TIR::Tiroirs(int nn,int hid,int sing) {     //recherche normale des tiroirs
	rangv=nn;single=sing;hid_dir=hid; int ir=0;
	int ied=0,ief=54; 
	if(!hid_dir )ief=27;if(hid_dir==1 )ied=27;if(single){ied=27;ief=54;}
	for( e=ied;e<ief;e++)   // direct, hidden or both upon request
	{rangc=rangv; non.f=cases.f=0; if(e<27) e27=e; else e27=e-27;
	if(jdk.NonFixesEl(e%27) < (rangv +1)) continue;
	for(int i=0;i<9-rangv+1;i++)
	{int nn= aztob.tpobit.el[e].eld[i].n;
	if(nn<2 || nn>rangv) continue;
	BF16 w;w=aztob.tpobit.el[e].eld[i].b;
	wi.f=0; wi.Set(i);
	if(!aztob.tpobit.el[e].tiroir(w,i,0)) continue;
	if (UnTiroir())ir= 1;	
	}  
	}
	if(ir)return 1; 
	return 0;
}

int TIR::UnTiroir() {// is there a single required after the locked set to accept it
	int ir=0;
	if(single) { // will be covered slowly can be any element row, col, box
		for(int i=0;i<9;i++)  if(wf.Off(i))
		{USHORT i8=divf.el81[e27][i]; 
		P81 p=T81t[i8];  if(p.v.typ ) continue;// must be non assigned 
		BF16 wc=p.v.cand-wi;
		for(int j=0;j<9;j++) if (wc.On(j) )// a possible hidden digit
		{BF16 wcd=aztob.tchbit.el[e27].eld[j].b-wf; // positions still valid
		if(wcd.QC()==1)
		{EE.Enl("ecs assignment");
		jdk.FaitGoA(i8,j+'1',4);// stop at first assignment
		ir=1; break;}
		}// end for j if

		}// end hidden ls
		if(!ir) return 0;// no single found
	}// end if single

	else if(e<27) {if (!jdk.ChangeSauf(e,wi,wf)&&(!ir) )return 0;  }
	else   { if (!jdk.Keep(e27,wf,wi) &&(!ir))return 0;  }

	if(!Op.ot) return 1; 
	// describe the LS even if no more eliminations after an assignment

	char *gt[]={"LS2 ","LS3 ","LS4 ","LS5 " };

	int in=rangv-2,  it2=(e%27)/9,ii2=e%9;
	char c2= (it2-1)?(char)(ii2+'1'):lc[ii2];
	EE.E("->");EE.E(gt[in]); 
	if(e<27)
	{EE.E(" cells ");	EE.E( wi.String());EE.E(" digits ");  }
	else
	{EE.E(" digits ");EE.E( wi.String());EE.E(" cells "); }
	EE.E( wf.String());EE.E(" ");EE.E(orig[it2]);EE.E(" ");EE.Enl(c2);
	return 1;
}

 


//<<<<<<<<<<<<<<<<<<<<<   On cherche XW dans lignes ou cols  boucle recurrente sur i
int TIR::XW(BF16 fd,int iold,int irang)  	// en �l�ment i chiffre ch
{   // on progresse sur indice et on regarde si fusion n-1  ok
  for (int i=iold+1;i<9;i++)  // il doit rester des �l�ments
  { int nn=el[i].eld[ch].n;
	 if(nn<2 ||nn>rangv) continue;
    BF16 wfu=el[i].eld[ch].b|fd;      if (wfu.QC() >rangv) continue;
    if(irang==(rangv-2)){ if(wfu.QC() - rangv)continue;
                         wf=wfu; wi.Set(i);return 1; }
      else if(XW(wfu,i,irang+1)) {wi.Set(i); return 1; }
  }
return 0;}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< en mode XWING liste objets et chiffres
int TIR::GroupeObjetsChange(int dobj,int ch)
{BF16 x(ch); int ir=0;
 for(int i=0;i<9;i++)   if(wf.On(i)) ir+= jdk.ChangeSauf(dobj+i,wi,x);
return ir;}
//<<<<<<<<<<<<<<<<<<<<<<<< ici chiffre en majeur �l�ments en mineur
int TIR::XW(int nn)
{char *gxw[]={"XWing ","SwordFish  ","Jelly (XW4) ","Squid (XW5)  " ,
              "Whale (XW6)","Leviathan (XW7)" };
 BF16 w; rangc=nn;
 for(int i=0;i<9;i++)
  {ch=(UCHAR)i;
   for(int iel=0;iel<10-rangv;iel++)
   {el=aztob.tchbit.el; int nn=el[iel].eld[i].n;
    if(nn>1 &&nn<=rangv)
    { w=el[iel].eld[i].b; wi.f=0; wi.Set(iel);
      if( XW(w,iel,0) )
       { if(GroupeObjetsChange(9,i) )  // action colonnes
	     {EE.E(gxw[rangv-2]);	 EE.E(" digit ");     EE.E(i+1); 
	      EE.E( " columns ");   EE.E(wf.String(0));
		  EE.E(" rows ");  EE.Enl(wi.String());     
		  return 1; 	 }}
       }
     el=&aztob.tchbit.el[9];nn=el[iel].eld[i].n;
     if(nn<2 || nn>rangv) continue;
     w=el[iel].eld[i].b;  	wi.f=0; wi.Set(iel);
     if( XW(w,iel,0) )
      { if(GroupeObjetsChange(0,i) ) // action lignes
	   {EE.E(gxw[rangv-2]);	 EE.E(" digit ");
	    EE.E(i+1); EE.E(" rows ");	EE.E(wf.String());
		EE.E( " columns ");	  EE.Enl(wi.String(0));     
		return 1; }  	 }
    } // end iel
  }    // end i niv
return 0;}





void TPAIRES::CreerTable() {
	ip = 0;
	ntplus = aigpmax = 0;
	zplus.SetAll_0();
	for(int i = 0; i < 81; i++) {
		int n = T81t[i].v.ncand;
		if(n == 2) {
			zp[ip].Charge(T81t[i]);
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
				P81F p1 = t81f[i1], p2 = t81f[i2];
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
	P81F p = t81f[i8];
	last = i8; 
	UNP pv = T81tc[i8].v;
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
		unsigned int curRow = I81::Div9(tcount[i]);
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
	if(cells.On(i8)) { // termin� on �limine URs et fausses boucles
		//EE.E("UL end ");EE.E(t81f[i8].pt);EE.Esp();cells.ImagePoints();EE.Enl();
		if(line_count < 5 || (i8 - tcount[0]))
			return 0;  
		tcount[line_count++] = i8;
		return Loop_OK();
	}
	Set(i8);              // On met le point en place
	P81F f = t81f[i8];

	// a d�faut une case avec additifs  ligne, puis col, puis bloc en paire
	// uniquement dans �l�ments non trait�s et si pas de double paire
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
	BF16 wc = aztob.tchbit.el[ele].eld[c1].b & aztob.tchbit.el[ele].eld[c2].b;
	for(int i = 0; i < 9; i++) {
		if(wc.On(i)) { // cells with both digits
			int i8r = divf.el81[ele][i];
			//EE.E("essai i8=");EE.Enl(t81f[i8r].pt);
			if(ele > 17) { // in a box, only not row col
				P81F f = t81f[i8r], f2 = t81f[last];
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
	P81F f = t81f[i8]; 
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
			tult.Store(this);
			return 0;
		}
		else
			action++;
	// les deux ci-dessous sortent en 4.6 et 4.7; voir l'origine de l'�cart (nb de pas???)
	if(action == 1 && nadds < 2) { //one cell with adds rating 4.6 rev�rifi�, c'est bien 4.6
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
		tult.Store(this);
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
	EE.E(Op.difficulty);
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
		return 0;  // maxi a v�rifier 6 cases et 4 candidats
	// set the parity of digits for bivalue cells in all elements
	for(int i = 0; i < 27; i++)
		el_par_ch[i].f = 0;
	for(int i = 0; i < ip; i++) {
		P81 p = T81t[zp[i].i8];  
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
			candp_or.f = candnp_or.f = candp_xor.f = candnp_xor.f = nwp = 0;
			for(int j = 0; j < 9; j++) {
				int i8 = divf.el81[i][j];
				P81 p = T81t[i8];
				if(zplus.On(i8)) { // wplus has the same order as tplus
					candnp_or |= p.v.cand;
					candnp_xor ^= p.v.cand;
					wplus[nwp++] = p.v.cand;
				}
				else if(p.v.ncand == 2) {
					candp_or |= p.v.cand;
					candp_xor ^= p.v.cand;
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
	P81 p = T81t[i8];
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
		P81F p1 = t81f[tplus[i]];
		b18.f ^= 1 << p1.el;
		b18.f ^= 1 << (p1.pl + 9);
	}
	BF81 zw;
	zw.SetAll_1();
	for(int i = 0; i < ntplus; i++) { // analyse all cells
		P81 p1 = T81t[tplus[i]];
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
		P81F p1 = t81f[tplus[i]]; 
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
		Op.SetDif(57);
	}
	return ir;
}
//===========================================
void TPAIRES::BugMess(char * lib) {
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
	P81 *pp;
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
					Op.SetDif(58);
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
					Op.SetDif(59);
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
						Op.SetDif(60);
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
							Op.SetDif(61);
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
	P81 p1 = T81t[tplus[0]], p2 = T81t[tplus[1]];
	int el1 = p1.f->el, el2 = p2.f->el; // use the row in priority
	if(el < 9) {
		el1 = p1.f->pl + 9;
		el2 = p2.f->pl + 9;
	}  // and col if row is "el"
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

	if((wc1.QC() - 2) || (wc2.QC() - 2))
		return 0;	 

	T81t[tplus[0]].Keep(wce1 | clock);
	T81t[tplus[1]].Keep(wce2 | clock);
	BugMess("3/4 a digit locked");
	Op.SetDif(57);
	return 1;
}

//============================================
int TPAIRES::Bug3_4_Nacked(int el) {
	EE.Enl("recherche  bug3_4 Nacked"); 

	USHORT ctl = ntplus, aig = 1;  
	P81 *pp;
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
					z2 = z1 & jdk.c[ich];  // z2 est � supprimer
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
					z2 = z1 & jdk.c[ich];  // z2 est � supprimer
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





P81 * CRIN::ta,*CRIN::tr; // ta action, tr recherche 
OBBIEL * CRIN::tchel;

CRIN::CRIN() {	 // constructor
	ta = T81t;
	tr = T81tc;
	tchel = aztob.tchbit.el;
}

int CRIN::GetElPlus() {
	return tp81f.GetLigCol(pp1,pp2);
} // assuming nplus=2

int CRIN::IsDiag() {
	if(divf.IsObjet(deux[0],deux[1]))
		diag=0;
	else
		diag = 1;
	return diag;
}

int CRIN::Jumeau(USHORT a,USHORT b,USHORT ch) {
	USHORT el = tp81f.GetLigCol(a,b);
	return Jum(el, ch);
}

void CRIN::ImageRI(char * lib,USHORT a) {
	EE.E(lib);
	EE.E(tr[a].f->pt);
	EE.E(" ");
	EE.E(tr[a].scand);
}

void CRIN::ImageRI(char * lib) {
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
int CRIN::T2(USHORT action) {
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
int CRIN::T2_el(USHORT el, USHORT action) {
	//look mode 1 for a bivalue of one of the common digits
	if(op0) {
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
		UNP v = T81t[ppi].v;
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
int CRIN::T2_el_set_hidden(USHORT len)
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
int CRIN::T2_el_set_nacked(USHORT len)
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
if(op0) { EE.E("look for nacked len ="); EE.E( len);
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

int CRIN::RID(int i1,int i2,int c1,int c2) {
	ia = I81::Pos(i1, c1);
	ib = I81::Pos(i1, c2);
	ic = I81::Pos(i2, c1);
	id = I81::Pos(i2, c2);
	char * gr = jdk.gg.pg;
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
		if(jdk.Keep(ch1, pp1, pp2)) {
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

int CRIN::RID3() {
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

	zw &= jdk.c[ch1];

	if(zw.IsNotEmpty()) {
		ImageRI(" UR one digit active");	
		return T81->Clear(zw,ch1);
	}
	return 0;
}

