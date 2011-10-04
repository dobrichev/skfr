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
JDK::JDK()
{solution=un_jeu.ggf.pg;  
 T81=&tp8N;T81C=&tp8N_cop;T81t=T81->t81;T81tc=T81C->t81;}


void JDK::Copie_T_c()
{tp8N_cop=tp8N;for(int i=0;i<9;i++) c_cop[i]=c[i];}

void JDK::Actifs()
{zactif.SetAll_0();T81->Actifs(zactif);
for(int i=0;i<27;i++)elza81[i]=divf.elz81[i]&zactif;}

void  JDK::cInit(int un)
{for (int i=0;i<9;i++) if(un)c[i].SetAll_1(); else c[i].SetAll_0();}

void JDK::cFixer(int ich,int i8)
 {for (int i=0;i<9;i++) c[i].Clear(i8); // pas de candidat ici
  c[ich].Clear(t81f[i8].z); }   // ni en zone influence

void JDK::cReport()     // on charge cand de ztzch
 {for(int i8=0;i8<81;i8++)
  {P81 *p8=&tp8N.t81[i8]; if (p8->v.typ) continue;
	p8->v.ncand=0; p8->v.cand.f=0;   p8->scand[0]=0;
	for (int i=0;i<9;i++) 	 if(c[i].On(i8)) p8->v.cand.Set(i);
	p8->v.ncand=p8->v.cand.CountEtString(p8->scand);
 }}

void JDK::TReport()     // on charge c de table en mode depart impose
 {for(int i=0;i<9;i++) c[i].SetAll_0();
 for(int i8=0;i8<81;i8++)
  {P81 *p8=&tp8N.t81[i8]; if (p8->v.typ) continue;
   for (int i=0;i<9;i++)if(p8->v.cand.On(i)) c[i].Set(i8);
   p8->v.ncand=p8->v.cand.CountEtString(p8->scand);
  }}


int JDK::Recale() 
{ //cReport();
 nfix=0;
 for(int i=0; i<81;i++)  {fix[i]='0'; if(T81t[i].v.ncand==0) return 0;}
  aztob.Genere();   
  for(int i=0; i<27;i++) for (int j=0;j<9;j++)
     if(aztob.tchbit.el[i].eld[j].n==0)   return 0;
return 1;}

int JDK::Directs() //en tete appliquer regle de base
{ int ir=0, i;
  for(i=0;i<81;i++)if((!T81t[i].v.typ)&&(T81t[i].v.ncand==1) )   // case 1 candidat
	  {FixerAdd(i,T81t[i].scand[0],3); ir=1;}
  for(i=0; i<27;i++)for(int j=0; j<9;j++)   // chiffre une place
  if(aztob.tchbit.el[i].eld[j].n==1)
  {int k=aztob.tchbit.el[i].eld[j].b.First(),i8=divf.el81[i][k];
    if(!T81t[i8].v.typ)	{ FixerAdd(i8,(char)('1'+j),i/9); ir=1;}}
return ir;}

// doing assignments matching expected rating if any
//    LastCell=10,		 last cell in row column box
//    SingleBlock=12,    single box
//    Single_R_C=15,     single row column
//    NakedSingle=23,	 cell one candidate 

int   JDK::FaitDirects(int rating)
{if(aigstop) return 1; 
 int ir=0;
 for(int i=0;i<81;i++)
   { char c1= fix[i], c2= fixt[i];
     if( c1-'0')     // donc fixée
      { // filter on rating expected
		 int ok=0; P81F p=t81f[i];
		 switch(rating)
		 {case 10: if(    (divf.N_Fixes(gg.pg,p.el)==8) 
		               || (divf.N_Fixes(gg.pg,p.pl+9)==8)
		               || (divf.N_Fixes(gg.pg,p.eb+18)==8) ) ok=1;
		           break;
		  case 12: if(aztob.tchbit.el[p.eb+18].eld[c1-'1'].n==1) ok=10;
			       break;
		  case 15: if(aztob.tchbit.el[p.el].eld[c1-'1'].n==1) ok=1;
			       if(aztob.tchbit.el[p.pl+9].eld[c1-'1'].n==1) ok=1;
			       break;
		  case 23: if( (gg.pg[i]=='0') && (T81t[i].v.ncand==1) ) ok=1;
			       break;
		 }
		 if(ok) ir+=FaitGo(i,c1,c2); 
	   }
      }
  EE.Enl();;  
  if(ir)cReport(); return ir;}


int JDK::FaitGo(int i,char c1,char c2) // function also called if single forced
{EE.E(++Op.assigned);
 EE.E(" ");EE.E(t81f[i].pt);EE.E("="); EE.E(c1);EE.E(" ");
 if(c2<4)EE.Enl(orig1[c2]);   else EE.Enl(" assigned");
 if((un_jeu.ggf.pg[i]-c1) && (!aigstop)) // validite  fixation
     {aigstop=1;  EE.E( "FIXATION INVALIDE"); return 0;}
 T81->Fixer(c1-'1',i,1);gg.pg[i]=c1;
 return 1;}

//----                     supprimer ch en elem sauf  pos
int JDK::ChangeSauf(int elem,BF16 pos,BF16 chiffres )
{int ir=0; for(int i=0;i<9;i++)
  {if(pos.On(i)) continue;  ir+=T81t[divf.el81[elem][i]].Change(chiffres) ; }
return ir; }

//----                     garder  ch en elem   pos
int JDK::Keep(int elem,BF16 pos,BF16 chiffres )
{int ir=0; for(int i=0;i<9;i++)
  {if(pos.On(i)) ir+=T81t[divf.el81[elem][i]].Keep(chiffres) ; }
return ir; }
/*
//------                  on a un ou sature objet
int JDK::Keep(int ch1,BF81 zk)
{int ir=0; // on cherche les objets contenant
 for(int il=0;il<27;il++) if(divf.IsObjetI(zk,il))
   {BF81 ze=divf.elz81[il]^zk; ze=ze&c[ch1];
    if(ze.IsNotEmpty()){ir+= T81->Clear(ze,ch1);} }
return ir;}
*/
//------                  ou simple deux points quelconques
int JDK::Keep(int ch1,USHORT p1,USHORT p2)
{BF81 ze=t81f[p1].z & t81f[p2].z & c[ch1];
if(ze.IsNotEmpty())return T81->Clear(ze,ch1);return 0;}


//---------
int JDK::NonFixesEl(int el)
{int n=0; for(int i=0;i<9;i++) if(gg.pg[divf.el81[el][i]]=='0')n++;
return n;}

//--------       verify that the puzzle is correct

int JDK::Check()
{ 
	BF16 c[27];	// a 9 bitfield for each house
	int i; for(i=0;i<27;i++) c[i].f=0;
	for(i=0;i<81;i++) 
	{
		int w=gg.pg[i]; 

		P81F w8=t81f[i]; // TO OPTIMIZE use a pointer to avoid copy of P81F
	    if((w<'1') || (w>'9')) continue;	// empty cell
		w-='1';
	    if(c[w8.el].On(w)) return 0;	// row
		c[w8.el].Set(w);
	    if(c[w8.pl+9].On(w)) return 0;	// column
		c[w8.pl+9].Set(w);
	    if(c[w8.eb+18].On(w)) return 0; // box
		c[w8.eb+18].Set(w);
	}
	return 1;
}

int JDK::CheckChange(int i, int ch)
{if(aigstop) return 1; if(solution[i]-(ch+'1'))  return 0;
aigstop=1; EE.E( "ELIMINATION INVALIDE ");EE.E(ch+1);EE.Enl(t81f[i].pt);
//T81->Candidats();zl.Image();zpln.ListeMarques();
//tac.Liste(); zcx.ListeLiens();
return 1;}


 void JDK::PointK()
 { couprem++; EE.E( "CREM=" );EE.E(couprem );}
 void JDK::UsePK(USHORT i)
 { EE.E(" UREM=");EE.E(i);EE.Enl(); if(i<=couprem )  pointK.Set(i);}

/*
void JDK::Influence (BF81 &zd,BF81 zo)
{for (int i=0;i<81;i++)if(zo.On(i)) zd=zd&t81f[i].z;}
//-------------
BF81 JDK::Points(int el,BF16 po)
{BF81 w; w.Init();
for(int i=0;i<9;i++)if(po.On(i)) w.Set(divf.el81[el][i]);  return w;}
    
*/

