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

/* the class PATH has been specifically designed for SE clone
   it is a support to look for the shortest path 
     after elimination/cycle has been found in tags terms
   having that class designed, all path are using it
*/
class PATH     
{public: USHORT pth[100],ipth,ipth_plus;
inline void Init(){ipth=ipth_plus=0;}
inline void Set(USHORT i,USHORT x){pth[i]=x;}
inline void Add(USHORT x){if(ipth<50)pth[ipth++]=x;}
void Add(USHORT * pthp,USHORT ipthp)
        {for(int i=0;i<ipthp;i++) Add(pthp[i]);}

void PrintPathTags();

inline USHORT GetLast(){if(ipth) return pth[ipth-1];else return 0;}
inline USHORT GetFirst(){if(ipth) return pth[0];else return 0;}
inline USHORT Get(int x){return pth[x];}
inline void Back(){ipth--;}

} ;  



