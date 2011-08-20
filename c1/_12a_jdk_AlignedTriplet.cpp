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

//! Search for aligned pair or aligned triplet
/**
	That proces is very close to SE's one.
	Find all cells with 2 or 3 candidates and 
	collect all cells seen by one of them (potential base cells)
	First choose 2 cells (in the same band)of the potential base cells.
*/



int JDK::AlignedTripletN() 
{
	int debuga=0;
	static int combi32[3][2]={{0,1},{0,2},{1,2}};
	BF81 z23,	// set of cells that have 2 or 3 candidates (could be excluding cells)
		zbase;	// set of cells that are visible by one potential excluding cells
	for(int i=0;i<81;i++)  
	{
		UNP p=T81t[i].v; 
		if(p.ncand<2 || p.ncand>3) continue;
		z23.Set(i); 
		zbase|=t81f[i].z;
	}
	zbase &= zactif; // possible base cells reduced to cells that have no value
	// loop on any combination of 2 potential base cells
	for(int i1=0;i1<80;i1++) 
		if(zbase.On(i1))
		 // if((z23 & t81f[i1].z).Count()>=3) // optimisation non concluante 
		 // il faudrait optimiser Count (qui balaye le tableau de bits !)
			for(int i2=i1+1;i2<81;i2++)
				if(zbase.On(i2))
				// if((z23 & t81f[i2].z).Count()>=3) // optimisation non concluante 
				// il faudrait optimiser Count
	{ 
		
		BF81 basei(i1,i2), // first 2 base cells
          *zi1=&t81f[i1].z, 
          *zi2=&t81f[i2].z,
		   zi1andi2=(*zi1) & (*zi2);
		if(zi1andi2.Count()<2) continue; // must be same band
		// we need to add an other base cell + 2 excludig cells that are visible
		// by the 3 base cells => the 3 base cells must share a same band
		// this condition is not used by serate
		BF81  zi1ori2=((*zi1) | (*zi2)) - basei; // excluding already there
		BF81  z23ori1i2=z23 & zi1ori2; // seen by at least one
		// z23ori1i2 is the "twinarea of serate"
		if(z23ori1i2.IsEmpty()) continue; // need a third cell as base cell
		
		for(int i3=0;i3<81;i3++) if(z23ori1i2.On(i3)) // try all possible third cell
		{
			if (i1==39 && i2==53 && i3==40) 
				debuga=1; 
			BF81 basei3=basei; basei3.Set(i3);	// set of 3 base cells
			BF81 z23f=(zi1andi2 & t81f[i3].z & z23) - basei3; // TODO verify but ' - basei3' is de trop
			if(z23f.Count()<2) continue; // we need at least 2 potential excluding cells

			// TODO modify 

			// loop on permutation of potential candidate for the 3 base cells
			int iCell[3]={i1,i2,i3};		// index of base cells 0-80
			int ch[3];				// candidate for the 3 base cells 8-0
			BF16 cands[3]= { T81t[i1].v.cand,T81t[i2].v.cand, T81t[i3].v.cand};	// set of candidates
			BF16 allowed[3];				
			for (int k1=0;k1<3;k1++) allowed[k1].SetAll_0();
			// test all combination of candidates
			for (ch[0] = 0;ch[0]<9;ch[0]++) if (cands[0].On(ch[0]))
				for (ch[1] = 0;ch[1]<9;ch[1]++) if (cands[1].On(ch[1]))
					for (ch[2] = 0;ch[2]<9;ch[2]++) if (cands[2].On(ch[2]))
			{
				bool allow = true;
				// verify if this combination is forbidden 
				// 2 base cells that are visible may not have the same candidate
				for (int combiIndex = 0;combiIndex<3;combiIndex++)
				{
					int ch1=ch[combi32[combiIndex][0]];
					int ch2=ch[combi32[combiIndex][1]];
					if (ch1==ch2)
					{
						int ic1=iCell[combi32[combiIndex][0]];
						int ic2=iCell[combi32[combiIndex][1]];
						if (T81t[ic1].f->ObjCommun(T81t[ic2].f))
						{
							allow = false;
							break;
						}
					}
							
				}
				if (!allow) continue;
				// verify if the combination is forbidden by an excluding cell
				// the potential candidate clear all possibliites for the exlcuding cell
				// loop on potential excluding cell
				for (int iec=0;iec<81;iec++) if (z23f.On(iec))
				{
					BF16 cec = T81t[iec].v.cand;
					for (int ich=0;ich<3;ich++) cec.Clear(ch[ich]);
					if (cec.isEmpty())
					{
						allow = false;
						break;
					}
				}
				// combination allowed => 3 candidates are allowed
				if (!allow) continue;
				for (int cellIndex = 0;cellIndex<3;cellIndex++)
					allowed[cellIndex].Set(ch[cellIndex]);
					} // end ch 1,2 & 3
			// verify for each potential candidates if there are no allowed possibility
			for (int ice2 = 0;ice2<3;ice2++) // loop on base cells
				if (!(cands[ice2]==allowed[ice2])) // optimisation
				for (int icand=0;icand<9;icand++)if (cands[ice2].On(icand)) // loop on candidates
			{
				if (allowed[ice2].Off(icand))
				{
					// we have found one "aligned triplet exclusion"
					T81t[iCell[ice2]].Change(icand); 
					if(Op.ot)
					{
						EE.E(" aligned triplet exclusion for ");
						EE.E(icand+1);EE.E(t81f[iCell[ice2]].pt);
						EE.E(" using base cells ");
						EE.E(t81f[i1].pt);EE.E(" , ");
						EE.E(t81f[i2].pt);EE.E(" and ");
						EE.E(t81f[i3].pt);EE.E(" ; ");EE.Enl();
						EE.E(" using excluding cells ...");
						// TODO add set of excluding cells
						EE.Enl();
					}
					return 1;
				} // end if(allowed[ice2].Off(icand))
			} // end  ice2 & icand
		} // end for i3
	} // end for i1 & i2
	debuga=1;
	return 0;
}
				

//similar process for aligned pair with 2 base cells only

//! Search for aligned pairs
int JDK::AlignedPairN() 
{
	BF81 z2,zbase;
	// look for cells that have 2 candidates
	for(int i81=0;i81<81;i81++)  
	{
		UNP p=T81t[i81].v;	// candidates of the cell
		if(p.ncand-2) continue; // only cell with 2 candidates
		z2.Set(i81);			// put cell to set
		zbase|=t81f[i81].z;	// or of cells controled by these cells
	}
	zbase &= zactif;		// keep only cells that have not yet a value
	for(int i1=0;i1<80;i1++) 
		if(zbase.On(i1))
			for(int i2=i1+1;i2<81;i2++)
				if(zbase.On(i2))
				{			// a couple of cells  that see at minimum one bivalue cell
					BF81 
						basei(i1,i2),
						*zi1=&t81f[i1].z, // influence zone of first cell
						*zi2=&t81f[i2].z, // influence zone of second cell
						z2f=(*zi1) & (*zi2) & z2; // set of cell with 2 cand in both influence zones
					if(z2f.Count()<2) continue;  
					// z2f is the set of excluding cells of serate
					// test combination of candidates for cell i1 and i2
					int ch1 = 0;					// candidate for cell i1
					BF16 cands1 = T81t[i1].v.cand;	// set of candidates
					int ch2 = 0;					// candidate for cell i2
					BF16 cands2 = T81t[i2].v.cand;	// set of candidates
					int excl1[9];	
					int excl2[9];
					for (int k=0;k<9;k++) {excl1[k]=excl2[k]=0;}
					// test all combination of candidates
					for (ch1 = 0;ch1<9;ch1++) if (cands1.On(ch1))
					{	for (ch2 = 0;ch2<9;ch2++) if (cands2.On(ch2))
						{
							// look if visible cells and same cand => excluding
							if (ch1==ch2)
							{
								if (T81t[i1].f->ObjCommun(T81t[i2].f))
								{
									excl1[ch1]++;
									excl2[ch2]++;
									continue;
								}
							}
							else 
							{
								// look if there is an exclusion cell
								for (int i = 0;i<81;i++) if (z2f.On(i))
									if (T81t[i].v.cand.On(ch1) && T81t[i].v.cand.On(ch2))
									{
										excl1[ch1]++;
										excl2[ch2]++;
										break;
									}
							}
						}
					}
					// verify if there is an excluded candidate
					int n1 = T81t[i2].v.ncand;
					int n2 = T81t[i1].v.ncand;
					int cell8=-1;
					int ch=-1;
					for (ch1 = 0;ch1<9;ch1++) if (cands1.On(ch1))
						if (excl1[ch1]==n1)
						{
							cell8 = i1;
							ch=ch1;
							break;
						}
					if (cell8<0)
						for (ch2 = 0;ch2<9;ch2++) if (cands2.On(ch2))
						if (excl2[ch2]==n2)
						{
							cell8 = i2;
							ch=ch2;
							break;
						}
					if (cell8>=0) // we have found one pair exclusion
					{
						T81t[cell8].Change(ch); // erase candidate
						if(Op.ot)
						{
							EE.E(" aligned pair exclusion for ");
							EE.E(ch+1);EE.E(t81f[cell8].pt);
							EE.E(" using base cells ");
							EE.E(t81f[i1].pt);EE.E(" and ");
							EE.E(t81f[i2].pt);EE.E(" ; ");EE.Enl();
							EE.E(" using excluding cells ");
							for (int i = 0;i<81;i++) if (z2f.On(i))
							{ EE.E(t81f[i].pt);EE.E(" ; ");}
							EE.Enl();
						}
						return 1;
					}// end  ich
					
				}
	return 0;
}

