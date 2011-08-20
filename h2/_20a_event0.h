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
/* the class PAT_TAG is designed to emulate 
   Sudoku Explainer (SE) dynamic behavior

   In SE some patterns are recognized as
   naked pairs; hidden pairs; XWings; row/col_box locked sets

   each time the appropriate condition are there, 
   new "one way" weak links appears

   The event comes out of a "set" including all candidates
   source of the event if they are "false"

   In that program, this is similar to a "derived weak link"
   To stay in SE logic, the derived weak link creates in fact 
    new "one way candidate to candidate" weak link.

   In an AIC, this will come in the following form     
	" x=>y - z "  equivalent to a theoretical " x - Y = y - Z "
   x=>y comes from the set and is a multi chains derivation
   y is our "PAT_TAG" 

   SE sees  " x - Y = y - Z " as a step with a length of one 
    only when all candidates of the set are "left false" 

   This is equivalent to considering directly "x - z" 
   "z" is any candidate false if "y", our PAT_TAG is true.
   as x=>y is one way, x - z is ones way as well. x=>Z

   When a pattern seen by SE is foun, the CB1024 set of 
   candidates (not tags) that would be false is prepared.

   tag  USHORT (as any tag) is the tag number.
        all PAT_TAGS will have an odd tag starting at 16384 (1<<14)
        each entry creates a new id.

   z      (ZINFL) is the cell pattern  to print
   ch     (CB9CG) is the digit pattern to print   
   cand   (CB1024)is the list of candidates that are false if the pat is "true"
          this excludes tha candidates of the set source of the "true"
   type code identify the patterns seen by SE
      
   0     group => row/col _ box locked

   2     XWing row
   3           col
   4     naked pair  
   
   8     hidden pair in an AC/AHS
   9     hidden pair out of an AC/AHS

*/


