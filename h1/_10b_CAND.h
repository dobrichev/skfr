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
class ZPTLN
{public:  
USHORT   ch,  // digit
		 ig;  // cell id

inline void Charge(USHORT i,USHORT che) {ig=i;ch=che;}
inline BF81 * GetZ(){return  &zgs.z[ig];}     
void Image(int no=0); // no=1 if false state
void Clear();
};
 #define zpln_lim 320
class TZPTLN 
{ public:ZPTLN zp[zpln_lim];
  BFCAND candtrue; 
  USHORT ip;            // index to zp
  USHORT indexc[9*81];  // digits 81 cells -> cand
  USHORT ptsch[10],iptsch, // to generate weak links
         el;               
	  
 USHORT Getch(int i){return zp[i].ch;};
 
  void Init();
  USHORT Charge0();

  void CellStrongLinks(); // Y mode
  void CellLinks();   // including bivalues  not done in 'X'mode 
  void RegionLinks(int biv)  // bivalues  except in Y mode
       {for(USHORT ich=0;ich<9;ich++) RegionLinks(ich,biv); 
        }
  private:
  void RegionLinks(USHORT ch,int weak);   
  void WeakLinksD();
  public:
  void GenRegionSets();  
  void GenCellsSets();

                   //========= TPT1 work within a layer
 // void SuppMarque(USHORT mm,int direct=0);   
  void Clear(USHORT c) {zp[c].Clear();}
  int ChainPath(int i1,int i2);
  void PrintPath(USHORT * tp,USHORT np);
  void PrintImply(USHORT * tp,USHORT np);
  void PrintListe(USHORT * t,USHORT n,int modetag=0);

  void Image(int i,int no=0) {zp[i].Image(no);  }
  void ImageTag(int i){Image(i>>1,i&1);}
  void Liste();        // for debugging only
  void ListeIndexc();  // for debugging only
};  


 