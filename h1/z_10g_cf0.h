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
/* weak links and strong links between tags  are the main tool in that process.
   
   weak links are not acting directly as " a - b " the AIC form but as
   a => ~b  where ~b is any complement to b
   The active table is a TDB class. The TCF class store the weak link 
   the main use is the length computation for complex chains
   
   Strong links appear in that clas thru a "double weak link" a - b + ~a- ~b

   The TZCF class handle also the TDB form

   a basic weak link is reflexive (cell conflict for example) 
     and will generate 2 TDB links  a -b   a=> ~b    b=> ~a

   some weak link not reflexive are also defined
   either as derived weak links from sets.
    or as consequence of any "event"
   Notes: 
   in the form a=> ~b, ~b can be a kind of dummy. In that case, it will end a chain.
   We can store with the same code reflexive and not reflexive form.
      Only the links loaded in TDB will affect the process
   Only one explaination is kept for a weak-link tag to tag. 
      sometimes, several candidate to candidate forms exist

   the storage form includes an ID identifying the kind of link. ID values are
   defined in the WL-TYPE enum.
*/


class TCF 	
{ public:
USHORT  
    cd[2] ,      // the 2 candidates involved in the specific relation
	id,          // the tag form  
	// so far, I am not sure phase is necessary in that process
	// used in my solver to prevent from looping in the  backtracking of an elimination 
    phase;       // phase value in ZTCF at the time of the entry
	void Load(USHORT  m1,USHORT  m2 ,
				USHORT ie,USHORT ph)
	  {cd[0]=m1;cd[1]=m2;id=ie;phase=ph; };
	void Image()
	{ EE.E(">");zpln.Image(cd[0]);EE.Esp(); zpln.Image(cd[1]) ; 
	  EE.E(" id=");EE.Enl(id);}
};


