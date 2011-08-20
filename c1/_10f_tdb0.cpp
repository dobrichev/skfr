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
/* the key routine expanding pieces of path out of a->b elementary components
   ExpandAll is the fast mode using in bloc what has already been done  
   partial mode are found in the BFTAG functions
   */ 

void TDB::ExpandAll(TDB & from )
{(*this)=from; // be sure to start with the set of primary data
 for( int i=2;i<col;i++)
  {if (t[i].IsEmpty())continue;   int n=1,pas=0;
   while(n)
    { n=0;for(int j=2;j<col;j++)   if((j-i) && t[i].On(j))
	 {BFTAG x=t[j]-t[i];	  if(x.IsNotEmpty()) {t[i]|=x;n++;}
   }} // end j  while
  } }// end i   proc


void TDB::ExpandShort(TDB & from ,int npas)
{(*this)=from; // be sure to start with the set of primary data
 for( int i=2;i<col;i++)
  {if (t[i].IsEmpty())continue;   int n=1,pas=0;
   while(n && (++pas<npas))
    { n=0;for(int j=2;j<col;j++)   if((j-i) && t[i].On(j))
	 {BFTAG x=from.t[j]-t[i];	  if(x.IsNotEmpty()) {t[i]|=x;n++;}
   }} // end j  while
  } }// end i   proc

/* that table is prepared for the derivation of weak links
   the "from" table is the table of implications
   */
void TDB::AllParents(TDB & from)
{t[0].SetAll_0();for(int i=1;i<col;i++) t[i]=t[0];
 for(int i=2;i<col;i++) for(int j=2;j<col;j++) 
	 if(from.t[i].On(j)) t[j].Set(i);
// EE.Enl("parents"); Image();
}

/* That process is valid only if no derived weak link has been produced
   The search finds the lot of shortest eliminations same length
   return 0 if nothing
   return the length if elimination(s) have been found

   from is the TDB of elementary weak links
   elims is set to 1 for all tags found in that process

   only "true" state of non valid candidates are expanded
*/

int TDB::SearchEliminations(TDB & from,BFTAG & elims)
{int npas=0,aigt=0;  elims.SetAll_0();
 (*this)=from; // be sure to start with the set of primary data
while(1)
{int aig=1; // to detect an empty pass
 npas++;
 for( int i=2;i<col;i+=2)  // only "true" state
	 if (zpln.candtrue.Off(i>>1) &&  // candidate not valid
		 t[i].IsNotEmpty()               // should always be
		 )
  {for(int j=2;j<col;j++)   if((j-i) && t[i].On(j))
	 {BFTAG x=from.t[j]-t[i];	  
      if(x.IsNotEmpty()) {t[i]|=x;aig=0;}
     }
   if(t[i].On(i^1)) // an elimination is seen
     {elims.Set(i); aigt=1;}
  }   
 if(aigt) return npas; // eliminations found
 if(aig) return 0;     // empty pass
}// end while
}


/* an X or Y cycle has been found.
   we have the path in the form a->~b->c->~d->a
   each step {a;b} {b;c} {c;d} {d,a} can eliminates candidates seen by both
   (this is a "primary weak link" function)
   look for t[a] & t[b] not IsEmptyl

*/
/*
//================================
int TDB::IsActiveOr(USHORT m1,USHORT m2)// yes if potential elimination
{//EE.E( "isactive");mms.ev(m1);EE.Esp(); mms.ev(m2);EE.Enl();  
for(int j=2;j<col;j++)  
	  if(t[j].On(m1) && t[j].On(m2)  && (j-(m1^1)) && (j-(m2^1)) )return 1;
return 0;}



//=============================
void TDB::CleanOr(USHORT m1,USHORT m2)  // do it
{for(int j=2;j<col;j++)  
	 if((t[j].On(m1)&& t[j].On(m2))	 && (j-(m1^1)) && (j-(m2^1)) )
	 zpln.SuppMarque(j);
}

*/
//<<<<<<<<<<<<<<<<<<<<<<<
void TDB::Image() {EE.Enl( "Image zone tdb");
 for(int i=2;i<col;i++)  if(t[i].IsNotEmpty())  t[i].Image(" ",i);   }

