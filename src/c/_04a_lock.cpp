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
// part of JDK class methos processing locked candidates in a box, row,col

void messlock(int obj,int obj2,int ch)
{if(!Op.ot) return;
int   it1=obj2/9,it2=obj/9,ii1=obj2%9,ii2=obj%9;
char c1,c2; c1= (it1-1)?(char)(ii1+'1'):lc[ii1];
			c2= (it2-1)?(char)(ii2+'1'):lc[ii2];
EE.E("-> ");EE.E(orig[it1]);EE.E(ii1+1);EE.E("  digit ");	
EE.E(ch+1);EE.E(" in ");EE.E(orig[it2]);EE.Enl(ii2+1);
}
/* rating difficulty linked to that include
    Single_after_Locked=17,		// locked in box  do only the fix
    Locked_box=26,				// locked in box  no fix
    Locked_RC=28,				// locked in row/col  no fix
  
*/

//<<<<<<<<<<<<<<<<<<<<
int JDK::TraiteLocked(int rating)
{if(rating==26) return TraiteLocked2(18,27); // box only no fix
 if(rating==28) return TraiteLocked2(0,18); // row col  no fix
// now rating 1.7, process only boxes, 
// must generate a new single in an attached box to elem2
// clearing is done in the cell where the fix takes place
 int ir=0; int ialt;
 for(int ich=0;ich<9;ich++)
 {BF81 wf=c[ich],wfel;
   for(int iel=18;iel<27;iel++)
      { wfel=divf.elz81[iel]& wf;if( wfel.IsEmpty())continue;
	    if(divf.IsAutreObjet (wfel,iel,ialt))
	     { BF81 wa=wf&divf.elz81[ialt],wex=wa^wfel;
	       if (wex.IsNotEmpty()) 
             // the search for singles is done only in boxes
			 // intersecting with ialt , so it must be a hidden single
		   {int ok=0;BF81 ww;
			for(int i=18;i<27;i++) // must be a box
				if((i-ialt) && (divf.elz81[i]&divf.elz81[ialt]).IsNotEmpty())
			  {ww=(divf.elz81[i]&jdk.c[ich])-wex;
			    if(  ww.Count()==1)     {ok=1;  break;}
			 }
            if(ok)  // clear others candidates in the cell to be fixed
			{messlock(ialt,iel,ich) ;
			 int i8=ww.First();T81t[i8].Keep(ich);
			 EE.Enl("lock assignment");
			 return jdk.FaitGoA(i8,ich+'1',4);}// immediate return after  assign  
		  }
		}
  }}
return ir;}




//<<<<<<<<<<<<<<<<<<<<
int JDK::TraiteLocked2(int eld,int elf)
{int ir=0; int ialt;
 for(int ich=0;ich<9;ich++)
 {BF81 wf=c[ich],wfel;
   for(int iel=eld;iel<elf;iel++)
      { wfel=divf.elz81[iel]& wf;if( wfel.IsEmpty())continue;
	    if(divf.IsAutreObjet (wfel,iel,ialt))
	     { BF81 wa=wf&divf.elz81[ialt],wex=wa^wfel;
	       if (wex.IsNotEmpty()) 
		   {messlock(ialt,iel,ich) ;
		   T81->Clear(wex,ich);ir=1;
		   wf=c[ich]=wf^wex;   }
		}
  }}
return ir;}