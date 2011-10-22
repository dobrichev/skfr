
/*              <<<<<<  TCHAIN module   >>>>>>
            handling all potential eliminations though chains

			not yet fully thread safe
			global variables still there

*/





/*	Upper bound for chains is actually unbounded: the longer chain, the higher rating.
 */

USHORT  steps[] = {4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128,
            192, 256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192};

/* attach the instance to the puzzle
*/

void TCHAIN::SetParent(PUZZLE * parent,FLOG * fl){
 parentpuz=parent;
 EE=fl;}
/* start a new cycle (if chains are needed)
*/
void TCHAIN::NewCycle() {
	rating = 200;
	ichain = 0;
	elims_done=0;
	cycle_elims.SetAll_0();
	achieved_rating= parentpuz->ermax;
	maxlength = 10;
	}


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
/* try to clear a candidate should always answer yes
*/

int TCHAIN::ClearChain(int ichain){ // clear candidates
 if(Op.ot){EE->E("clean for UREM=");EE->Enl(chains[ichain].urem);}
 CANDIDATE cw=zpln.zp[chains[ichain].cand];
 return T81t[cw.ig].Change(cw.ch);
}

int TCHAIN::ClearImmediate(USHORT cand){ // clear candidates
 if(cycle_elims.On(cand)) return 0;
 if(Op.ot){EE->Enl("immediate elimination through chain for candidate ");
           zpln.Image(cand);
            EE->Enl();
 }
 cycle_elims.Set(cand);
 CANDIDATE cw=zpln.zp[cand];
 int ir=T81t[cw.ig].Change(cw.ch); // should always answer yes
 if(ir) elims_done=1;   // but keep protected against loops
 return ir;
}

int TCHAIN::Clean() { // clear all pending candidates
 int ir = elims_done;  //  answer is yes is immediate eliminations done
 for(int i = 0; i < ichain; i++)
 		ir += ClearChain(i);
 return ir;
}



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
    check for safety that the load is valid

	if possible, (rating lower than the achived one)
	do immediate elimination
*/
void TCHAIN::LoadChain(USHORT rat,char * lib,USHORT cand)
{ if(rat>rating) return ;
  if(rat<=achieved_rating) { //then do it  
	  ClearImmediate(cand);
	  return;
  }
  if(rat<rating) {ichain=0;  rating=rat;}
  if(ichain>=30) return;
  if(Op.ot)
           {EE->Enl();parentpuz->PointK(); EE->Esp(); EE->Enl(lib);
			EE->E(" load tchain rating=");EE->E(rat);
		    EE->E(" elimination of ");zpln.Image(cand);
			EE->Enl();}
   chains[ichain].cand=cand;
   chains[ichain++].urem=parentpuz->couprem;
}


int TCHAIN::IsOK(USHORT x) {
	if(elims_done) return 1;
	int ir=((rating <= x) );
	return ir;
	//{return ((rating <= x ) || (rating <= Op.ermax));}
}

