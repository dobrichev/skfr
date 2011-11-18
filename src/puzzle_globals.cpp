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
		UL_SEARCH uls(tp[i], this, &zpt[id], ie - id); //build search 
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




