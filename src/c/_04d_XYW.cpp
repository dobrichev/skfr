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

//<<<<<<<<<<<<<<<<<<<< // depart deux paires pas objet commun et trio
int TPAIRES::XYWing() // troisieme par les isoles  objets  communs
{int ir=0;    
 for(int i=0;i<ip-1;i++)
 for(int j=i+1;j<ip;j++)
    { if(CommunPaires(i,j)) continue;
      BF16 w=zp[i].pa|zp[j].pa;  if(w.QC()-3) continue;
      BF16 w1=(zp[i].pa&zp[j].pa),w2=w1 ^ w;  // les deux non communs
       for(int k=0;k<ip;k++)
	 {if( zp[k].pa.f-w2.f) continue; // il faut =
	  if(!CommunPaires(i,k)) continue;
	  if(!CommunPaires(j,k)) continue;
	  // on a un XYWing  potentiel
	  int ich=w1.First(); // le chiffre 
	  BF81 z1= t81f[zp[i].i8].z & t81f[zp[j].i8].z ,
	        z2=z1&jdk.c[ich]  ;  // z2 est à supprimer
	  if(z2.IsNotEmpty() ) 
	  {	CommunLib(i,j,zp[k].i8,"->XY WING pivot= ");  T81->Clear(z2,ich); return 1;  }
     } 	}
 return ir;}
 //<<<<<<<<<<<<<<<<<<<< // depart deux paires pas objet commun et trio
int TPAIRES::XYZWing() // troisieme est le trio objets communs
{int ir=0;  
 for(int i=0;i<ip-1;i++)
  for(int j=i+1;j<ip;j++)
   { if(CommunPaires(i,j)) continue;
     BF16 w=zp[i].pa|zp[j].pa;  if(w.QC()-3) continue;
     BF16 w1=(zp[i].pa&zp[j].pa);  // le chiffre
      for(int k=0;k<81;k++)
       	{if(T81t[k].v.cand.f-w.f) continue; // il faut = trio
	 if(!CommunTrio(k,i)) continue;
	 if(!CommunTrio(k,j)) continue;
	 // on a un XYZWing  potentiel
	 int ich=w1.First(); // le chiffre
	 BF81 z1= t81f[zp[i].i8].z & t81f[zp[j].i8].z & t81f[k].z,
	       z2=z1&jdk.c[ich]  ;  // z2 est à supprimer
	 if(z2.IsNotEmpty() )  
	   {CommunLib(i,j,k,"->XYZ WING pivot= ");  T81->Clear(z2,ich); return 1;   }
    }   }
 return ir;}
