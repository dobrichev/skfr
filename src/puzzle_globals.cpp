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
            <<<<<<  BRUTE FORCE module   >>>>>>
             <<<<<<  TWO_REGIONS_INDEX   >>>>>>

*/

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
 return T81t[cw.ig].Change(cw.ch);
}

int TCHAIN::ClearImmediate(USHORT cand){ // clear candidates
 if(cycle_elims.On(cand)) return 0;
 if(Op.ot){EE->Enl("immediate elimination through chain for candidate ");
           zpln.Image(cand);
            EE->Enl();
 }
 cycle_elims.Set(cand);
 CANDIDATE cw=zpln.zp[cand];
 int ir=T81t[cw.ig].Change(cw.ch); // should always answer yes
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
	if(elims_done) return 1;
	if(!ichain) return 0;
	int ir=((rating <= x) );
	return ir;
}

/*              <<<<<<  BRUTE FORCE module   >>>>>>
            entry for Brute Force solution checking uniqueness

			not yet fully thread safe
			global variables still there  
			zpln
			T81t

*/

int BRUTE_FORCE::Uniqueness(GG &ge, GG *puz_solution) {
		USHORT nsol = 0;
		gg.Copie(ge);
		dep.Init(gg, &nsol, puz_solution);
        for(int i = 0; i < 81; i++) {
			char c = gg.pg[i];
			if(c > '0' && c <= '9')
				dep.Fixer(i, c - '1');
		}
        dep.NsolPas();
		return (nsol == 1);
	}

/*              <<<<<<  BRUTE FORCE STEP    >>>>>>
                recursive search in brute force mode

*/
//! Intialize with the puzzle
/**
	 * Copy the puzzle in memeber of the class<br>
	 * Consider all cells as free.
	 * Reset number of solution
 */

void BRUTE_FORCE_STEP::Init(const GG &ge, USHORT *nsole, GG *ggfe)
    {	nsol=nsole;
	    ggf=ggfe;
		libres.SetAll_1();
		nlibres=81;
		(*nsol)=0;
		tu[0].Init();	// consider all candidates as possible in all cell
		for(int i=1;i<81;i++)tu[i]=tu[0];
		gg.Copie(ge);    
	}

//! Give a value to a cell and supress candidates in influence zone
/**
	 * Doesn't change the status of the cell <code>typ</code>
	 * \param i cell index (0-80)
	 * \param ch digit (0-8)
 */

void BRUTE_FORCE_STEP::Fixer(int i, USHORT ch)
    {
		if(libres.Off(i))
			return;  
		libres.Clear(i); 
		nlibres--;
        tu[i].Fixer(ch); // give the value to the cell
		Clear(i, ch);
		gr[i] = ch + '1';
	}
//! Supress all candidates corresponding to digit in influence zone of the cell
	/**
	 * uses <code>t81f</code> global variable
	 * \param i8 cell index (0-80)
	 * \param ch digit (0-8)
 */

void BRUTE_FORCE_STEP::Clear(int i8,USHORT  ch )
{
	CELL_FIX *wf=&t81f[i8];			// t81f global variable giving influence zone of all cells
	for(int i=0;i<20;i++)
    {
		USHORT j=wf->pi[i];		// cell index in influence zone
		if(libres.On(j))
			tu[j].Clear(ch);
	}
}
//! Look for Singles (and do them) and blocking situation
/**
	 * Loop on {look for "single" and do them} until there is no more single. Singles include :<ul>
	 * <li> naked single (priority)</li>
	 * <li> single in house (row/col/box </li>
	 * </ul>
	 * At each loop verify that there is no blocking situation <ul>
	 * <li> a cell with no possibilities </li>
	 * <li> a house where a digit has no possible position</li></ul>
	 * \return 1 if a blocking has been found(one empty cell has no candidate
	 * or a digit has no position in a house), else 0. This return status is 
	 * independent of the number of single found.
 */

int BRUTE_FORCE_STEP::Avance()
{
	USHORT ib=1; 
	while(ib&&nlibres)
	{
		ib=0;
		// look for all naked single and verify if there is a cell with no candidate
		for (USHORT i=0;i<81;i++)
        {
			if(tu[i].ncand==0) // there is a cell with no candidates !
				return 1;		
			if( libres.On(i)&&(tu[i].ncand==1)) // naked single
			{
				// only one candidate in a cell
				ib=1;
				Fixer(i,tu[i].GetCand());
			}
		} 
		if(ib) continue;  // priority to naked single over single in house (row/col/box)

		for (USHORT ie=0; ie<27;ie++)  // loop on 27 houses (row, col, box)
		{ 
			USHORT * pel=divf.el81[ie]; // pointer to an array of cell index for this house
			BF16 or,		// accumulate digit that have >=1 position
				deux;		// accumulate digit that have >=2 positions
			for(USHORT p=0;p<9;p++)	// loop on cell of the house
			{ 
				USHORT i81=pel[p];	// cell index
				if(!libres.On(i81))
					or |= tu[i81].cand;    //force double
				BF16 w=tu[i81].cand & or; // & between 2 bitfields
				deux |=w;
				or|=tu[i81].cand;
			}
			if(or.f-0x1ff) // verify if there is a digit with no possible position
				return 1;  
			or -=deux;		// or=0x1ff before or digit that have only one position after
			if(!or.f)continue; // no single in house
			// TO OPTIMIZE : use BF16::First (if First is itself optimized)
			for(USHORT  ich=0;ich<9;ich++) // loop on digit
				
				if(or.On(ich))
					for(USHORT p=0;p<9;p++)  // loop to find the cell that have the single
						if(tu[pel[p]].cand.On(ich))
						{
							ib=1;
							Fixer(pel[p],ich);
							break;
						}
		
		} // end of loop on 27 houses
	}  //end of while
	return 0;
}

//! Brute force recursive method to find solution and to know if number of solutions is >1
	/**
	 * This method try first to find all single path. Then look for a cell 
	 * with a minimum number of candidates. Assign it and call itself recursively.
	 * Stop if blocking, if more than 1 solution, or if tries are exhausted
	 * Update static member <code>BRUTE_FORCE_STEP::nsol</code>
	 * Update global variable <code>un_jeu.ggf</code> with the first found solution if any
 */

void BRUTE_FORCE_STEP::NsolPas()
{
	if(Avance())
		return;		// blocking
	if(!nlibres)	// all cells are valued : we have a solution
	{
		(*nsol)++;		// count solutions
		if((*nsol)==1)  // keep first solution image
			ggf->Copie(gg); 
		return;  
	}
	// look for a cell with a minimal number of candidates
	USHORT max=10,iw; 
	for(int i=0;i<81;i++)  // loop on empty cells
	{ 
		if(!libres.On(i))
			continue;     
		iw=tu[i].ncand;		// number of candidates
		if(iw<2)continue;	// should not happen
		if(iw<max)
		{
			max=iw;
			iactif=i;		// memorize the cell index
			if(max<3) break;// we have found a cell with 2 candidates
		}
	}    
	
	candactif=tu[iactif].cand; // the candidates of the cell
	for(int i=0;i<9;i++) 
		if(candactif.On(i))
		{
			BRUTE_FORCE_STEP pn(*this);  
			pn.Fixer(iactif,i);  
			pn.NsolPas();
			if(((*nsol)>1)) return ;    
		}
}

	//! Get a bitfield representing the givens
	/**
	 * <b>NOTA :</b>This method is not used in the SEclone !
	 */
BF81 BRUTE_FORCE_STEP::GetZ() const {
	BF81 zw;
	for(int i = 0; i < 81; i++) 
		if(gr[i] - '0')
			zw.Set(i);
    return zw;
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