/*
           <<<<<<  SEARCH_LS_FISH  >>>>>>
   class used to search locked sets and fishes

*/

 void SEARCH_LS_FISH::SetParent(PUZZLE * parent){
 parentpuz=parent;
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
		{EE.Enl("ecs assignment");
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
	EE.E("->");EE.E(gt[in]); 
	if(e<27)
	{EE.E(" cells ");	EE.E( wi.String());EE.E(" digits ");  }
	else
	{EE.E(" digits ");EE.E( wi.String());EE.E(" cells "); }
	EE.E( wf.String());EE.E(" ");EE.E(orig[it2]);EE.E(" ");EE.Enl(c2);
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
	     {EE.E(gxw[rangv-2]);	 EE.E(" digit ");     EE.E(i+1); 
	      EE.E( " columns ");   EE.E(wf.String(0));
		  EE.E(" rows ");  EE.Enl(wi.String());     
		  return 1; 	 }}
       }
	 regxw=regindchcol;
	 eld=regxw[iel].eld;
     nn=eld[i].n;
     if(nn<2 || nn>rangv) continue;
     w=eld[i].b;  	wi.f=0; wi.Set(iel);
     if( XW(w,iel,0) )
      { if(GroupeObjetsChange(0,i) ) // action lignes
	   {EE.E(gxw[rangv-2]);	 EE.E(" digit ");
	    EE.E(i+1); EE.E(" rows ");	EE.E(wf.String());
		EE.E( " columns ");	  EE.Enl(wi.String(0));     
		return 1; }  	 }
    } // end iel
  }    // end i niv
return 0;}
