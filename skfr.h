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

#include "ratEnum.h"            // enum list of solving techniques
#include "bitfields1.h"		// bitfields are grouped in this file
BF_CONVERT bfconv;
#include "bitfields2.h"		// bitfields are grouped in this file
#include "opsudo.h"             // storing and managing options
class OPSUDO;
 OPSUDO Op;

#include "finout.h"                // relay for printing only in tests
class FINPUT ;
 FINPUT finput;
class FOUTPUT;
 FOUTPUT foutput, se_refus, flog;
#include "fsr.h"                // relay for printing only in tests
class FSR;
 FSR EE;

#include "wa_puzzle.h"
DIVF divf;
class ZTOB;
  ZTOB aztob;
#include "wb_bruteforce.h"
 UN_JEU un_jeu;
#include "h\_03a_jdk.h"                  // general class to solve a puzzle
class JDK;
  JDK jdk;
TP81F tp81f;
P81F * t81f=tp81f.t81f;			//pointer to speed up the process
TP81 T81dep;
TP81 * T81,*T81C;		//standard names for main objects of the class
class P81;
 P81 * T81t,*T81tc;		//and corresponding tables of cells  

#include "h\_04b_tir.h"                // class for processing of locked sets and fishes
class TIR;
 TIR yt;
#include "h\_04d_paires.h"
class TPAIRES;
 TPAIRES zpaires;
#include "h\_05a_RIN.h"
class CRIN;
 CRIN ur;
class CRINT;
 CRINT urt;
class ULT;
  ULT tult;

// h1 subdirectory contains exclusively classes for tagging
//logique de regroupement que je ferai dans le nettoyage
#include "h1\_10a_path.h"
#include "h1\_10a_chaine.h"

//  un lot zg + pt + tpt
#include "h1\_10a_zg.h"
#include "h1\_10b_cand.h"    //
class TZPTLN;
 TZPTLN zpln;
/*
// un lot tdb + cf0 + cf à renommer ?? 
#include "h1\_10f_tdb.h"
#include "h1\_10g_cf0.h"
#include "h1\_10g_cf.h"
// un lot choix + tchoisx a renommer SETS TSETS
#include "h1\_11m_choix.h"
#include "h1\_11n_tchoix.h"
#include "h2\_20a_event.h"
*/

// un lot tdb + cf0 + cf à renommer ?? 
#include "h1\_10f_tdb.h"
//#include "h1\_10g_cf0.h" now obsolete
#include "h1\_10g_cf.h"
// un lot choix + tchoisx a renommer SETS TSETS
#include "h1\_11m_choix.h"
#include "h1\_11n_tchoix.h"


#include "h2\_20a_event.h"
