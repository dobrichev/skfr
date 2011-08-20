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
/* deriving for SE clone
SE clone has several kinds of sets
the basic choices as in my solver  {a;b;c} where all are candidates
Sets leading to a known pattern identified by the tag 'P' {a;b;c;P}

In the first mode, any candidate of the group can be the target,
in the second group, only 'P' can be the target. 'P' is a special tag.

Note: SE does not recognize {a;b;c;d}  x -{a;b}    y - {c;d} -> x - y


Another point is that SE, working left to right with a "shortest length" goal
 will use in priority short sequences.

One way to go faster is to give higher priority to sequences as 
 a=12345  b=12  c=123   ( 1a=>3c  2a=>3c )

 In the clone process, this is [] 1a - 1b = 2b - 2c  [] 1a - 1c  (length 7 in SE).

Another point is that if in tag mode we fing x - {a,b,c} 
we have to consider all possible values of 'x' in candidate mode (most often one, but sometime more.)

Last but not least, if x = X - y - {a;b;c} = d, 
 we have no need to create x=>d we only need y=>d

Each new weak link is summarized in SE by the list of candidates used to find it
this is a cumulative property 

Note: a new link is needed only if it does not already exist

if we have a=> b we expand in tag mode a - B
if we find later b=>a we expand   A - b  it is another conflict.

The only special situation is a=>b and B=>A 
  what should never be when we reach that difficulty level
  to keep the possibilty to catch it, 
    we maintain in TZCF a specific TDB "single" registering all One way entries
  

  



*/