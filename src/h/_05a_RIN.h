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
// class defined to handle Unique rectangles and Unique loops
// the search for URs is started in TP81, locating potential URs
// That class is called by TP81

class CRIN  
{
public:     //on ne traite que deux communs. 
	static P81 *ta,*tr; // ta action, tr recherche 
	static OBBIEL * tchel;
	BF16 wc,wou,wr;   
	int  ia,ib,ic,id,deux[4],plus[9],pp1,pp2, // voir pourquoi plus est 9
		ndeux,nplus,nwc,ch1,ch2,chc1,chc2,nautres,diag,rating;

	// data specific to common processing UR/UL (need sub routines)
	int th[10],td[10],tnh[10],nth,ntd,nnh,aig_hid;
	BF16 wh,wd,wnh,wnd,wdp;
	BF81 zwel; // to prepare clearing of naked sets

CRIN(){ta=T81t;tr=T81tc;tchel=aztob.tchbit.el;}	 // constructor

    // short functions called by the mains ones

int GetElPlus() {return tp81f.GetLigCol(pp1,pp2);}// assuming nplus=2
int IsDiag()    {if(divf.IsObjet(deux[0],deux[1])) diag=0;
                       else diag=1; return diag;}
int Setw(){wc=tr[ia].v.cand & tr[ib].v.cand & tr[ic].v.cand & tr[id].v.cand;   
            nwc=wc.QC();  return nwc; }
int Setwou(){wou=tr[ia].v.cand | tr[ib].v.cand | tr[ic].v.cand | tr[id].v.cand;
             wr=wou^wc;      nautres=wr.QC();      return nautres; }
   
void CalDeuxd(int i) {if((tr[i].v.cand-wc).f) plus[nplus++]=i; else deux[ndeux++]=i;}
void CalcDeux()   {ndeux=nplus=0;
                   CalDeuxd(ia);CalDeuxd(ib);CalDeuxd(ic);CalDeuxd(id);
                   pp1=plus[0];pp2=plus[1];}

inline int Jum(USHORT el,USHORT ch) {if (tchel[el].eld[ch].n==2)return 1;return 0;}
int Jumeau(USHORT a,USHORT b,USHORT ch){USHORT el=tp81f.GetLigCol(a,b); return Jum(el,ch);}

int GetJJDir(USHORT a,USHORT b,USHORT c,USHORT d,int ch)
                   { int ok=Jumeau(a,b,ch); ok+=Jumeau(b,d,ch)<<1;
                     ok+=Jumeau(c,d,ch)<<2;ok+=Jumeau(a,c,ch)<<3; return ok;} 
int GetJJDir(int ch){ int ok=Jumeau(ia,ib,ch); ok+=Jumeau(ib,id,ch)<<1;
                      ok+=Jumeau(ic,id,ch)<<2;ok+=Jumeau(ia,ic,ch)<<3; return ok;}

void GenCh(){BF16 ww=wr;ch1=ww.First(); ww.Clear(ch1);ch2=ww.First();
             ww=wc;chc1=ww.First(); ww.Clear(chc1);chc2=ww.First();}

BF81 GetZ(){BF81 zw(ia,ib);zw.Set(ic);zw.Set(id);return zw;} // le BF81 des 4 points
 


void ImageRI(char * lib,USHORT a)
          { EE.E( lib);EE.E(tr[a].f->pt);EE.E(" ");EE.E( tr[a].scand);}
void ImageRI(char * lib)
          { if(!Op.ot) return;
	        EE.E( "->UR" );EE.E( lib);ImageRI(" P1=",ia) ;ImageRI(" P2=",ib) ;
            ImageRI(" P3=",ic) ;ImageRI(" P4=",id) ;EE.Enl();}


       // look for pseudo locked set in a unique loop
 int  StartECbi(USHORT p1,USHORT p2,BF16 com, int action)
          {pp1=p1;pp2=p2; wc=com;
           chc1=com.First();com.Clear(chc1);chc2=com.First();
            wr=(ta[p1].v.cand | ta[p2].v.cand)-wc; nautres=wr.QC();
            return T2(action);}  

   int T2(USHORT action);
   int T2_el(USHORT el,USHORT action);
   int T2_el_set(USHORT len);// if len =0 get len if len>0 go
   int T2_el_set_nacked(USHORT len);
   int T2_el_set_hidden(USHORT len);
   int T2_el_set_nack_pair();
//==================================================
// main subroutines


   int RID(int l1,int l2,int c1,int c2);  // entry : a potential UR to check
   int RID2(int rat);
   int RID3();
};


P81 * CRIN::ta,*CRIN::tr; // ta action, tr recherche 
OBBIEL * CRIN::tchel;

// CRINT is a table storing possible UR type other than 1 for further processing
class CRINT  
{
public:
	CRIN tur[20];  
	int n;

	void Init() {n=0;}
	void Store(CRIN *x) {if(n<20) tur[n++]=(*x);}
	int Traite(int rat) 
	{int irat=rat-44; if(irat<2 || irat>4) return 0;
    // EE.E("recherche UR rating =");EE.E(rat);EE.E(" pour irat=");EE.Enl(irat);
	 for(int i=0;i<n;i++)
		 if( tur[i].T2(irat)==1)
			{tur[i].ImageRI(" action from object");     return 1; }	
	 return 0;}
};
