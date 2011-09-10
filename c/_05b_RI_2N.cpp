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

int TP81::RIN(int aig)       // look for unique rectangle 
{int ir=0; urt.Init();
 for(int i0=0;i0<3;i0++) // band/stack 1 to 3
 for(int i1=0;i1<2;i1++)for(int i2=i1+1;i2<3;i2++) // 2 rows  
 for(int j1=0;j1<2;j1++)for(int j2=j1+1;j2<3;j2++) // boxes   12 13 23
  for(int j3=0;j3<3;j3++)for(int j4=0;j4<3;j4++)  // 2 cols  
   //analysing band and stack, main diagonal symmetry
   {int l1=3*i0+i1,l2=3* i0+i2,c1=3*j1+j3,c2=3*j2+j4;
    if (ur.RID(l1,l2,c1,c2)||ur.RID(c1,c2,l1,l2)) ir++;  }
return ir;}

// one posible location for a UR;
// no assigned position, 2 and only 2 common digits

int CRIN::RID(int i1,int i2,int c1,int c2)
{ia=I81::Pos(i1,c1);ib=I81::Pos(i1,c2);ic=I81::Pos(i2,c1);id=I81::Pos(i2,c2);
 char * gr=jdk.gg.pg;
 if( (gr[ia]-'0') ||(gr[ib]-'0') ||(gr[ic]-'0') ||(gr[id]-'0') ) return 0;
 if(Setw()-2) return 0;   CalcDeux();   
 if(ndeux==3)  {ta[pp1].Change(wc);   ImageRI("1");  return 1;}// type 1
 Setwou();  GenCh(); 

 
 if((ndeux-2)|| IsDiag())
	 if(ndeux==1) return RID3();
		 else return 0; // not processed

 // if one digit active, do it now 4.5
 if(nautres==1)  // un chiffre en lig/col  ou diagonal
     if(jdk.Keep (ch1,pp1,pp2))  {ImageRI(" one digit active"); return 1; }
   // if one of the common digit is a bivalue, do it as well it is 4.5
 while((tr[ia].v.ncand>2) ||  (tr[ic].v.ncand>2 ))
    {USHORT pw=ia; ia=ib; ib=id;id=ic;ic=pw;}// sort cells 
 int ir= T2(1) ; // we  want only rating 4.5 as "good" so far
 if(ir==1) {ImageRI(" action from object");return 1;} // it was ok
 if(ir)  urt.Store(this); 
 return 0; //store if something else seen  
  }


int CRIN::RID3()
{if(nautres-1) return 0;
BF81 zw; zw.SetAll_1();

if(tr[ia].v.ncand==3) zw &= t81f[ia].z;
if(tr[ib].v.ncand==3) zw &= t81f[ib].z;
if(tr[ic].v.ncand==3) zw &= t81f[ic].z;
if(tr[id].v.ncand==3) zw &= t81f[id].z;

zw &=jdk.c[ch1];


if(zw.IsNotEmpty()){ImageRI(" UR one digit active");	
                 return T81->Clear(zw,ch1);}
return 0;}