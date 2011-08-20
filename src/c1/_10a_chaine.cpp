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
/*	Upper bound for chains is actually unbounded: the longer chain, the higher rating.
 */

USHORT  steps[] = {4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128,
            192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192};
/* clean a candidate if the final rating is ok
*/
int CHAIN::Clean() // clear candidates
     {if(Op.ot){EE.E("clean for UREM=");EE.Enl(urem);}
      ZPTLN cw=zpln.zp[cand];
	  return T81t[cw.ig].Change(cw.ch);}
/* accelerator for the search of loops
   to be revised
*/
void TCHAIN::SetMaxLength( int bw)  
       {base=bw;
		if(bw<rating) 
			{if(bw<75) maxlength=16; // same as no limit
			 else maxlength=100;}
		     // chexk why, but more creates a problem in bitfield
		else if(bw>80 || bw==75) maxlength=100;// linked to CANDGO
		else if(bw==rating )  maxlength=4;// equivalent to loop length 6
		else {int ii=rating - bw; maxlength=steps[ii]/2+2;}
       }
/* same gaol in dynamic mode
   to be revised as well
   */
USHORT TCHAIN::ParsingMini(USHORT basee) {if(rating>(basee+10)) return 200; //max
                                   if(rating <=basee) return 6; // mini
								   int ii=rating - basee; return(steps[ii]/2+2);}
/* another function to be revised
*/
int TCHAIN::GetYlower(int lg) // limit for a shorter chain/loop
       {int xl=2*(lg-1); //  equivalent length pure Y mode
        if(xl<7 ) return 0; //nothing shorter can be found 
		if(xl>32) return 0;// should never happen
		xl-=2; // the length for index
		for(int ii=0;ii<6;ii++) if(xl<steps[ii+1])	return steps[ii]/2+2;
        return 0;} // shoul never happen
/* new function to control performances
   the target is the following
   a chain has been found, what is the maximum length for a loop
     having a lower rating
  in 'X' 'Y' mode, the base is 0.1 lower
  in 'XY' mode the base is the same
  modexy is 1 if "XY" else it must be 0
*/
 int TCHAIN::GetLimit(int lg,int modexy) // limit for that group
       {lg-=2; // the length for index
		for(int ii=0;ii<10;ii++) if(lg<=steps[ii])	
			return (steps[ii]-modexy);
        return 0;} // shoul never happen
/* attempt to enter the table of pending eliminations.
   compute the rating and return 0 if
    . higher than current rating, 
	. equal to current rating and 
	   - table is full
	   - or the cand is already in pending eliminations
   if not
   return the rating found
*/
int TCHAIN::GetRatingBase(USHORT bb,USHORT lengthe,USHORT cand)
       { if(lengthe<2) return 0; // should not happen
		  USHORT wrating=300,length=lengthe-2; int ii;
          for(ii=0;ii<22;ii++) 
	         if(length<= steps[ii]) {wrating=bb+ii; break;}
          if(wrating>rating) return 0;
		  if(wrating==rating )
			  {if(ichain >=30)  return 0;
		       for(int i=0;i<ichain;i++) if(chains[i].cand==cand) return 0;
		      }
		  return wrating;}

 /* Load after GetRating
    check for safety the load is valid
*/
void TCHAIN::LoadChain(USHORT rat,char * lib,USHORT cand)
{ if(rat>rating) return ;
  if(rat<rating) {ichain=0;  rating=rat;}
  if(ichain>=30) return;
  if(Op.ot)
           {EE.Enl();jdk.PointK(); EE.Esp(); EE.Enl(lib);
			EE.E(" load tchain rating=");EE.E(rat);
		    EE.E(" elimination of ");zpln.Image(cand);
			EE.Enl();}
   chains[ichain++].Load(cand);
 }
 