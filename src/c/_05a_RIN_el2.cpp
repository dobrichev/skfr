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


// we are now looking for nacked or hidden sets not  pair
// we have not so many possible patterns, 
// we try to catch them per pattern
// it need more code, but it should be faster and easier to debug

// look for hidden  
int CRIN::T2_el_set_hidden(USHORT len)
{	// first pattern, take it if direct 
 if(nth==len)   // not the expected length
 { // identify extra digits in hidden locked set
 BF16 whh=wh-wnh;  // all digits of the assumed locked set
 if(whh.QC()-(nth+1)) return 0;
 //go for the a hidden set if active
  int ir1=0;
  for(int i=0;i<nth;i++)  ir1+=T81t[th[i]].Keep(whh); 
  if(ir1) { EE.E("UR/UL hls whh="); EE.Enl(whh.String());
	       EE.Enl("UR/UL hidden locked set"); return 1;}	
 }
 if(nth==2 && len==3) 
// second pattern with nth=2 but a hidden quad 
// adding another cell
 {	for(int i=0;i<nnh;i++)
   {BF16 wa=wh|T81t[tnh[i]].v.cand,wb,wx;
    for(int j=0;j<nnh;j++) if(j-i) wb|=T81t[tnh[j]].v.cand;
	wx=wa-wb-wr; // must not be an extra digit included in the UR
	if(wx.QC()==4) // we got it
	{int ir1=0; ir1+=T81t[tnh[i]].Keep(wx);
     for(int k=0;k<nth;k++)  ir1+=T81t[th[k]].Keep(wx); 
     if(ir1) { EE.E("UR/UL hls wx="); EE.Enl(wx.String());
	       EE.Enl("UR/UL hidden locked set"); return 1;}	
	}
   }
 }

return 0;}

// look for nacked sets n
int CRIN::T2_el_set_nacked(USHORT len)
 {if(ntd<(nautres-1)) return 0;  // minimum without extra digit

 if(ntd && (nautres==2)&&(len==1)) // look for a nacked pair
  {for(int i=0;i<ntd;i++) 
   {int i8=td[i];BF16 ww=T81t[i8].v.cand;  
    if( ww.f==wr.f) // we got it     
	{ BF81 zwel1=zwel; zwel1.Clear(i8);
	  if(T81->Clear(zwel1,wr  ))
		{ EE.Enl("UR/UL nacked pair");return 1;}
	}}
  }

 if(len<2) return 0;// nothing else if a pair is expected
if(op0) { EE.E("look for nacked len ="); EE.E( len);
          EE.E(" nnh ="); EE.Enl( nnh);}
// first   pattern is nacked triplet no extra digit
if(nautres==3 && ntd>2 && len==2)
 { BF81 zwel1=zwel; int n=0;  
   for (int i=0;i<ntd;i++)      if(!(T81t[td[i]].v.cand-wr).f)  
	                           { n++; zwel1.Clear(td[i]);}
   if(n==2 && T81->Clear(zwel1,wr  ))
		       { EE.Enl("UR/UL nacked LS3");return 1;}
 }

// second  pattern : directly correct (may be with extra digit)
if(((ntd+1)==wd.QC())  && (len== ntd))
 { BF81 zwel1=zwel; 
   for (int i=0;i<ntd;i++)  zwel1.Clear(td[i]);
   if(T81->Clear(zwel1,wd  ))
		{ EE.E("UR/UL nacked LS");EE.Enl(ntd+1);return 1;}
 }
// third  pattern is  "one cell in excess"
//should cover several situations 
if(ntd>=nautres && (len==(ntd-1)))
 { for (int i=0;i<ntd;i++)
	{BF16 wdx=wr;	 // wr is the minimum fo the nacked lock set
	 for (int j=0;j<ntd;j++) if(j-i) wdx|=T81t[td[j]].v.cand;
	 // check if now correct
	      if(ntd==wdx.QC()) //  one cell less ntd must be number of digits
       { BF81 zwel1=zwel; 
        for (int j=0;j<ntd;j++)  if(j-i)zwel1.Clear(td[j]);
		if(T81->Clear(zwel1,wdx  ))
		   { EE.E("UR/UL nacked LS");EE.Enl(ntd);return 1;}
       }
     }
  }

// Fourth  pattern is  "2 cells in excess"
//could cover several situations 
if(ntd>=(nautres+1) && (len==(ntd-2)))
 { for (int i=0;i<(ntd-1);i++)for (int k=i+1;k<ntd;k++)
	{BF16 wdx=wr;	 // wr is the minimum fo the nacked lock set
	 for (int j=0;j<ntd;j++) if((j-i) && (j-k))
		    wdx|=T81t[td[j]].v.cand;
	 // check if now correct
	if(ntd==(wdx.QC()+1))  
       { BF81 zwel1=zwel; 
        for (int j=0;j<ntd;j++)  if((j-i) && (j-k))
			 zwel1.Clear(td[j]);
		if(T81->Clear(zwel1,wdx  ))
		   { EE.E("UR/UL nacked LS");EE.Enl(ntd-1);return 1;}
       }
     }
  }
// fifth pattern,  we look now for something with all extra cells "over all"
// have an example of naked quad, but can be simpler
if(nnh>=nautres && (len==nnh))
 {BF16 wdx; int ir=0;
  for (int i=0;i<nnh;i++) wdx|=T81t[tnh[i]].v.cand;
  if(nnh==(wdx.QC()-1) && ((wdx&wr).f == wr.f) ) 
    { for (int i=0;i<nth;i++)  ir+=T81t[th[i]].Change(wdx  );
      if(ir)  { EE.E("UR/UL nacked LS");EE.Enl(wdx.QC());return 1;}
    } 
 }

// fifth all extra cells except one
if(nnh>=nautres && (len==(nnh-1)))
 {for (int j=0;j<nnh;j++)
  {BF16 wdx; int ir=0;
	  for (int i=0;i<nnh;i++) if(j-i)  wdx|=T81t[tnh[i]].v.cand;
  if((wdx&wr).f-wr.f) continue;
  if(nnh==wdx.QC()) 
    { BF81 zwel1=zwel; 
      for (int i=0;i<nth;i++)  ir+=T81t[th[i]].Change(wdx  );
	  ir+=T81t[tnh[j]].Change(wdx  );
      if(ir)  { EE.E("UR/UL nacked LS");EE.Enl(wdx.QC());return 1;}
    } 
  }
 }
// sixth all extra cells except 2
if(nnh>=(nautres+1) && (len==(nnh-2)))
 { EE.E("look for all extra -2 nnh=");EE.Enl(nnh);
 for (int j=0;j<nnh-1;j++)for (int k=j+1;k<nnh;k++)
  {BF16 wdx; int ir=0;
	  for (int i=0;i<nnh;i++) if((j-i)  && (k-i))
		     wdx|=T81t[tnh[i]].v.cand;
   if((wdx&wr).f-wr.f) continue;
   if(nnh==wdx.QC()+1) 
    { BF81 zwel1=zwel; 
      for (int i=0;i<nth;i++)  ir+=T81t[th[i]].Change(wdx  );
	  ir+=T81t[tnh[j]].Change(wdx  ); ir+=T81t[tnh[k]].Change(wdx  );
      if(ir)  { EE.E("UR/UL nacked LS");EE.Enl(wdx.QC());return 1;}
    } 
  }
 }


return 0;}

/*

// sixth pattern,  we look now for something with 3 extra cells "over all"
// has been seen 
if(nnh>=(nautres+1)&& (len==(nnh-3)) )
 {EE.Enl("look for nacked LS + extra digit less 3 cells");
	 for (int i1=0;i1<nnh-2;i1++)for (int i2=i1+1;i2<nnh-1;i2++)
     for (int i3=i2+1;i3<nnh;i3++)
	{BF16 wdx=wr;	 // wr is the minimum fo the nacked lock set
	 for (int j=0;j<nnh;j++) 
		  if((j-i1) &&(j-i2)&&(j-i3))    wdx|=T81t[tnh[j]].v.cand;
	 // check if now correct
     if((nnh-2)==wdx.QC()) 
       { BF81 zwel1=zwel; 
        for (int j=0;j<nnh;j++)  
			 if((j-i1) &&(j-i2)&&(j-i3))zwel1.Clear(tnh[j]);
         if(T81->Clear(zwel1,wdx  ))
		   { EE.E("UR/UL nacked LS");EE.Enl(nnh-2);return 1;}
       }
     }
  }	 
*/