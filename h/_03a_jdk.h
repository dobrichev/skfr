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
#pragma once
class JDK
{
public:
    GG gg;          //< copy of the puzzle (normalized form - empty cell '0')
    TP81 tp8N,
		tp8N_cop;  
	BF81 
		c[9],
		c_cop[9];	// grille globale et par chiffre
    BF81 zactif,
		elza81[27],
		csol[9];   //< Solution as positions of the 9 digits
    BFTAG pointK;
    int cycle, 
		coup,
		coupMM,
		couprem, 
		cas,
		fixes,
		ecrit,
		compte;
    char fix[81],
		fixt[81],
		*solution;
    USHORT nfix,
		nmin,
		p81min,
		erreur,
		erreurv,
		bloc,
		aigboucler;

	JDK();
    void Initial(); 
	void cInit(int un=0);
	void Copie_T_c();
    void cFixer(int ich,int i8);
    void cReport();   
    void TReport();   
    void Actifs();
    void Influence (BF81 &zd,BF81 zo);
    BF81 Points(int el,BF16 po);
    int CheckChange(int i, int ch);
    int ChangeSauf(int elem,BF16 pos,BF16 chiffres );
    int Keep(int elem,BF16 pos,BF16 chiffres );
    int Keep(int ch,BF81 zk); // éliminer ailleurs en objets contenant
    int Keep(int ch,USHORT p1,USHORT p2); // éliminer ailleurs de deux points
    int NonFixesEl(int e);
    void FixerAdd(int i,char c,int elt)
    {
		fix[i]=c;
		fixt[i]=(char)elt;
		nfix++;
	};

	//! verify  if the puzzle is correct
	/**
	 * Verify only that each house has no duplicate givens
	 * No check on the number of solutions
	 * \return 0 not correct, 1 correct
	 */
    int Check();
    int Recale();              
	int Traite();
	int Traite_a();
	int Traite_b();
	int Traite_c();
    int Directs();             
	int FaitDirects(int type);
 	int FaitGoA(int i8,char c1,char c2)
	{
		int ir=FaitGo(i8,c1,c2);
		cReport();return ir;
	}

    int TraiteLocked(int rating);  // start for locked in row,cil,box
    int TraiteLocked2(int eld,int elf); // detail for ratings 2.6  2.8
    void PKInit(){couprem=0;pointK.SetAll_0();}
    void PointK();
    void UsePK(USHORT i);
// tagging process added here in that version
    void TaggingInit();
	void GenCellBivalues();
	void GenYBivalues();
	void BiValues();
	void BiValues(USHORT ch);
    int AlignedPairN();
	int AlignedTripletN();
	void Chaining(int option,int chain_level,int base);
	int Rating_end(int next);
	int Rating_base_65();  // x or y cycles
	int Rating_base_66();  // x or y cycles
	int Rating_base_70();  // xy chains xy cycles ?
	int Rating_base_75();  // align tripl exclus  //  nishio
	int Rating_base_80();  // dynamic chains
	int Rating_base_85();  // multiple chains
	int Rating_base_90();  // dynamic plus 
	void InitNested();
	int Rating_baseNest(USHORT base,int quick);  // nesting  95 forcing chains
	int Rating_base_95_Quick();  // quick nesting   
	int Rating_baseNest(USHORT base);  // nesting  95 forcing chains
	                              // 100 multi chains 110 nesting dynamic chains
	void Rating_Nested(USHORT base,USHORT * ttags,USHORT ntags,USHORT target);
private:
	int FaitGo(int i8,char c1,char c2);
};



