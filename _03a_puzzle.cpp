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

#include "_03a_jdk.h"
#include "_30a_CANDGO.h"
#include "ratingengine.h"

JDK::JDK() {
	solution = un_jeu.ggf.pg;  
	T81 = &tp8N;
	T81C = &tp8N_cop;
	T81t = T81->t81;
	T81tc = T81C->t81;
}

//void JDK::Initial()
//{// tzg.PG54Init();         en attente      <========================<<<<<<<<<
//}

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
		if(c1 - '0') {     // donc fixée
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

//------                  on a un ou sature objet
int JDK::Keep(int ch1, BF81 const &zk) {
	int ir=0; // on cherche les objets contenant
	for(int il = 0; il < 27; il++) {
		if(divf.IsObjetI(zk, il)) {
			BF81 ze = divf.elz81[il] ^ zk;
			ze = ze & c[ch1];
			if(ze.IsNotEmpty()) {
				ir += T81->Clear(ze, ch1);
			}
		}
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
	//T81->Candidats();zl.Image();zpln.ListeMarques();
	//tac.Liste(); zcx.ListeLiens();
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

/*
void JDK::Influence (BF81 &zd,BF81 zo)
{for (int i=0;i<81;i++)if(zo.On(i)) zd=zd&t81f[i].z;}
//-------------
BF81 JDK::Points(int el,BF16 po)
{BF81 w; w.Init();
for(int i=0;i<9;i++)if(po.On(i)) w.Set(divf.el81[el][i]);  return w;}
    
*/

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
			// pour trouver un équilibre efficacité performance

			TDB::InitParsing();
			EE.E("contrôle parsing mini ");
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
// tout ce qui suit est provisoire et à préciser

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

