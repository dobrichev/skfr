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

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
void TPAIRES::CreerTable()
{ip=0; ntplus=aigpmax=0; zplus.SetAll_0();
 for(int i=0;i<81;i++) 
   {int n=T81t[i].v.ncand;
	if(n==2)    {zp[ip].Charge(T81t[i]);  zpt[ip]=zp[ip++];}
    else if(n>2)
     {if(ntplus<8) {tplus[ntplus]=i;//direct access to first plus cells
	                 ntplus++;   zplus.Set(i);}
      if(n>aigpmax) aigpmax=n;}
  }
 // now sorting the table zpt for the search of UR/UL 
 for(int i=0;i<ip-1;i++) for(int j=i+1;j<ip;j++) 
	 if(zpt[j].pa.f<zpt[i].pa.f  
		||(zpt[j].pa.f==zpt[i].pa.f  && zpt[j].i8<zpt[i].i8) )
 {PAIRES temp=zpt[i]; zpt[i]=zpt[j]; zpt[j]=temp;}
 // and final entries in tp izpd izpf
 np=0; if(!ip) return; tp[0]=zpt[0].pa;izpd[0]=0;
 for(int i=1;i<ip;i++)
   {if(zpt[i].pa.f==tp[np].f)continue;
    izpd[++np]=i;tp[np]=zpt[i].pa;}
    izpd[++np]=ip;
}

//====================================
int TPAIRES::UL()
{int ir=0;tult.Init();
 for(int i=0;i<np;i++)
   {USHORT id=izpd[i],ie=izpd[i+1];
  // EE.Enl("un depart paire");
	UL_SEARCH uls(tp[i],this,&zpt[id],ie-id); //build search 
    for(int j=id;j<ie-1;j++) for(int k=j+1;k<ie;k++)
	  {USHORT i1=zpt[j].i8,i2=zpt[k].i8;
	   P81F p1=t81f[i1],p2=t81f[i2];
	   if(!(p1.el==p2.el || p1.pl==p2.pl) )continue; // start row or col
   //  EE.E(p1.pt); EE.E(p2.pt);EE.Enl("un depart lig/col");
       UL_SEARCH ulsn(&uls); ulsn.Set(i1);
	   if(p1.el==p2.el) ulsn.el_used.Set(p1.el);  
	   else ulsn.el_used.Set(p1.pl+9); 
       ir+=ulsn.Add_Chain(i2);  // looking for all equivalent moves
	}// end jk
    }// end i
return ir;}

//========================= insert a new cell after el_used correct
void UL_SEARCH::Set(int i8) // el_used already ok if necessary
{cells.Set(i8);  
 P81F p=t81f[i8];last=i8; 
 UNP pv=T81tc[i8].v;
 parity.Inv(p.el);parity.Inv(p.pl+9);parity.Inv(p.eb+18);                   
 elcpt[p.el]++;elcpt[p.pl+9]++;elcpt[p.eb+18]++;
 cht|=pv.cand; int nc=pv.cand.QC(); 
 if(nc>2) adds[nadds++]=i8;
// EE.E("UL lc=");EE.E(line_count);EE.Esp();EE.Enl(p.pt);
 tcount[line_count++]=i8;
}


//====================================
// Check if the loop has more than 0 solutions by parity
bool UL_SEARCH::ParityCheck(void)
{
	// Check for the "0 solutions" parity check
	unsigned int oddPos=0, evenPos=0;
	bool isOdd = false;
	for (int i=0;i<(line_count-1);++i) {
		isOdd = !isOdd;
		unsigned int curRow = I81::Div9(tcount[i]);
		unsigned int curCol = tcount[i]%9;
		unsigned int curBox = I81::Boite(curRow, curCol);
		int newPos = 1<<curRow | 1<< (curCol + 9) | 1 << (curBox + 18);
		if (isOdd) {
			if (oddPos & newPos) return false;
			oddPos |= newPos;
		} else {
			if (evenPos & newPos) return false;
			evenPos |= newPos;
		}
	}
	if (oddPos != evenPos) return false;
	return true;
}

//============================= recurrent process to add next point
int UL_SEARCH::Add_Chain(int i8)
{if(line_count>20) return 0; // securite
 if(cells.On(i8)) // terminé on élimine URs et fausses boucles
   {//EE.E("UL end ");EE.E(t81f[i8].pt);EE.Esp();cells.ImagePoints();EE.Enl();
	if (line_count<5 || (i8-tcount[0])) return 0;  
    tcount[line_count++]=i8;return Loop_OK();}
Set(i8);              // On met le point en place
P81F f=t81f[i8];

// a défaut une case avec additifs  ligne, puis col, puis bloc en paire
// uniquement dans éléments non traités et si pas de double paire
// not more than 3 adds except one digit
if(nadds>7 ||(cht.QC()>3 && nadds>2)) return 0; 
if(El_Suite(f.el) ) return 1;
if(El_Suite(f.pl+9) ) return 1;
if(El_Suite(f.eb+18) ) return 1;
return 0;}


//====================================
int UL_SEARCH::El_Suite(USHORT ele)
{if(el_used.On(ele)) return 0;
 //EE.E("suite el=");EE.Enl(ele+1);
 BF16 wc=aztob.tchbit.el[ele].eld[c1].b
        & aztob.tchbit.el[ele].eld[c2].b ;
 for(int i=0;i<9;i++) if (wc.On(i))// cells with both digits
	{int i8r=divf.el81[ele][i];
     //EE.E("essai i8=");EE.Enl(t81f[i8r].pt);
     if(ele>17) // in a box, only not row col
	     {P81F f=t81f[i8r],f2=t81f[last];
	      if(f.el==f2.el || f.pl==f2.pl) continue;
	     }
     if(! Is_OK_Suite(i8r)) continue;
	 UL_SEARCH ulsn(this); ulsn.el_used.Set(ele); 
     if(ulsn.Add_Chain(i8r)) return 1;
	}// end for
 return 0; } 

//=================================================
int UL_SEARCH::Is_OK_Suite(USHORT i8) 
{if(i8==last)return 0; if(i8==tcount[0]) return 1;
 if(cells.On(i8)) return 0; // false loop  
 P81F f=t81f[i8]; 
 if(elcpt[f.el]>1 ||  elcpt[f.pl+9]>1 ||  elcpt[f.eb+18]>1) return 0;
 // for the time being, I see no more exclusion
 return 1;}
//  entry action 0 is the start 4.6.  store after
//       one digit in excess and 

//==================================================
int UL_SEARCH::Loop_OK(int action)
{//UL_Mess("potential loop",1);
if (parity.f)return 0; // must be even number everywhere

if ( !ParityCheck() ) // check for more than 0 solutions
		return 0;
if(!action) // split processing depending on size of the loop
	if(line_count>7) {  tult.Store(this); return 0;}
	else action++;
 // les deux ci-dessous sortent en 4.6 et 4.7; voir l'origine de l'écart (nb de pas???)
if(action==1 && nadds<2) //one cell with adds rating 4.6 revérifié, c'est bien 4.6
    {USHORT iu=adds[0];      if(T81t[iu].Change(chd))
         {UL_Mess("one cell with extra digits ",1);
          return 1;}
     } // nothing if redundancy with another loop

 // now one digit in excess ++++++++++++
 if (action==1 && (cht.QC()==3))  
   { BF81 zi; zi.SetAll_1(); for(int i=0;i<nadds;i++) zi&=t81f[adds[i]].z;
     if(T81->Clear(zi,(cht-chd).First() ))
	      {UL_Mess(" one digit in excess",1); return 1; }         
    }


// action 1, launching the process common to UR/UL
if(nadds==2) // type 2;3;4 must be same object 
    { if(!(t81f[adds[0]].ObjCommun(&t81f[adds[1]]) )) return 0;	
      int ir=ur.StartECbi(adds[0],adds[1],chd,action);
	  if(ir==1)
         {UL_Mess("action UL / 2 cells)",1); return 1;}   

    }
	  // store it if action 0 
if(action<2)   { tult.Store(this); return 0;}

//UL_Mess("nothing",1);
 return 0;}
//-----  
void UL_SEARCH::UL_Mess(char * lib,int pr)  // et flag des "faits"
{ EE.Enl();
 EE.E("UL loop nadds=");EE.E(nadds); EE.E(" count=");EE.E(line_count-1);
 EE.E(" rating=");EE.E(Op.difficulty);
 for(int i=0;i<line_count;i++) {EE.Esp(); EE.E(t81f[tcount[i]].pt);}
 EE.Enl();
 if(pr) {EE.E("active due to " );EE.Enl(lib);   }
}
 
