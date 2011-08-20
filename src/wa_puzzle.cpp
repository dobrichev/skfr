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

//*** à mettre dans .cpp
GG::GG()		// constructor
{
	pg=g[0]; 
	pg[81]=0;
}

int GG::NBlancs()
{
	int i,n=0; 
	for(i=0;i<81;i++) 
		if(pg[i]=='0')n++;   
	return n;
}  

int GG::Nfix()
{
	int i,n=0; 
	for(i=0;i<81;i++) 
		if(pg[i]-'0')n++;   
	return n;
}

void GG::Image(char * lib)
{
	EE.E(lib); 
	EE.Enl(); 
	char wc[10];
	for(int i=0;i<9;i++)
	{
		strncpy_s(wc,10,g[i],9);wc[9]=0;
		EE.E(i+1);EE.E("=");EE.E(wc);EE.Enl();
	}
}




int P81F::BugParite(int ch)
{if(aztob.tchbit.el[el].eld[ch].n>2 ||
	aztob.tchbit.el[pl+9].eld[ch].n>2||
	aztob.tchbit.el[eb+18].eld[ch].n>2) return 1; ;
return 0;}

// DIVF implementation
DIVF::DIVF()   // constructor making initialisations
{ 
	for(int r=0;r<9;r++) 
		for(int c=0;c<9;c++)
		{	// loop on cells
			int p=I81::Pos(r,c); // cell index (0-80)
			el81[r][c]=p;		// cell is in row r
			el81[c+9][r]=p;		// cell is in column c
			int eb=I81::Boite(r,c),pb=I81::PosBoite(r,c); 
			el81[eb+18][pb]=p;	// cell is in box eb and position in box pb
		}
		for(int i=0;i<27;i++)	// convert array to bitfield
		{	BF81 z;   
			for(int j=0;j<9;j++) 
				z.Set(el81[i][j]);  
			elz81[i]=z;
		}  
};

int DIVF::IsObjetI (BF81 &ze,int i)
{return (ze.EstDans(elz81[i])); 
};

int DIVF::IsObjet(BF81 &ze)
{
	for(int i=0;i<27;i++) 
		if(IsObjetI(ze,i)) 
			return 1;  
	return 0;
};  
	
int DIVF::IsBox(BF81 &ze) 
{
	for (int i=18;i<27;i++) 
        if (IsObjetI (ze,i))
			return i; 
	return 0;
}; 
  
int DIVF::IsObjet(USHORT p1,USHORT p2) 
{
	BF81 z(p1,p2); 
	return IsObjet(z);
}

int DIVF::IsAutreObjet(BF81 &ze,int obje, int &objs)
{
	for(int i=0;i<27;i++)
    {
		if(i==obje) continue; 
		if(IsObjetI(ze,i)) 
		{
			objs=i;return 1; 
		}
	}
	return 0;
}
 
int DIVF::N_Fixes(char * pg,int el)
{
	int n=0; 
	for(int i=0;i<9;i++) 
		if(pg[el81[el][i]]-'0') 
			n++;
	return n;
}


 int P81::Change (int ch)
{if(v.cand.Off(ch)) return 0;  if(jdk.CheckChange(f->i8,ch))return 0;
 v.cand.Clear(ch); v.ncand=v.cand.CountEtString(scand);
 jdk.c[ch].Clear(f->i8);return 1;}

 

void TP81::init()
{for(int i=0;i<81;i++) 
      {t81[i].v.Init();
       t81[i].f=&t81f[i]; } 	
}
void TP81::Fixer(int ch,int i8,UCHAR typ)
  {t81[i8].Fixer(typ,ch);jdk.cFixer(ch,i8);};

int TP81::Clear(BF81 &z,int ch  )
{//EE.E("clear tp81 ");EE.E(ch+1);z.ImagePoints();  EE.Enl();
int ir=0; for(int i=0;i<81;i++)if( z.On(i))  ir+= t81[i].Change(ch);
return ir;}
//<<<<<<<<<<<<<<<<<<<<
int TP81::Clear(BF81 &z,BF16 x  )
{ int ir = 0; for(int j=0;j<9;j++)   if( x.On(j)) ir+=Clear(z,j);
return ir;}
//<<<<<<<<<<<<<<<<<<<<    specific ot UR/UL filter to find the lowest length
int TP81::CheckClear(BF81 &z,BF16 x  )
{ for(int i=0;i<81;i++)  if(z.On(i))    if((t81[i].v.cand&x).f )  return 1;
return 0;}// positive as soon as one effect found

//<<<<<<<<<<<<<<<<<<<<<<<<<
void TP81::Actifs(BF81 & z)
{ z.SetAll_0(); for(int i=0;i<81;i++) if(!t81[i].v.typ)z.Set(i);}
//<<<<<<<<<<<<<<<<<
BF16   TP81::GenCand(BF81 & z)
{BF16 w;w.f=0; for(int i=0;i<81;i++) if( z.On(i)&&(!t81[i].v.typ)) w=w|t81[i].v.cand;
return w;}
//<<<<<<<<<<<<<<<<<     y compris assigned pour RIs
BF16   TP81::GenCandTyp01(BF81 & z)
{BF16 w;w.f=0; for(int i=0;i<81;i++) if( z.On(i)&&t81[i].v.typ<2) w=w|t81[i].v.cand;
return w;}
/*
//<<<<<<<<<<<<<<<<
void 	 TP81::GenzCand(BF81 & z1,BF81 & z2,int ic)
{z2.Init();  for(int i=0;i<81;i++)
if(z1.On(i)&&(!t81[i].v.typ)&&t81[i].v.cand.On(ic)) z2.Set(i);  }


*/
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void TP81::CandidatsT()
{if(!Op.ot) return; 
 int i,j,l,lcol[9],tcol=0; char * pw;       //lcol  largeur maxi colonne
 EE.Enl("PM map ");  
 for(i=0;i<9;i++)  // attention ici i indice colonne
  {lcol[i]=2;    // 2  mini tous chiffres imposés
   for(j=0;j<9;j++)   {l=strlen( t81[9*j+i].strcol()); 
			     if(l>lcol[i])lcol[i]=l;}
        tcol+=lcol[i];}
 EE.Enl();
 for(i=0;i<9;i++) {if((i==3) ||(i==6)) EE.E("|");
			  EE.E((char)('A'+i));EE.E(Blancs(lcol[i],1));  } 
  EE.Enl();
 for(i=0;i<9;i++)  // maintenant indice ligne
  { if((i==3) ||(i==6))
     {for(int ix=0;ix<(tcol+10);ix++) EE.E((char)'-');   EE.Enl(); }
      for(j=0;j<9;j++) 
	    { if((j==3) ||(j==6)) EE.E("|");
		  P81* pp8= &t81[9*i+j]; pw=pp8->strcol();		  
	      EE.E(pw);   EE.E(Blancs(lcol[j]+1- strlen(pw),1));
	             } // end for j
     EE.Enl();  } // end for i
  EE.Enl("\n\n"); }




void ZTOB::Genere()
{ int i,j; for(i=0;i<81;i++)   // on charge tpobit
 {P81F w=t81f[i]; UNP x=T81t[i].v;
  tpobit.el[w.el].eld[w.pl].Genpo (x);
  tpobit.el[w.pl+9].eld[w.el].Genpo (x);
  tpobit.el[w.eb+18].eld[w.pb].Genpo (x);}
  // on génère tch a partir de tpo
 for(i=0;i<27;i++)  // les 27 groupes
  {for(j =0;j<9;j++) tchbit.el[i].eld[j].Raz();
	for(j =0;j<9;j++) for(int k =0;k<9;k++)
	  if(tpobit.el[i].eld[j].b.On(k)) tchbit.el[i].eld[k].Set(j);   }

 }


