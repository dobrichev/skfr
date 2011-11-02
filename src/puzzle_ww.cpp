/*               <<<<<<  CELL  CELLS   >>>>>>

     main table for the cells of the  puzzle
	 this is the entry point for any elimination
	 keep updated situation of candidates and assigned/given

	 a copy of the table is needed in some steps as UR search


*/
int CELL::Change(int ch) {
	if(v.cand.Off(ch))
		return 0;
	if(puz.CheckChange(f->i8, ch))
		return 0;
	v.cand.Clear(ch);
	v.ncand = v.cand.CountEtString(scand);
	puz.c[ch].Clear(f->i8);
	return 1;
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
	puz.cFixer(ch, i8);
}

int CELLS::Clear(BF81 &z, int ch) {
	//EE->E("clear tCELL ");EE->E(ch+1);z.ImagePoints();  EE->Enl();
	int ir = 0;
	for(int i = 0; i < 81; i++)
		if(z.On(i))
			ir += t81[i].Change(ch);
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
void CELLS::CandidatsT() {
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

