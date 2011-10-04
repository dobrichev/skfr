 
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

// to see later, added by mladen
//#include "wb_BruteForce.h"
//#include "ratingengine.h"



// **** static member to send later in PUZZLE ****
ULONG UNPAS::nsol; 



void UNPAS::Clear(int i8,USHORT  ch )
{
	P81F *wf=&t81f[i8];			// t81f global variable giving influence zone of all cells
	for(int i=0;i<20;i++)
    {
		USHORT j=wf->pi[i];		// cell index in influence zone
		if(libres.On(j))
			tu[j].Clear(ch);
	}
}

int UNPAS::Avance()
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

void UNPAS::NsolPas()
{
	if(Avance())
		return;		// blocking
	if(!nlibres)	// all cells are valued : we have a solution
	{
		nsol++;		// count solutions
		if(nsol==1)  // keep first solution image
			un_jeu.ggf.Copie(gg); 
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
			UNPAS pn(this);  
			pn.Fixer(iactif,i);  
			pn.NsolPas();
			if((nsol>1)) return ;    
		}
}

