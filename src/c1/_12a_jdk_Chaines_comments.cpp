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
//====================================
/* chain process try to be as close as possible of SE 
        but keep a parallel process of all candidates using tagging
   Tagging remains active for 
    . all bivalues
	. equivalent candidates thru groups 

*/
//===================================
/* rules applied    before nesting is active or nesting level 1
   box alignment
   hidden or naked pair
   Fisherman ???

    
	The pattern that can give such effect are first found and 
	 processed in a vicinity analysis to generate relevant weak links


*/
//=====================================
/* Chaining routines using same parameters as SE
   bivalues      1 if digit bivalue in a RCB (Row;col;box) in use
   cell_bivalue  1 if bivalue in a cell of 2 candidates in use 
     (X_chain Y_chain XY_chain)
   nishio     (base 7.5)
   dynamic    (base 8.0)
   multiple   (base 8.5)
   level      (base 1->9 2->9.5 3>10  4>10.5)
             1 nested rule lot 1
			 2 nested forcing chain
			 3 nested multiple forcing chain
			 4 nested dynamic forcing chain

*/ 
