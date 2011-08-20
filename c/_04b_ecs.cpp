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


//<<<<<<<<<<<<<<<<<<<<<   On cherche tiroir dans element  // boucle recurrente sur i
int OBBIEL::tiroir(BF16 fd,int iold,int irang)
{  // on progresse sur indice et on regarde si fusion n-1  ok
	int i,id=(yt.non.f && (irang-1))? 0:iold+1;  // debut 0 avec pseudos et rang 1
	for (i=id;i<(11-yt.rangc+irang);i++)  // il doit rester des cases
	{ 
		if(eld[i].n<2 ||eld[i].n>yt.rangv) continue;
		if( yt.non.On(i))continue; //pour pseudo
		BF16 wf=eld[i].b|fd; 
		if (wf.QC() >yt.rangv) continue;
		if(irang==(yt.rangc-2))
		{yt.wf=wf;yt.wi.Set(i);
		if(wf.QC() ==yt.rangv)return 1; else return 0; }
       	// une erreur a tester    on poursuit si <
    	// il peut manquer des tiroirs!!!!! curieux que jamais détecté!!!!
	 if(tiroir(wf,i,irang+1)){yt.wi.Set(i); return 1; }
  }
return 0;}

int TIR::Tiroirs(int nn,int hid,int sing)     //recherche normale des tiroirs
{rangv=nn;single=sing;hid_dir=hid; int ir=0;
int ied=0,ief=54; 
if(!hid_dir )ief=27;if(hid_dir==1 )ied=27;if(single){ied=27;ief=54;}
  for( e=ied;e<ief;e++)   // direct, hidden or both upon request
  {rangc=rangv; non.f=cases.f=0; if(e<27) e27=e; else e27=e-27;
	if(jdk.NonFixesEl(e%27) < (rangv +1)) continue;
	for(int i=0;i<9-rangv+1;i++)
	{int nn= aztob.tpobit.el[e].eld[i].n;
	 if(nn<2 || nn>rangv) continue;
		BF16 w;w=aztob.tpobit.el[e].eld[i].b;
	 wi.f=0; wi.Set(i);
	 if(!aztob.tpobit.el[e].tiroir(w,i,0)) continue;
	 if (UnTiroir())ir= 1;	
	}  
   }
if(ir)return 1; 
return 0;}

int TIR::UnTiroir()
{// is there a single required after the locked set to accept it
 int ir=0;
 if(single) // will be covered slowly can be any element row, col, box
 { for(int i=0;i<9;i++)  if(wf.Off(i))
      {USHORT i8=divf.el81[e27][i]; 
       P81 p=T81t[i8];  if(p.v.typ ) continue;// must be non assigned 
	   BF16 wc=p.v.cand-wi;
	  	   for(int j=0;j<9;j++) if (wc.On(j) )// a possible hidden digit
	      {BF16 wcd=aztob.tchbit.el[e27].eld[j].b-wf; // positions still valid
	       if(wcd.QC()==1)
             {EE.Enl("ecs assignment");
			  jdk.FaitGoA(i8,j+'1',4);// stop at first assignment
		      ir=1; break;}
	      }// end for j if
	  
    }// end hidden ls
  if(!ir) return 0;// no single found
  }// end if single

 else if(e<27) {if (!jdk.ChangeSauf(e,wi,wf)&&(!ir) )return 0;  }
	   else   { if (!jdk.Keep(e27,wf,wi) &&(!ir))return 0;  }

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

 