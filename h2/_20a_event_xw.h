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
/* PAT_TAG with a claiming/pointing pattern 

   claiming / pointing are covering similar patterns.
   each of the 9*54 groups box_row/col has to be considered;
   assuming the group exists (with more than one candidate)
            the group is not alone is row/col/box,
   then if all candidates of the complément is the row/col_bax are "false"
        the group is valid and the conterpart in the bow_row/col is not valid

   as in the XWing situation below, each group generate 2 tags, 
     one in bos, one in row/col

   the set in the first case is the CB1024 in the second case and vice-versa
   Note: the set can be reduced to one candidate. In the program logic, this should stay as a set
     (2 tags including the PAT_TAg°.
   Each of the 9*54 groups will generate 0 or 2 PAT_TAGs.
   Note 2: in a pattern with 3 singles and 1 group in 2 blocs, nothing is done.


PAT_TAG with a XW pattern 
   
   XWing pattern exists for each group of cells  
     2 rows; 2 colums; 2 or 4 boxes
     with one "digit candidate" belonging to the 4 cells

   Xwing can come either from rows or from columns.

   if XWing comes from rows
     the set is done of all candidates out of the square in the rows
	 all candidates in the columns out of the square become false

  here an example from a puzzle solved thru XW by SE

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
 
  Potential XWing is 5r1c24r7c24

  thru rows the XW is true if 5 r1c1 r7c7 are false (the set)
  then 5 r3459c2 r39c4 are false (the CB1024)

  thru columns we have just to exchange the set and the CB1024
  if 5 r3459c2 r39c4 are false (the set now 6 candidates)
  then 5 r1c1 r7c7 are false (the CB1024)

  We define 2 separate tags, 
   one for the appearance of the XW rows
   one for the appearance of the XW columns 

 my rough estimate of the number of tags we need, is 1 000
 which means 1 000 sets

*/


