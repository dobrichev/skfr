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
/*  PAT_TAG with a hidden pair pattern 
   
   SE looks for hidden pairs 
   this means SE considers the status of "all??" pairs of cells in a row,column,box.
   and this after each step in the development of the solution.

   In fact, the best chances to get a hidden pair comes from AHS and from cells having a small number of candidates 
   (due to the length issue, but I guess this is not the way SE is working.) 

   here after is 
  5789 568  1     |4568  2     46789 |3    4789 789   
  789  4    678   |168   1679  3     |279  5    2789  
  2    358  3578  |458   4579  4789  |6    4789 1     
  ---------------------------------------------------
  458  2568 4568  |7     1469  12469 |1259 3    25689 
  1    2356 3567  |236   8     269   |4    279  25679 
  3478 9    34678 |12346 146   5     |127  1278 2678  
  ---------------------------------------------------
  6    158  2     |158   3     178   |1579 179  4     
  3458 7    3458  |9     145   1248  |125  6    235   
  345  135  9     |12456 14567 12467 |8    127  2357

  in that example, SE uses r3c35. In former step using the start "1r7c7 true"
  5r3c24 and  7r3c68 have been "false"
  so we find the "upstream controlled" hidden pair "57r3c35"
  and 1r7c7/{3;8r3c3; 4;9r3c5) (going downstream)

  This can lead to a very big number of possible patterns. 
  For the time being, I'll limit the search to sets with a maximum of 5 candidates.

  PAT_TAG with a naked pair pattern 

  In that same example, SE uses the cells  r5c8r6c7 to form a naked pair
  in that case, we have an upstream elimination of 9r5c8 and 1r6c7.
  this gives 1r7c7(true) => naked pair 27r5c8r6c7
  and consequently (going downstream)
  1r7c7/{27 in other positions of the column or box)

  again, the number of possibilities grows rapidly with the number of candidates in the 2 cells.
  I'll limit in a first step the search to cells containing in total 4 digits (6 atgs).
  Going to 5 digits will grow the number of tags to 10.
  Going to 6 digits would grow the number of tags to 15
  */


