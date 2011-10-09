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


void PATH::PrintPathTags() {
	zpln.PrintImply(pth,ipth);
}


void CHAIN::Load(USHORT cande) {
	urem = puz.couprem;
	cand=cande;
}

/*	Upper bound for chains is actually unbounded: the longer chain, the higher rating.
 */

USHORT  steps[] = {4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128,
            192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192};
/* clean a candidate if the final rating is ok
*/
int CHAIN::Clean() // clear candidates
     {if(Op.ot){EE.E("clean for UREM=");EE.Enl(urem);}
      ZPTLN cw=zpln.zp[cand];
	  return T81t[cw.ig].Change(cw.ch);}
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
/* same gaol in dynamic mode
   to be revised as well
   */
USHORT TCHAIN::ParsingMini(USHORT basee) {if(rating>(basee+10)) return 200; //max
                                   if(rating <=basee) return 6; // mini
								   int ii=rating - basee; return(steps[ii]/2+2);}
/* another function to be revised
*/
int TCHAIN::GetYlower(int lg) // limit for a shorter chain/loop
       {int xl=2*(lg-1); //  equivalent length pure Y mode
        if(xl<7 ) return 0; //nothing shorter can be found 
		if(xl>32) return 0;// should never happen
		xl-=2; // the length for index
		for(int ii=0;ii<6;ii++) if(xl<steps[ii+1])	return steps[ii]/2+2;
        return 0;} // shoul never happen
/* new function to control performances
   the target is the following
   a chain has been found, what is the maximum length for a loop
     having a lower rating
  in 'X' 'Y' mode, the base is 0.1 lower
  in 'XY' mode the base is the same
  modexy is 1 if "XY" else it must be 0
*/
 int TCHAIN::GetLimit(int lg,int modexy) // limit for that group
       {lg-=2; // the length for index
		for(int ii=0;ii<10;ii++) if(lg<=steps[ii])	
			return (steps[ii]-modexy);
        return 0;} // shoul never happen
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
    check for safety the load is valid
*/
void TCHAIN::LoadChain(USHORT rat,char * lib,USHORT cand)
{ if(rat>rating) return ;
  if(rat<rating) {ichain=0;  rating=rat;}
  if(ichain>=30) return;
  if(Op.ot)
           {EE.Enl();puz.PointK(); EE.Esp(); EE.Enl(lib);
			EE.E(" load tchain rating=");EE.E(rat);
		    EE.E(" elimination of ");zpln.Image(cand);
			EE.Enl();}
   chains[ichain++].Load(cand);
}

int TCHAIN::IsOK(USHORT x) {
	int ir=((rating <= x) || (rating <= puz.ermax));
	if(ir && Op.ot) {
		EE.E(" sortie isok chaine x=");
		EE.E(x); 
		EE.E(" rating=");
		EE.E(rating);  
		EE.E(" ermax==");
		EE.Enl(puz.ermax); }
	return ir;
	//{return ((rating <= x ) || (rating <= Op.ermax));}
}

void TCHAIN::Status() {
	EE.E("tchain ichain=");
	EE.E(ichain);
	EE.E(" rating=");
	EE.Enl(rating);
}



ZGROUPE::ZGROUPE (PUZZLE * parent)          
{   parentpuz=parent;
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
USHORT ZGROUPE::Charge(BF81  &ze)
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



BF81 * ZPTLN::GetZ() {
	return &zgs.z[ig];
}     

void ZPTLN::Clear(){T81t[ig].Change(ch); }
void ZPTLN::Image(int no) {EE.Esp();if(no)EE.E("~");
                     EE.E(ch+1);EE.E(t81f[ig].pt); }

/* creates the table of candidates
   and the direct index cell+ digit -> cand
   looks for true candidates
   init table zcf
   set up the usefull limit for BFCAND and CB1024 
 */


void TZPTLN::Init() {
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
	// BFTAG::SetIsize(puz.col); //MD: removed
	// BFCAND::SetIsize(ip);
}
/* just put zp[0] in the next position 
   check for free room just in case
   if no free room, puz.stop_rating is set to 1 thru Elimite
*/
USHORT TZPTLN::Charge0()
{if(ip>=zpln_lim){  parentpuz->Elimite("TZPLN"); return 0;}
 USHORT ir=ip; zp[ip++]=zp[0]; return ir;}

/* send in TZCF all weak links
   if it is a bi value, then 2 weak links are created
      a - b  and   ~a - ~b
   one entry for 'X' mode and one entry for 'Y' mode
   in 'X' mode we check for no double entry

*/

// this is a process specific to 'Y' mode
// only cell bivalues + 
void  TZPTLN::CellStrongLinks()
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
void  TZPTLN::CellLinks() 
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
void  TZPTLN::RegionLinks(USHORT ich,int biv)
{for (el=0;el<27;el++)
   { iptsch=0;  if(aztob.tchbit.el[el].eld[ich].n <2 )continue;
     for(int i=0;i<9;i++)
       {USHORT w=indexc[divf.el81[el][i]+81*ich]; 
		if(w) ptsch[iptsch++]=w;}
	if(iptsch==2 && biv) {zcf.LoadBivalue(ptsch[0],ptsch[1]);
	                      zcf.LoadBase(ptsch[0],ptsch[1]);}
	else    if(iptsch)WeakLinksD();
   }
}
void  TZPTLN::WeakLinksD()
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
void TZPTLN::GenRegionSets()     // can only be one per row col box, only if more than 2 candidates
{  USHORT mch[10];       
   for( int ich=0;ich<9;ich++) for (int el=0;el<27;el++)
   {USHORT nmch=aztob.tchbit.el[el].eld[ich].n,ipts=0;;
	if(nmch<3) continue; // minimum set size is 3
	BF81 zel=divf.elz81[el]&puz.c[ich];  int j;
    for(j=1;j<ip;j++)
     {if(zp[j].ch-ich )continue;
      if(zel.On(zp[j].ig) ) mch[ipts++]=j; 
     }
     zcx.ChargeSet(mch,nmch,CH_base);
    }
}
void  TZPTLN::GenCellsSets()
{for(USHORT i=0;i<81;i++)
  {USHORT n= T81t[i].v.ncand; if(n<3 || n>chx_max) continue;    
   BF16 ch8=T81t[i].v.cand;  
   USHORT nm=0,tm[9];    for(int j=0,jp=i;j<9;j++,jp+=81) if(ch8.On(j))
     {tm[nm++]=indexc[jp];}
     zcx.ChargeSet(tm,nm,CH_base);  
    }  }

void TZPTLN::Liste()
{EE.Enl("Liste des candidats ");
 for(int j=1;j<ip;j++)  {zp[j].Image();   EE.Enl();}  }

void TZPTLN::ListeIndexc()
{EE.Enl("Liste des candidats via index ");
 for(int i=0;i<81;i++) for(int ich=0;ich<9;ich++) 
 {USHORT j=indexc[ich*81+i];
 if(j)  {zp[j].Image();   EE.Enl();}  }
}

 void  TZPTLN::PrintPath(USHORT * tp,USHORT np)
{if(!Op.ot) return; EE.Echem();
 for(int i=0;i<np;i++)
  {ImageTag(tp[i]);
  if(i<np-1) if(i&1)EE.Esl(); else EE.Ewl(); } 
 EE.Enl();}

 void  TZPTLN::PrintImply(USHORT * tp,USHORT np)
{if(!Op.ot) return; EE.Echem();
 for(int i=0;i<np;i++)
  {USHORT px=tp[i];  ImageTag(tp[i]);
  if(i<np-1) EE.E(" -> ");  } 
 EE.Enl();}

void  TZPTLN::PrintListe(USHORT * tp,USHORT np,int modetag)
{if(!Op.ot) return; EE.E("candidats");
 for(int i=0;i<np;i++)
  {USHORT px=tp[i];  
  if(modetag) ImageTag(px); else  zp[px].Image();
  EE.Esp();  } 
 EE.Enl();}




BFTAG TDB::done[50]; 
USHORT  TDB::ich, TDB::mode, TDB::length, TDB::mmfin, TDB::maxlength;
PATH   TDB::path,TDB::pathr;
USHORT TDB::maxichain,TDB::maxsearch,TDB:: parsediag,TDB:: parsecount;
BFCAND TDB::tbf_end,TDB::tbf_endok; 

void TDB::Parents(USHORT x) {
	parents.SetAll_0();
	for(int i=2;i< puz.col;i++)
		if(t[i].On(x))
			parents.Set(i);
}

/* the key routine expanding pieces of path out of a->b elementary components
   ExpandAll is the fast mode using in bloc what has already been done  
   partial mode are found in the BFTAG functions
   */ 

void TDB::ExpandAll(TDB & from) {
	(*this)=from; // be sure to start with the set of primary data
	for(int i=2;i< puz.col;i++) {
		if (t[i].IsEmpty())
			continue;
		int n=1;
		while(n) {
			n=0;
			for(int j=2;j< puz.col;j++)
				if((j-i) && t[i].On(j)) {
					BFTAG x=t[j]-t[i];
					if(x.IsNotEmpty()) {
						t[i]|=x;n++;
					}
				}
		} // end j  while
	} 
}// end i   proc


void TDB::ExpandShort(TDB & from ,int npas)
{(*this)=from; // be sure to start with the set of primary data
 for( int i=2;i< puz.col;i++)
  {if (t[i].IsEmpty())continue;   int n=1,pas=0;
   while(n && (++pas<npas))
    { n=0;for(int j=2;j< puz.col;j++)   if((j-i) && t[i].On(j))
	 {BFTAG x=from.t[j]-t[i];	  if(x.IsNotEmpty()) {t[i]|=x;n++;}
   }} // end j  while
  } }// end i   proc

/* that table is prepared for the derivation of weak links
   the "from" table is the table of implications
   */
void TDB::AllParents(TDB & from)
{t[0].SetAll_0();for(int i=1;i< puz.col;i++) t[i]=t[0];
 for(int i=2;i< puz.col;i++) for(int j=2;j< puz.col;j++) 
	 if(from.t[i].On(j)) t[j].Set(i);
// EE.Enl("parents"); Image();
}

/* That process is valid only if no derived weak link has been produced
   The search finds the lot of shortest eliminations same length
   return 0 if nothing
   return the length if elimination(s) have been found

   from is the TDB of elementary weak links
   elims is set to 1 for all tags found in that process

   only "true" state of non valid candidates are expanded
*/

int TDB::SearchEliminations(TDB & from,BFTAG & elims)
{int npas=0,aigt=0;  elims.SetAll_0();
 (*this)=from; // be sure to start with the set of primary data
while(1)
{int aig=1; // to detect an empty pass
 npas++;
 for( int i=2;i< puz.col;i+=2)  // only "true" state
	 if (zpln.candtrue.Off(i>>1) &&  // candidate not valid
		 t[i].IsNotEmpty()               // should always be
		 )
  {for(int j=2;j< puz.col;j++)   if((j-i) && t[i].On(j))
	 {BFTAG x=from.t[j]-t[i];	  
      if(x.IsNotEmpty()) {t[i]|=x;aig=0;}
     }
   if(t[i].On(i^1)) // an elimination is seen
     {elims.Set(i); aigt=1;}
  }   
 if(aigt) return npas; // eliminations found
 if(aig) return 0;     // empty pass
}// end while
}


//<<<<<<<<<<<<<<<<<<<<<<<
void TDB::Image() {EE.Enl( "Image zone tdb");
 for(int i=2;i< puz.col;i++)  if(t[i].IsNotEmpty())  puz.Image(t[i]," ",i);   }


int TZCF::DeriveCycle(int nd, int nf, int ns, int npas) {
	int icd = ic;  // to check if something new comes
	hstart = h; // copy to have the situation at the start
	PlusPhase(); // save for later comments in test mode
	zcx.Derive(nd, nf, ns);
	if(ic == icd) return 0;
	if(!npas)
		h.d.ExpandAll(h.dp);
	else
		h.d.ExpandShort(h.dp, npas);
	return 1;
}

// put in the table if there is some room
// return 1 if no room
int TZCF::LoadVoid(USHORT cd1, USHORT cd2, WL_TYPE type, USHORT ph) {
	ic++;
	//if(ic>=zcf_lim) {EE.Elimite("ZCF");return 1;}
	//f[ic++].Load(cd1,cd2,type,ph);
	return 0;
} 

// entry for basic weak link a - b
void TZCF::LoadBase(USHORT cd1 ,USHORT cd2) {
	if(LoadVoid(cd1, cd2, wl_basic, 0))
		return;  
	Entrep(cd1 << 1, cd2 << 1);
}

// entry for bi value  a = b thru a - b and ã - ~b   
void TZCF::LoadBivalue(USHORT cd1, USHORT cd2) {
	if(LoadVoid(cd1, cd2, wl_bivalue, 0))
		return;  
	Entrep((cd1 << 1) ^ 1, (cd2 << 1) ^ 1);  
	isbival.Set(cd1);
	isbival.Set(cd2);
}

// entry for weak link derived from a set  a -> b
void TZCF::LoadDerivedTag(USHORT tg1, USHORT cd2) {
	if(LoadVoid(tg1, cd2, wl_set, iphase))
		return;
	Plusp(tg1, cd2 << 1);  //  a -> b
}

// entry for event  a - b
void TZCF::LoadEventTag(USHORT tg1, USHORT cd2) {
	if(LoadVoid(tg1, cd2, wl_event, iphase))
		return;
	Plusp(tg1, (cd2 << 1) ^ 1); // a -> ~b
}

// entry for direct event  ~a - b
void TZCF::LoadEventDirect(USHORT cd1, USHORT cd2) {
	if(LoadVoid(cd1, cd2, wl_ev_direct, iphase))
		return;
	Plusp((cd1 <<1 ) ^ 1, (cd2 << 1) ^ 1); 
}

//former _10g_cf_chain_plus.cpp follows

/* looking for elimination in dynamic mode
   in SE, such eliminations are always worked out of 2 chains
   analyzed form the left to the right.

   That process works in the same way, so elimination is one of the followings

   a-> b  and a -> ~b  (the classical  form a->~a is included here)
   x-> ~a and ~x -> ~a
   
   region or cell multi chain  a -> {~x ~y ~z);

   to be revisited to optimize the length control
   provisionnal status, each search is force to parsemini=100 in direct path
   */

void TZCF::ChainPlus(BFCAND & dones) {
	BFTAG *t = h.d.t, *tp = h.dp.t; 
	for(int i = 2; i < puz.col; i += 2) {
		BFTAG zi = (t[i].Inverse()).TrueState(), zw1 = t[i] & zi,
			zw2 = (t[i] & t[i^1]).FalseState();
		if(zw1.IsNotEmpty()) { // this is a a-> b  and a -> ~b
			if(1 && Op.ot) {
				//long tw=GetTimeMillis();
				//  int dt=tw-tdebut;
				// EE.E("time =");EE.Enl(dt);
				EE.E("found active a->x and a->~x");
				puz.Image(zw1," elims",i);
			}
			CANDGO cg;
			cg.GoCand(i); 
		}
		// if we start form a bi value, SE does not say
		// ~x but goes immediatly to the bi-value  saving one step.
		if(zw2.IsNotEmpty()) { // this is x-> ~a and ~x -> ~a
			if(1 && Op.ot) {
				//long tw=GetTimeMillis();
				// int dt=tw-tdebut;
				//  EE.E("time =");EE.Enl(dt);
				EE.E("found active x->~a and ~x->~a");
				puz.Image(zw2,"elims", i);
			}
			CANDGO cg1;
			cg1.GoOne(i, zw2);
			CANDGO cg2;
			cg2.GoOne(i ^ 1, zw2);
			BFTAG bf1 = zw2 & cg1.cumsteps[cg1.npas],
				bf2 = zw2 & cg2.cumsteps[cg2.npas],
				bft = bf1 & bf2;
			if(bft.IsEmpty())
				continue;
			// find the smallest overall length
			int totlength = 300, i2f = 0;
			for(int i2 = 3; i2 < puz.col; i2 += 2)
				if(bft.On(i2)) {
					EE.E("essaichain plus killing ");
					zpln.Image(i2>>1);
					EE.Enl();	
					int l1 = cg1.GoBack(i2, 0), l2 = cg2.GoBack(i2, 0);
					if((!l1) || (!l2)) continue;// should never be
					// if chain 2 starts by a strong link, reduce the count by one
					if((zcf.dpbase.t[i^1] & cg2.cumsteps[1]).IsNotEmpty())
						l2--;
					int ll = l1 + l2;
					if(ll<totlength) {
						totlength = ll;
						i2f = i2;
					}
				}
				if(i2f) {
					int ratch = tchain.GetRating(totlength, i2f >> 1);
					if(ratch) { // chain is accepted load it (more comments in test mode)
						if(Op.ot) { // this can be complex and will be developped later
							EE.E("chain plus killing ");
							zpln.Image(i2f >> 1);
							EE.Enl();	     
							EE.E("chain 1 ");
							cg1.GoBack(i2f, 1);
							EE.E("chain 2 ");
							cg2.GoBack(i2f, 1);
						}
						tchain.LoadChain(ratch, "chain plus", i2f >> 1);
					}
				}// end if i2f
		}// end if zw2
	} // end for i

	if(tchain.base < 85)
		return;

	// now check multichains in a similar way but not for nishio
	for(int ie = 1; ie < zcx.izc; ie++) {
		const ZCHOIX &chx = zcx.zc[ie];
		if(chx.type - CH_base)
			break; // only base sets can be used
		int n = chx.ncd;
		USHORT *tcd = chx.tcd;
		BFTAG tbt, bfset;
		tbt.SetAll_1();
		for(int i = 0; i < n; i++)
			bfset.Set((tcd[i] << 1) ^ 1);
		for(int i = 0; i < n; i++)
			tbt &= (t[tcd[i] << 1] - bfset); 
		if(!tbt.IsNotEmpty())
			continue;
		// candidate(s) to clear found
		if(Op.ot&& 0) {
			puz.Image(tbt," eliminations  multi chain dynamic mode  ", 0);
			chx.Image();
			EE.Enl();
		} 
		for(int j = 3; j < puz.col; j += 2) {
			if(!tbt.On(j))
				continue;
			// all tags killed
			int tot_length = 0; 
			if(Op.ot && 0) {
				EE.E(" Set killing ");
				zpln.Image(j >> 1);
				EE.Enl(); }
			for(int i2 = 0; i2 < n; i2++) {
				BFTAG wbf;
				wbf.Set(j);
				CANDGO cg1;
				cg1.GoOne(tcd[i2] << 1, wbf); 
				if(cg1.cumsteps[cg1.npas].Off(j)) {
					tot_length = 0;
					break;
				}// process stopped
				tot_length += cg1.GoBack(j, 0);
			}
			if(!tot_length)
				continue;
			int ratch = tchain.GetRating(tot_length, j >> 1);
			if(ratch) {
				// chain is accepted load it (more comments in test mode)
				// in test mode  give the details for the chains
				// in that case, do it again and print
				if(Op.ot) {
					for(int i2 = 0; i2 < n; i2++) {
						BFTAG wbf;
						wbf.Set(j);
						CANDGO cg1;
						cg1.GoOne(tcd[i2] << 1, wbf); 
						cg1.GoBack(j, 1);
					}
				}
				tchain.LoadChain(ratch, "chain", j >> 1);
			}
		} // end  for j
	}// end for ie
}

//former _10g_cf_cycle_chain.cpp follows

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

void TZCF::Aic_Cycle(int opx) {  // only nice loops and solve them
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
			int ratch = tchain.GetRatingBase((opx == 3) ? 70 : 66, npasch + 1, i >> 1);
			if(ratch) { // chain is accepted load it (more comments in test mode)
				if(Op.ot) {
					ExplainPath(wch, i, i^1, npasch + 2, i ^ 1);
				}
				tchain.LoadChain(ratch, "chain", i >> 1);	
			}
		}
		//--------------------------- now look for a loop 
		BFTAG w = h.dp.t[i] & xb; 
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
						ZPTLN candt1 = zpln.zp[t1 >> 1], candt2 = zpln.zp[t2 >> 1];
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
					int rat = tchain.GetRatingBase((opx == 3) ? 70 : 65, npascycle + 1, i >> 1);
					if(!rat)
						continue;// chain is accepted load it (more comments in test mode)
					if(Op.ot) {
						ExplainPath(wstart, t1, t1, npascycle + 2, t2);
					}
					tchain.LoadChain(rat, "cycle", i >> 1); 
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
void TZCF::Aic_Ycycle(USHORT t1, USHORT t2, BFTAG & loop, USHORT cand) {
	// forget if not same digit or t2 is ~t1
	ZPTLN candt1 = zpln.zp[t1 >> 1], candt2 = zpln.zp[t2 >> 1];
	if(cand == (t2 >> 1))
		return;
	if(candt1.ch-candt2.ch)
		return;
	USHORT ct1 = (h.dp.t[t1] & loop).Count(), ct2 = (h.dp.t[t2] & loop).Count();

	if(ct1 < 1 || ct2 < 1)
		return; // should never be

	if(ct1 < 2) {
		Aic_YcycleD(t1, t2 ^ 1, loop, cand);
		return;
	}  
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
void TZCF::Aic_YcycleD(USHORT t1,USHORT t2,BFTAG & loop,USHORT cand) { // up to 4 starts
	USHORT tt[20], itt, lg = 200;
	PATH resf, resw;
	USHORT tagc = cand << 1, tagcn = tagc ^ 1;
	(h.dp.t[t1] & loop).String(tt, itt); // starts in table
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
	int rat = tchain.GetRatingBase(65, lg, cand);
	if(!rat)
		return;
	// chain is accepted load it and in test mode, find more comments
	if(Op.ot) {
		EE.Enl("Y cycle out of the region");
		resf.PrintPathTags();
	}
	//  ExplainPath(resf,t1,t1,lg+2,t2);  }
	tchain.LoadChain(rat, "Y cycle", cand); 
}

/* first process failed
   try starttin from the second candidate in the cell containing t1
*/

void  TZCF::Aic_YcycleD2(USHORT t1x,USHORT t2x,BFTAG & loop,USHORT cand)// up to 4 starts
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
 (h.dp.t[t1] & loop).String(tt,itt); // starts in table
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
 int rat=tchain.GetRatingBase(65,lg,cand);
 if(!rat) return;
     // chain is accepted load it and in test mode, find more comments
 if(Op.ot){EE.Enl("Y cycle out of the region");
           resf.PrintPathTags();   }
	     //  ExplainPath(resf,t1,t1,lg+2,t2);  }
 tchain.LoadChain(rat,"Y cycle",cand); 
}

/* process one start  t1->t1a looking for t1
   and get back the count
   go first to t2
   then track back the path to clean the "done" filtering the process
   and continue to t1. send back the count and the BFTAG
*/
int TZCF::Aic_Ycycle_start(USHORT t1,USHORT t1a,USHORT t2,BFTAG & loop,PATH & path) {
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
void TZCF::ExplainPath(BFTAG & forward, int start, int end, int npas, USHORT relay) {
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
int TZCF::Fast_Aic_Chain() {
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





void ZCXB::GetSpace(USHORT *(& ps),int n) {
	ps = &zs[izs];
	izs += n;
	if(izs >= zcxb_lim) {
		ps=0;
		parentpuz->Elimite("ZCXB");
	}
}

void ZCHOIX::Image() const {  // liste of candidates in the set
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

int ZCHOIX::Prepare (USHORT * mi,USHORT nmi,CHOIX_TYPE ty,USHORT ixe) {
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




void TZCHOIX::Init() {
	izc = 1;
	zcxb.Init();
	nmmax = 0;
	nmmin = 10;
	direct = 0;
}

void TZCHOIX::Image() {
	EE.E("\nimage fichier choix izc=");
	EE.Enl(izc);
	for(int i = 1; i < izc; i++) {
		zc[i].Image();
		EE.Enl();
	}
}


int TZCHOIX::ChargeSet (USHORT * mi,USHORT nmi,CHOIX_TYPE ty)
{if(nmi<2||puz.stop_rating) return 0;
 if(ty &&  nmi>(chx_max+1) ) return 0;
 if(!zc[0].Prepare(mi,nmi,ty,izc)) return 0;
if(izc<zcx_lim) {zc[izc++]=zc[0];  
                 if(nmi>nmmax)nmmax=nmi;  
				 if(nmi<nmmin)nmmin=nmi; return 1;}
parentpuz->Elimite("ZCX");return 0;}

int TZCHOIX::CopySet (int i)
{if(izc<zcx_lim) {zc[izc++]=zc[i];  return 1;}
parentpuz->Elimite("ZCX");return 0;}

 // multi chains version
 int TZCHOIX::Interdit_Base80() 
 {t= zcf.h.d.t;
 int ir=0;
  for (int ie=1;ie<izc;ie++)     // all active sets 
   {if(zc[ie].type-CH_base) continue;
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
           int ratch=tchain.GetRatingBase(80,tot_length,jj>>1);
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
               tchain.LoadChain(ratch,"chain",jj>>1);	
               }
 	      } // end  for j
       } // end if
  }// end ie
 return ir;}
 
void TZCHOIX::Derive(int min,int max,int maxs)  
{if(max>nmmax) max=nmmax;   if(min<nmmin) min=nmmin;if(maxs>nmmax) maxs=nmmax;
 int maxt=(max>maxs)?max:maxs;

 if(Op.ot && 0)
  {EE.E("debut Derive min= ");EE.E(min);EE.E("  max= ");EE.E(max);;EE.E("  maxs= ");EE.E(maxs);
   EE.E("  zcf.ic= ");EE.Enl(zcf.ic); }  

 if(direct) {t= zcf.h.dp.t; allparents.AllParents(zcf.h.dp);}
  else {t= zcf.h.d.t; allparents.AllParents(zcf.h.d);}// usually direct=0
  for (int ie=1;ie<izc;ie++)
   {int nnm=zc[ie].ncd;   
    switch (zc[ie].type)
	{case CH_base: if(nnm<=max) DeriveBase(zc[ie]); break;
	 case CH_set : if(nnm<=maxs)DeriveSet(zc[ie]); break;	
	 }   
   }
if(Op.ot && 0)  {EE.E("end Derive  zcf.ic= ");EE.Enl(zcf.ic); }  
}
void TZCHOIX::DeriveBase(ZCHOIX & chx) // each candidate can be the target
{if(0){EE.E("on traite"); chx.Image(); EE.Enl();     }

 USHORT * tcd=chx.tcd, nni=chx.ncd ; 
 BFTAG tcf2,tcf2f,tcft,bfset;   tcf2.SetAll_1(); tcft=tcf2;
 // bfset is the set itself in bf form for the exclusion of the set itself
 for(int i=0;i<nni;i++) bfset.Set(tcd[i]<<1);

 for(int i=0;i<nni;i++)    tce[i]=allparents.t[(tcd[i]<<1)^1]-bfset;  
 for(int i=0;i<nni;i++)
      {tcf2f=tcf2;  if(i<nni)for(int k=i+1;k<nni;k++) tcf2f=tcf2f&tce[k];
       if(tcf2f.IsNotEmpty())
         {for(USHORT j=2;j< puz.col;j++)  if(tcf2f.On(j)) 
           { if( zcf.IsStart(j,tcd[i]<<1) )continue; // skip if defined		    
		  	 zcf.LoadDerivedTag(j,tcd[i]);
	       }// end j
         } //end if
       tcf2=tcf2&tce[i];if(tcf2.IsEmpty()) return;
	 }// end i
}

/* deriving a set creating an event
   the set must be in a dead state for a candidate in left to right mode
   then, the event is established and the event process is called
*/
void TZCHOIX::DeriveSet(ZCHOIX & chx) // only the "event" can be the target
{USHORT * tcd=chx.tcd, nni=chx.ncd-1 ; 
 BFTAG tcft,bfset;   tcft.SetAll_1();
 // bfset is the set itself in bf form for the exclusion of the set itself
 for(int i=0;i<nni;i++)    bfset.Set(tcd[i]<<1);
 for(int i=0;i<nni;i++)    tce[i]=allparents.t[(tcd[i]<<1)^1]-bfset;  
 for(int i=0;i<nni;i++)    tcft&=tce[i];
 if(tcft.IsNotEmpty()) // event established for any 'true' in tcft
	 {for(USHORT j=2;j< puz.col;j++)  if(tcft.On(j))
           {if(   tevent.EventSeenFor(j,tcd[nni])) // just for diag
			{EE.E("diag choix");chx.Image();}
		  }// end j
	}// end if

}

//former _11n_tchoix_check_go_nested.cpp follows

/* called to prepare the nested step for candidate cand
   return code is 0 if no new strong link 
                  1 if at least one strond link found
   create the new sets and the new strong links.
*/

int TZCHOIX::CheckGoNested1(const BFTAG &bftag, USHORT cand) {
	int ir=0;
 for(int ie=1;ie<izc_one;ie++)// check all sets
  {ZCHOIX chx=zc[ie];
   USHORT nni=chx.ncd,n=0,toff[10]; 
   if (zc[ie].type-CH_base) continue;
    // forget all sets where one candidate is now true
	// Gen a strong link if reduced to 2 candidates
	// should never be 0 candidate true
	// gen the reduced set if more than 2
   for(int i=0;i<nni;i++)
      {USHORT ti=chx.tcd[i]<<1; // candidate in tag form
       if(bftag.On(ti)) {n=1; break; }// set assigned
	   if(bftag.Off(ti^1)) toff[n++]=chx.tcd[i];;
      }
     if(n<2) continue;
	 if(n==2) // create a new strong link
	     {zcf.LoadBivalue(toff[0],toff[1]); ir=1; }
	 else // create a new set 
	     {ChargeSet(toff,n,CH_base);}
			// CopySet(ie);} ???(copy the previous one)
	}   
return ir;}
