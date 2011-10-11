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


//*** should not be here to be thread safe<<<<<<<<<<<<<<<<<


int CELL_FIX::BugParite(int ch) {
	if(aztob.tchbit.el[el].eld[ch].n > 2 ||
		aztob.tchbit.el[pl + 9].eld[ch].n > 2 ||
		aztob.tchbit.el[eb + 18].eld[ch].n > 2)
		return 1;
	return 0;
}

// DIVF implementation
DIVF::DIVF() {  // constructor making initialisations
	for(int r = 0; r < 9; r++) {
		for(int c = 0; c < 9; c++) {	// loop on cells
			int p = I81::Pos(r, c); // cell index (0-80)
			el81[r][c] = p;		// cell is in row r
			el81[c + 9][r] = p;		// cell is in column c
			int eb = I81::Boite(r, c), pb = I81::PosBoite(r, c); 
			el81[eb + 18][pb] = p;	// cell is in box eb and position in box pb
		}
	}
	for(int i = 0; i < 27; i++) {	// convert array to bitfield
		BF81 z;   
		for(int j = 0; j < 9; j++) 
			z.Set(el81[i][j]);  
		elz81[i] = z;
	}
}

int DIVF::IsObjetI(BF81 const &ze, int i) {
	return (ze.EstDans(elz81[i]));
}

int DIVF::IsObjet(BF81 &ze) {
	for(int i = 0; i < 27; i++)
		if(IsObjetI(ze,i)) 
			return 1;  
	return 0;
}
	
int DIVF::IsBox(BF81 &ze) {
	for(int i = 18; i < 27; i++) 
		if(IsObjetI(ze, i))
			return i; 
	return 0;
} 

int DIVF::IsObjet(USHORT p1, USHORT p2) {
	BF81 z(p1, p2);
	return IsObjet(z);
}

int DIVF::IsAutreObjet(BF81 &ze, int obje, int &objs) {
	for(int i = 0; i < 27; i++) {
		if(i == obje)
			continue;
		if(IsObjetI(ze, i)) {
			objs=i;
			return 1; 
		}
	}
	return 0;
}
 
int DIVF::N_Fixes(char * pg,int el) {
	int n = 0; 
	for(int i = 0; i < 9; i++) 
		if(pg[el81[el][i]] - '0') 
			n++;
	return n;
}

