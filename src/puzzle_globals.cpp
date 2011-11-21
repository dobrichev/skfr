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

/* included here
            <<<<<<  TCHAIN module   >>>>>>
            <<<<<<  TWO_REGIONS_INDEX   >>>>>>
            <<<<<<  SEARCH_LS_FISH  >>>>>>
            <<<<<<  CELL  CELLS   >>>>>>
            <<<<<<  TPAIRES  >>>>>> 
            <<<<<<  ZGS table   >>>>>>
            <<<<<<  UL_SEARCH module   >>>>>>
            <<<<<< EVENT family classes >>>>>>>>>>


/*              <<<<<<  TCHAIN module   >>>>>>
            handling all potential eliminations through chains

			not yet fully thread safe
			global variables still there  
			zpln
			T81t

*/





/*	Upper bound for chains is actually unbounded: the longer chain, the higher rating.
 */

USHORT  steps[] = {4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128,
            192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192};

/* attach the instance to the puzzle
*/

void TCHAIN::SetParent(PUZZLE * parent,FLOG * fl){
 parentpuz=parent;
 EE=fl;}
/* start a new cycle (if chains are needed)
*/
void TCHAIN::NewCycle() {
	rating = 200;
	ichain = 0;
	elims_done=0;
	cycle_elims.SetAll_0();
	achieved_rating= parentpuz->ermax;
	maxlength = 10;
	}


/* accelerator for the search of loops
   to be revised
*/
void TCHAIN::SetMaxLength( int bw)  
       {base=bw;
		if(bw<rating) 
			{if(bw<75) maxlength=16; // same as no limit
			 else maxlength=100;}
		     // chexk why, but more creates a problem in bitfield
		else if(bw>80 || bw==75) maxlength=100;// linked to CANDGO
		else if(bw==rating )  maxlength=4;// equivalent to loop length 6
		else {int ii=rating - bw; maxlength=steps[ii]/2+2;}
       }
/* try to clear a candidate should always answer yes
*/

int TCHAIN::ClearChain(int ichain){ // clear candidates
 if(Op.ot){EE->E("clean for UREM=");EE->Enl(chains[ichain].urem);}
 CANDIDATE cw=zpln.zp[chains[ichain].cand];
 return T81t[cw.ig].Change(cw.ch,parentpuz);
}

int TCHAIN::ClearImmediate(USHORT cand){ // clear candidates
 if(cycle_elims.On(cand)) return 0;
 if(Op.ot){EE->Enl("immediate elimination through chain for candidate ");
           zpln.Image(cand);
            EE->Enl();
 }
 cycle_elims.Set(cand);
 CANDIDATE cw=zpln.zp[cand];
 int ir=T81t[cw.ig].Change(cw.ch,parentpuz); // should always answer yes
 if(ir) elims_done=1;   // but keep protected against loops
 return ir;
}

int TCHAIN::Clean() { // clear all pending candidates
 int ir = elims_done;  //  answer is yes is immediate eliminations done
 for(int i = 0; i < ichain; i++)
 		ir += ClearChain(i);
 return ir;
}



/* attempt to enter the table of pending eliminations.
   compute the rating and return 0 if
    . higher than current rating, 
	. equal to current rating and 
	   - table is full
	   - or the cand is already in pending eliminations
   if not
   return the rating found
*/
int TCHAIN::GetRatingBase(USHORT bb,USHORT lengthe,USHORT cand)
       { if(lengthe<2) return 0; // should not happen
		  USHORT wrating=300,length=lengthe-2; int ii;
          for(ii=0;ii<22;ii++) 
	         if(length<= steps[ii]) {wrating=bb+ii; break;}
          if(wrating>rating) return 0;
		  if(wrating==rating )
			  {if(ichain >=30)  return 0;
		       for(int i=0;i<ichain;i++) if(chains[i].cand==cand) return 0;
		      }
		  return wrating;}


 /* Load after GetRating
    check for safety that the load is valid

	if possible, (rating lower than the achived one)
	do immediate elimination
*/
void TCHAIN::LoadChain(USHORT rat,char * lib,USHORT cand)
{ if(rat>rating) return ;
  if(rat<rating) {ichain=0;  rating=rat;}
  if(rat<=achieved_rating) { //then do it 
	  ClearImmediate(cand);
	  return;
  }
  if(ichain>=30) return;
  if(Op.ot)
           {EE->Enl();parentpuz->PointK(); EE->Esp(); EE->Enl(lib);
			EE->E(" load tchain rating=");EE->E(rat);
		    EE->E(" elimination of ");zpln.Image(cand);
			EE->Enl();}
   chains[ichain].cand=cand;
   chains[ichain++].urem=parentpuz->couprem;
}


int TCHAIN::IsOK(USHORT x) {
	// push back an error as soon as possible
	if(parentpuz->stop_rating) return 1;
	if(elims_done) return 1;
	if(!ichain) return 0;
	int ir=((rating <= x) );
	return ir;
}



/*               <<<<<<  TWO_REGIONS_INDEX   >>>>>>

     alternative index for the puzzle
	 only on routine to update the index at the start of a cycle

*/
void TWO_REGIONS_INDEX::Genere() {
	int i, j;
	for(i = 0; i < 81; i++) {   // on charge tpobit
		CELL_FIX w = t81f[i];
		CELL_VAR x = T81t[i].v;
		tpobit.el[w.el].eld[w.pl].Genpo(x);
		tpobit.el[w.pl + 9].eld[w.el].Genpo(x);
		tpobit.el[w.eb + 18].eld[w.pb].Genpo(x);
	}
	// on génère tch a partir de tpo
	for(i = 0; i < 27; i++) {  // les 27 groupes
		for(j = 0; j < 9; j++)
			tchbit.el[i].eld[j].Raz();
		for(j = 0; j < 9; j++)
			for(int k = 0; k < 9; k++)
				if(tpobit.el[i].eld[j].b.On(k))
					tchbit.el[i].eld[k].Set(j);
	}
}

/*
           <<<<<<  SEARCH_LS_FISH  >>>>>>
   class used to search locked sets and fishes

*/

 void SEARCH_LS_FISH::SetParent(PUZZLE * parent, FLOG * xx){
 parentpuz=parent;
 EE=xx;
 regindp=parentpuz->alt_index.tpobit.el;
 regindch=parentpuz->alt_index.tchbit.el;
 regindchcol=&regindch[9];
}

//<<<<<<<<<<<<<<<<<<<<<   On cherche tiroir dans element  // boucle recurrente sur i
int SEARCH_LS_FISH::Tiroir(BF16 fd,int iold,int irang) { // on progresse sur indice et on regarde si fusion n-1  ok
	int i,id=(non.f && (irang-1))? 0:iold+1;  // debut 0 avec pseudos et rang 1
	for (i=id;i<(11-rangc+irang);i++)  // il doit rester des cases
	{ 
		if(eld[i].n<2 ||eld[i].n>rangv) continue;
		if( non.On(i))continue; //pour pseudo
		BF16 wx=eld[i].b|fd; 
		int nx=wx.QC();
		if (nx >rangv) continue;
		if(irang==(rangc-2))
		{wf=wx;wi.Set(i);
		if(nx ==rangv)return 1; else return 0; }
		// une erreur a tester    on poursuit si <
		// il peut manquer des tiroirs!!!!! curieux que jamais détecté!!!!
		if(Tiroir(wx,i,irang+1)){wi.Set(i); return 1; }
	}
	return 0;
}

int SEARCH_LS_FISH::Tiroirs(int nn,int hid,int sing) {     //recherche normale des tiroirs
	rangv=nn;single=sing;hid_dir=hid; int ir=0;
	int ied=0,ief=54; 
	if(!hid_dir )ief=27;if(hid_dir==1 )ied=27;if(single){ied=27;ief=54;}
	
	for( e=ied;e<ief;e++)   // direct, hidden or both upon request
	{rangc=rangv; non.f=cases.f=0; if(e<27) e27=e; else e27=e-27;
	if(parentpuz -> NonFixesEl(e%27) < (rangv +1)) continue;

	for(int i=0;i<9-rangv+1;i++)
	{eld=regindp[e].eld;
	int nn= eld[i].n;
	if(nn<2 || nn>rangv) continue;
	BF16 w;w=eld[i].b;
	wi.f=0; wi.Set(i);
	if(!Tiroir(w,i,0)) continue;
	if (UnTiroir())ir= 1;	
	}  
	}
	if(ir)return 1; 
	return 0;
}

int SEARCH_LS_FISH::UnTiroir() {// is there a single required after the locked set to accept it
	int ir=0;
	if(single) { // will be covered slowly can be any element row, col, box
		for(int i=0;i<9;i++)  if(wf.Off(i))
		{USHORT i8=divf.el81[e27][i]; 
		CELL p=T81t[i8];  if(p.v.typ ) continue;// must be non assigned 
		BF16 wc=p.v.cand-wi;
		for(int j=0;j<9;j++) if (wc.On(j) )// a possible hidden digit
		{BF16 wcd=regindch[e27].eld[j].b-wf; // positions still valid
		if(wcd.QC()==1)
		{EE->Enl("ecs assignment");
		parentpuz -> FaitGoA(i8,j+'1',4);// stop at first assignment
		ir=1; break;}
		}// end for j if

		}// end hidden ls
		if(!ir) return 0;// no single found
	}// end if single

	else if(e<27) {
		    if (!parentpuz ->ChangeSauf(e,wi,wf)&&(!ir) )return 0;  }
	else   { if (!parentpuz ->Keep(e27,wf,wi) &&(!ir))return 0;  }

	if(!Op.ot) return 1; 
	// describe the LS even if no more eliminations after an assignment

	char *gt[]={"LS2 ","LS3 ","LS4 ","LS5 " };

	int in=rangv-2,  it2=(e%27)/9,ii2=e%9;
	char c2= (it2-1)?(char)(ii2+'1'):lc[ii2];
	EE->E("->");EE->E(gt[in]); 
	if(e<27)
	{EE->E(" cells ");	EE->E( wi.String());EE->E(" digits ");  }
	else
	{EE->E(" digits ");EE->E( wi.String());EE->E(" cells "); }
	EE->E( wf.String());EE->E(" ");EE->E(orig[it2]);EE->E(" ");EE->Enl(c2);
	return 1;
}

 


//<<<<<<<<<<<<<<<<<<<<<   On cherche XW dans lignes ou cols  boucle recurrente sur i
int SEARCH_LS_FISH::XW(BF16 fd,int iold,int irang)  	// en élément i chiffre ch
{   // on progresse sur indice et on regarde si fusion n-1  ok
  for (int i=iold+1;i<9;i++)  // il doit rester des éléments
  { int nn=eld[ch].n;
	 if(nn<2 ||nn>rangv) continue;
    BF16 wfu=regxw[i].eld[ch].b|fd; 
	int nx=wfu.QC();
	if (nx >rangv) continue;
    if(irang==(rangv-2)){ if(nx - rangv)continue;
                         wf=wfu; wi.Set(i);return 1; }
      else if(XW(wfu,i,irang+1)) {wi.Set(i); return 1; }
  }
return 0;}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< en mode XWING liste objets et chiffres
int SEARCH_LS_FISH::GroupeObjetsChange(int dobj,int ch)
{BF16 x(ch); int ir=0;
 for(int i=0;i<9;i++)   if(wf.On(i)) ir+= parentpuz ->ChangeSauf(dobj+i,wi,x);
return ir;}
//<<<<<<<<<<<<<<<<<<<<<<<< ici chiffre en majeur éléments en mineur
int SEARCH_LS_FISH::XW(int nn)
{char *gxw[]={"XWing ","SwordFish  ","Jelly (XW4) ","Squid (XW5)  " ,
              "Whale (XW6)","Leviathan (XW7)" };
 BF16 w; rangc=nn;
 for(int i=0;i<9;i++)
  {ch=(UCHAR)i;
   for(int iel=0;iel<10-rangv;iel++)
   { regxw=regindch;
	 eld=regxw[iel].eld;     
   	 int nn=eld[i].n;
    if(nn>1 &&nn<=rangv)
    { w=eld[i].b; wi.f=0; wi.Set(iel);
      if( XW(w,iel,0) )
       { if(GroupeObjetsChange(9,i) )  // action colonnes
	     {EE->E(gxw[rangv-2]);	 EE->E(" digit ");     EE->E(i+1); 
	      EE->E( " columns ");   EE->E(wf.String(0));
		  EE->E(" rows ");  EE->Enl(wi.String());     
		  return 1; 	 }}
       }
	 regxw=regindchcol;
	 eld=regxw[iel].eld;
     nn=eld[i].n;
     if(nn<2 || nn>rangv) continue;
     w=eld[i].b;  	wi.f=0; wi.Set(iel);
     if( XW(w,iel,0) )
      { if(GroupeObjetsChange(0,i) ) // action lignes
	   {EE->E(gxw[rangv-2]);	 EE->E(" digit ");
	    EE->E(i+1); EE->E(" rows ");	EE->E(wf.String());
		EE->E( " columns ");	  EE->Enl(wi.String(0));     
		return 1; }  	 }
    } // end iel
  }    // end i niv
return 0;}



/*               <<<<<<  CELL  CELLS   >>>>>>

     main table for the cells of the  puzzle
	 this is the entry point for any elimination
	 keep updated situation of candidates and assigned/given

	 a copy of the table is needed in some steps as UR search


*/
int CELL::Change(int ch,PUZZLE * ppuz) {
	if(v.cand.Off(ch))
		return 0;
	if(ppuz->CheckChange(f->i8, ch))
		return 0;
	v.cand.Clear(ch);
	v.ncand = v.cand.CountEtString(scand);
	ppuz->c[ch].Clear(f->i8);
	return 1;
}

// obsolete to clean later

int CELL::Changex(int ch) {
	if(v.cand.Off(ch))
		return 0;
	if(puz.CheckChange(f->i8, ch))
		return 0;
	v.cand.Clear(ch);
	v.ncand = v.cand.CountEtString(scand);
	puz.c[ch].Clear(f->i8);
	return 1;
}

int CELL::Change (BF16 cb9,PUZZLE * ppuz) {    // clear candidates 
	   int ir=0;
	   for(int i=0; i < 9; i++) 
		   if(cb9.On(i))
			   ir += Change(i,ppuz);
	   return ir;
   }

// obsolete to clean later

int CELL::Changey (BF16 cb9) {    // clear candidates 
	   int ir=0;
	   for(int i=0; i < 9; i++) 
		   if(cb9.On(i))
			   ir += Changex(i);
	   return ir;
   }

int CELL::Keep (BF16 cb9,PUZZLE * ppuz) {       // clear candidates others than
	   int ir=0;
	   for(int i = 0; i < 9; i++) {
		   if(v.cand.On(i) && !cb9.On(i)) {
			   Change(i,ppuz);
			   ir=1;
		   }
	   }
	   return ir;
   }

int CELL::Keep(int ch1,int ch2,PUZZLE * ppuz) { // clear candidates others than
	   int ir = 0;
	   for(int i = 0; i < 9; i++) {
		   if(v.cand.On(i) && (i - ch1) && (i - ch2)) {
			   Change(i,ppuz);
			   ir = 1;
		   }
	   }
	   return ir;
   }

int CELL::Keepy (BF16 cb9) {       // clear candidates others than
	   int ir=0;
	   for(int i = 0; i < 9; i++) {
		   if(v.cand.On(i) && !cb9.On(i)) {
			   Changex(i);
			   ir=1;
		   }
	   }
	   return ir;
   }

int CELL::Keepy(int ch1,int ch2) { // clear candidates others than
	   int ir = 0;
	   for(int i = 0; i < 9; i++) {
		   if(v.cand.On(i) && (i - ch1) && (i - ch2)) {
			   Changex(i);
			   ir = 1;
		   }
	   }
	   return ir;
   }

  void CELL::Fixer(UCHAR type,UCHAR ch) { // force digit as valid in the solution
	   v.Fixer(type, ch);
	   scand[1] = 0;
	   scand[0] = '1' + ch;
   }


void CELLS::SetParent(PUZZLE * parent,FLOG * fl){
 parentpuz=parent;
 EE=fl;}


void CELLS::init() {
	for(int i = 0; i < 81; i++) {
		t81[i].v.Init();
		t81[i].f = &t81f[i];
	}
}
void CELLS::Fixer(int ch, int i8, UCHAR typ) {
	t81[i8].Fixer(typ, ch);
	parentpuz->cFixer(ch, i8);
}

int CELLS::Clear(BF81 &z, int ch) {
	//EE->E("clear tCELL ");EE->E(ch+1);z.ImagePoints();  EE->Enl();
	int ir = 0;
	for(int i = 0; i < 81; i++)
		if(z.On(i))
			ir += t81[i].Change(ch,parentpuz);
	return ir;
}
int CELLS::Clear(BF81 &z, BF16 x) {
	int ir = 0;
	for(int j = 0; j < 9; j++)
		if(x.On(j))
			ir += Clear(z,j);
	return ir;
}
//<<<<<<<<<<<<<<<<<<<<    specific ot UR/UL filter to find the lowest length
int CELLS::CheckClear(BF81 &z, BF16 x) {
	for(int i = 0; i < 81; i++)
		if(z.On(i))
			if((t81[i].v.cand&x).f)
				return 1;
	return 0;
}// positive as soon as one effect found

//<<<<<<<<<<<<<<<<<<<<<<<<<
void CELLS::Actifs(BF81 & z) {
	z.SetAll_0();
	for(int i = 0; i < 81; i++)
		if(!t81[i].v.typ)
			z.Set(i);
}
//<<<<<<<<<<<<<<<<<
BF16 CELLS::GenCand(BF81 & z) {
	BF16 w;
	w.f = 0;
	for(int i = 0; i < 81; i++)
		if(z.On(i) && (!t81[i].v.typ))
			w = w | t81[i].v.cand;
	return w;
}
//<<<<<<<<<<<<<<<<<     y compris assigned pour RIs
BF16 CELLS::GenCandTyp01(BF81 & z) {
	BF16 w;
	w.f = 0;
	for(int i = 0; i < 81; i++)
		if(z.On(i) && t81[i].v.typ < 2)
			w = w | t81[i].v.cand;
	return w;
}
/*
//<<<<<<<<<<<<<<<<
void 	 CELLS::GenzCand(BF81 & z1,BF81 & z2,int ic)
{z2.Init();  for(int i=0;i<81;i++)
if(z1.On(i)&&(!t81[i].v.typ)&&t81[i].v.cand.On(ic)) z2.Set(i);  }
*/
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void CELLS::Candidats() {
	if(!Op.ot)
		return; 
	int i, j, l, lcol[9], tcol = 0;
	char * pw;       //lcol  largeur maxi colonne
	EE->Enl("PM map ");  
	for(i = 0; i < 9; i++) {  // attention ici i indice colonne
		lcol[i] = 2;    // 2  mini tous chiffres imposés
		for(j = 0; j < 9; j++) {
			l = (int)strlen(t81[9 * j + i].strcol()); 
			if(l > lcol[i])
				lcol[i] = l;
		}
		tcol += lcol[i];
	}
	EE->Enl();
	for(i = 0; i < 9; i++) {
		if((i == 3) || (i == 6))
			EE->E("|");
		EE->E((char)('A' + i));
		EE->E(Blancs(lcol[i], 1));
	} 
	EE->Enl();
	for(i = 0; i < 9; i++) { // maintenant indice ligne
		if((i == 3) || (i == 6)) {
			for(int ix = 0; ix < (tcol + 10); ix++)
				EE->E((char)'-');
			EE->Enl();
		}
		for(j = 0; j < 9; j++) {
			if((j == 3) ||(j == 6))
				EE->E("|");
			CELL * pp8 = &t81[9*i + j];
			pw = pp8->strcol();		  
			EE->E(pw);
			EE->E(Blancs(lcol[j] + 1 - (int)strlen(pw), 1));
		} // end for j
		EE->Enl();
	} // end for i
	EE->Enl("\n\n");
}





int CELLS::RIN(int aig) {      // look for unique rectangle 
	int ir=0;
	urt.Init();
	for(int i0 = 0; i0 < 3; i0++) // band/stack 1 to 3
		for(int i1 = 0; i1 < 2; i1++)
			for(int i2 = i1 + 1; i2 < 3; i2++) // 2 rows  
				for(int j1 = 0; j1 < 2; j1++)
					for(int j2 = j1 + 1; j2 < 3; j2++) // boxes   12 13 23
						for(int j3 = 0; j3 < 3; j3++)
							for(int j4 = 0; j4 < 3; j4++) {  // 2 cols  
								//analysing band and stack, main diagonal symmetry
								int l1 = 3 * i0 + i1, l2 = 3 * i0 + i2, c1 = 3 * j1 + j3, c2 = 3 * j2 + j4;
								if(ur.RID(l1, l2, c1, c2) || ur.RID(c1, c2, l1, l2))
									ir++;
							}
	return ir;
}


/*
           <<<<<<  TPAIRES  >>>>>>
   class used for all processes based on pairs

   BUGS  XYwing  XYZWing and partly ULs

*/




 void TPAIRES::SetParent(PUZZLE * parent, FLOG * xx){
 parentpuz=parent;
 EE=xx;
}

//====================================
int TPAIRES::UL() {
	int ir = 0;
	tult.Init();
	for(int i = 0; i < np; i++) {
		USHORT id = izpd[i], ie = izpd[i + 1];
		// EE->Enl("un depart paire");
		UL_SEARCH uls(tp[i], this, &zpt[id], ie - id,EE); //build search 
		for(int j = id; j < ie - 1; j++) {
			for(int k = j + 1; k < ie; k++) {
				USHORT i1 = zpt[j].i8, i2 = zpt[k].i8;
				CELL_FIX p1 = t81f[i1], p2 = t81f[i2];
				if(!(p1.el==p2.el || p1.pl==p2.pl))
					continue; // start row or col
				//  EE->E(p1.pt); EE->E(p2.pt);EE->Enl("un depart lig/col");
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
	EE->Enl("debut recherche bug");
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
	if(ntplus == 1)
		return Bug1();
	if(Bug2())
		return 1; // priority to bug 2
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
	T81t[i8].Keepy(w); // eliminate the others
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
			ir += T81t[i].Changex(ch);
	if(ir) {
		BugMess("2 same digit");
		puz.SetDif(57);
	}
	return ir;
}
//===========================================
void TPAIRES::BugMess(const char * lib) const {
	EE->E("Bug type ");
	EE->Enl(lib);
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
	EE->Enl("recherche en boite"); 

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

	EE->Enl("recherche en boite phase 1"); 

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
	EE->E("recherche en boite phase 2"); 
	EE->E(" wpar");
	EE->Enl(wpar.String());

	wpar ^= el_par_ch[el];  // adjust parity in the box

	// finish the task if one has no row/col free
	for(int i=0;i<ntplus;i++) {
		if(phx[i])
			continue;// done
		pp = &T81t[tplus[i]]; 
		wcx = pp->v.cand & wpar;
		annul = pp->v.cand - wcx;
		EE->E(t81f[tplus[i]].pt);
		EE->Esp();
		EE->E(" wpar");
		EE->E(wpar.String());
		EE->E(" wcx");
		EE->Enl(wcx.String());
		if((wcx.QC() - 2))
			return 0;
		welim |= annul;
	}	 

	return Nacked_Go(welim);
}

int TPAIRES::Nacked_Go(BF16 welim) {
	//we look now for  "naked locked sets"
	EE->E("recherche  bug3_4 Nacked ok to go welim= ");
	EE->Enl(welim.String());
	int nelim = welim.QC(); // look for naked in increasing order
	if(nelim < 2 || nelim > 5)
		return 0;
	if(brat == 58 && nelim < 3) { //then search for naked 2
		for(int i = 0; i < ntpa; i++) {
			if(T81t[tpa[i]].v.cand.f == welim.f) { // we found it
				int ir = 0;
				for(int j = 0; j < ntpa; j++)
					if(j-i)
						ir += T81t[tpa[j]].Change(welim,parentpuz);
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
						ir += T81t[tpa[j]].Change(ww,parentpuz);
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
							ir += T81t[tpa[j]].Change(ww,parentpuz);
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
								ir += T81t[tpa[j]].Change(ww,parentpuz);
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
	EE->Enl("recherche  bug_lock"); 
	BF16 clock = candnp_or - candp_or;  // locked candidate
	EE->E(" clock=");
	EE->Enl(clock.String());
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
	EE->E(p1.f->pt);
	EE->E(" el=");
	EE->E(el1 + 1);
	EE->E(" wc1=");
	EE->Enl(wc1.String());
	EE->E(p2.f->pt);
	EE->E(" el=");
	EE->E(el2 + 1);
	EE->E(" wc2=");
	EE->Enl(wc2.String());

	if(wce2.f-wce1.f) return 0; // must be the same digit
	if((wc1.QC() - 2) || (wc2.QC() - 2))
		return 0;	 

	T81t[tplus[0]].Keepy(wce1 | clock);
	T81t[tplus[1]].Keepy(wce2 | clock);
	BugMess("3/4 a digit locked");
	puz.SetDif(57);
	return 1;
}

//============================================
int TPAIRES::Bug3_4_Nacked(int el) {
	EE->Enl("recherche  bug3_4 Nacked"); 

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
		//EE->E(t81f[tplus[i]].pt); EE->E(" el=");EE->E(elx+1);
		//EE->E(" wc=");EE->Enl(wcx.String());
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
	EE->E(lib);
	EE->E(t81f[k].pt);
	EE->E(" ");
	EE->E(T81t[k].scand);	 
	EE->E(" (1)=");
	EE->E(t81f[zp[i].i8].pt);
	EE->E(" ");
	EE->E(T81t[zp[i].i8].scand);
	EE->E(" (2)=");
	EE->E(t81f[zp[j].i8].pt);
	EE->E(" ");
	EE->Enl(T81t[zp[j].i8].scand);	
}

int TPAIRES::CommunPaires(int i, int j) {
	return T81t[zp[i].i8].ObjCommun(&T81t[zp[j].i8]);
}
int TPAIRES::CommunTrio(int i, int j) {
	return T81t[i].ObjCommun(&T81t[zp[j].i8]);
}

/*              <<<<<<  ZGS table   >>>>>>
        list of all BF81 in use
		can disappear in an optimised code for skfr
		inherited from GP solver where more pattern were used
		as it is, it is a a "quasi constant"
		initialised by the constructor

		81 BF81 for the cells
		then 54 BF81 for the groups row/box column box

*/


ZGROUPE::ZGROUPE () {
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
	//ReInit();
}






/*             <<<<<<  UL_SEARCH module   >>>>>>

that clas is a support to find a UL
each occurence is normally stored for further processins


*/



// constructor to start a UL search


UL_SEARCH::UL_SEARCH(BF16 c, TPAIRES * tpae, PAIRES * pae, USHORT npae,FLOG * xx ) {
		// initial for a new start
		EE=xx;
		tpa = tpae;
		pa = pae;
		npa = npae;
		chd = cht = c;
		nadds = line_count = 0; 
		char * st = c.String();
		c1 = st[0] - '1';
		c2 = st[1] - '1';
		cells.SetAll_0();
		el_used.f = parity.f = 0;
		for(int i = 0; i < 27; i++)
			elcpt[i] = 0;
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
	// EE->E("UL lc=");EE->E(line_count);EE->Esp();EE->Enl(p.pt);
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
		//EE->E("UL end ");EE->E(t81f[i8].pt);EE->Esp();cells.ImagePoints();EE->Enl();
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
	//EE->E("suite el=");EE->Enl(ele+1);
	BF16 wc = puz.alt_index.tchbit.el[ele].eld[c1].b & puz.alt_index.tchbit.el[ele].eld[c2].b;
	for(int i = 0; i < 9; i++) {
		if(wc.On(i)) { // cells with both digits
			int i8r = divf.el81[ele][i];
			//EE->E("essai i8=");EE->Enl(t81f[i8r].pt);
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
		if(T81t[iu].Changey(chd)) {
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
	EE->Enl();
	EE->E("UL loop nadds=");
	EE->E(nadds);
	EE->E(" count=");
	EE->E(line_count - 1);
	EE->E(" rating=");
	EE->E(puz.difficulty);
	for(int i = 0; i < line_count; i++) {
		EE->Esp();
		EE->E(t81f[tcount[i]].pt);
	}
	EE->Enl();
	if(pr) {
		EE->E("active due to " );
		EE->Enl(lib);
	}
}
 
/*           <<<<<< EVENT family classes >>>>>>>>>>

  EVENT is the name given to a state of the PM where a specific pattern occurs

  eg  XWing, Nacked pait, Hidden pair ...

  Any event has 3 component

  (1) the set of candidates that must be false to have the "event"
  (2) the event description (candidates)
  (3) the set of candidates cleared if the "event" is established

  set (1) is loaded in the table of sets
  set (3) is an EVENLOT
  set (2) is located in the EVENT


  all potential events are search for each cycle and stored in TEVENT

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

int EVENTLOT::GenForTag(USHORT tag, WL_TYPE type,FLOG * EE) const {
	for(int i = 0; i < itcd; i++) {
		USHORT cand2 = tcd[i], tag2 = (cand2 << 1) ^ 1;
		if(zcf.Isp(tag, tag2))
			continue;// only if not yet defined
		if(0 && (tag ==(tag2 ^ 1))) {
			EE->E("gen contraire");
			Image(EE);
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

void EVENTLOT::Image(FLOG * EE) const {
	EE->E(" evenlot itcd=");
	EE->E(itcd);
	for(int i = 0; i < itcd; i++) {
		EE->Esp();
		zpln.Image(tcd[i]);
	}
	EE->Enl();
}

             //============================


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


int EVENT::IsPattern (USHORT cand) const {
	for(int i = 0; i < ntcand; i++)
		if(cand == tcand[i])
			return 1;
		return 0;
}


void EVENT::Image(FLOG * EE) const {
	if(!Op.ot)
		return;
	EE->E(" event image tag=");
	EE->E(tag);
	EE->E(" type=");
	EE->E(type); 
	for(int i = 0; i < ntcand; i++) {
		EE->Esp();
		zpln.Image(tcand[i]);
	} 
	evl.Image(EE);
}
void EVENT::ImageShort(FLOG * EE) const {
	char * tlib[]={"pointing rc","pointing b","naked pair","hidden pair","Xwingr","Xwingc"};
	if(!Op.ot)
		return;
	EE->E("\t event   ");
	EE->E(tlib[type]); 
	EE->E("\t\t");
	for(int i = 0; i < ntcand; i++) {
		EE->Esp();
		zpln.Image(tcand[i]);
	} 

}
               //================================

void TEVENT::SetParent(PUZZLE * parent,FLOG * fl)
{parentpuz=parent; 
EE=fl;}

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


// initial seacrh with the priority to the first found
void TEVENT::LoadAll() {
	Init();
	LoadLock();
	LoadPairs();
	LoadXW();
}


void TEVENT::EventBuild(EVENT_TYPE evt, EVENTLOT & eva, EVENTLOT & evb, EVENTLOT & evx) {
	if(eva.itcd < 1 || evb.itcd < 1)
		return;
	// if only one in the set, go to gen and return
	if(eva.itcd == 1) {
		USHORT cw = eva.tcd[0]; 
		if(0 && Op.ot) {
			EE->E("gen direct event type=");
			EE->E(evt);
			EE->Esp(); 
			eva.Image(EE);
			evb.Image(EE);
			EE->Enl();
		}
		evb.GenForTag((cw << 1) ^ 1, wl_ev_direct,EE);
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
	if(t[ind].evl.GenForTag(tagfrom, wl_event,EE)) { // diag a supprimer
		t[ind].Image(EE);
		return 1;
	} 
	return 0; 
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

	EE->E("check after having loaded events nb events=");
	EE->Enl(it);
	EE->E("zcx izc=");
	EE->Enl(zcx.izc);
	for(int i = 1; i < it; i++) {
		EVENT ev = t[i]; 
		EE->E("ev type=");
		EE->E(ev.type);
		EE->E(" tag=");
		EE->E(ev.tag);
		for(int j = 0; j < ev.ntcand; j++) {
			EE->Esp();
			zpln.Image(ev.tcand[j]);
		}
		ev.evl.Image(EE);
	}
	zcx.Image();
}

