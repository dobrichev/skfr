#pragma once

#include "ratEnum.h"            // enum list of solving techniques

#include "flog.h"                // relay for printing only in tests
#include "bitfields.h"		// bitfields are grouped in this file
#include "opsudo.h"             // storing and managing options


#include "wa_puzzle.h"
#include "wb_bruteforce.h"
#include "_03a_puzzle.h"                  // general class to solve a puzzle
//#include "h\_05a_RIN.h"


// h1 subdirectory contains exclusively classes for tagging
//logique de regroupement que je ferai dans le nettoyage
#include "h1\_10a_path.h"
#include "h1\_10a_chaine.h"

//  un lot zg + pt + tpt
#include "h1\_10a_zg.h"
#include "h1\_10b_cand.h"    //
// un lot tdb + cf0 + cf à renommer ?? 
#include "h1\_10f_tdb.h"
#include "h1\_10g_cf.h"
// un lot choix + tchoisx a renommer SETS TSETS
#include "h1\_11m_choix.h"
#include "h1\_11n_tchoix.h"



#include "h2\_20a_event.h"
#include "h2\_30a_CANDGO.h"  // calle exclusively by chain_plus
#include "h2\_30a_TCANDGO.h"  // calle exclusively by chain_plus
