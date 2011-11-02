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
