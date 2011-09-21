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
#include "bitfield32.h"

//! class storing  options of the command line
/**
 * warning: all commands using the signs < > must be given within " "  eg "-d<6.2"
 *
 * <li>-d   stop if not diamond also --diamond</li>
 * <li>-p   stop if not pearl also --pearl</li>
 * <li>-D   same as diamond possible deviation 0.2</li>
 * <li>-P   same as pearl possible deviation 0.2</li>
 * <li>-d>  stop if ED lower than    -d&gtxx.y</li>
 * <li>-d<  stop if ED higher than   -d&ltxx.y</li>
 * <li>-p>  stop if EP lower than    -p&gtxx.y</li>
 * <li>-p<  stop if EP higher than   -p&ltxx.y</li>
 * <li>-r<  stop if ER higher than   -r&ltxx.y</li>  
 *
 * only one of the following limitations should be given, 
 * the last one in the command line will act
 * <li>--NoMul stop evaluation at "multi chains" (excluded) internal code 1</li> 
 * <li>--NoDyn stop evaluation at "dynamic" (excluded) internal code 2</li> 
 * <li>--NoDPlus stop evaluation at "dynamic plus" (excluded) internal code 3</li> 
 * <li>--NoLevel2 stop evaluation at "Nested level 2" (excluded) internal code 4</li> 
 * <li>--NoLevel3 stop evaluation at "Nested level 3" (excluded) internal code 5</li> 
 * <li>--NoLevel4 stop evaluation at "Nested level 4" (excluded) internal code 6</li> 
 *
 * <li>-Q   Quick classification for nested level </li>
 *
 * <li>-t   allow printing of the solution (test mode)</li>
 * <li>-n()xx.y  if after n cycles highest rating still lower than xx.y
 *      forces split mode</li>
 * <li>-e  elapsed time per puzzle (total for benchmark allways given)</li></ul>
 * The following options are processed outside that class<ul>
 * <li>-i input file also --input=</li>
 * <li>-s split treatment if dm or dl (or both) defined and forced if -n() defined</li>
 * </ul>
 * For can have all "l" and "m" 
 */
class OPSUDOCMD
{
public: 
 int o1,   //<main mode 0 basic, 1 ed, 2 ep ,3  (n)x.y
	 delta, // authorized deviation in ED EP mode 2 if -D -P
	 ot,   //<option test on
	 os,   //<option split on
	 oq,   // otpion quick classification at nested level
	 oexclude, //<option code for the limit in processes activated
	          // 0 no exclusion 1 stop at multi chain ....0
	 maxed,      //< ed limit for ED filter <
	 mined,      //< ed limit for ed filter >
	 maxep,      //< ep limit for EP filter <
	 minep,      //< ep limit for EP filter >
	 maxer,      //< er limit for -r<     (rate only lowest puzzles
	 miner,      //< er limit for -n(?)> (high rating low ed)
	 edcycles,   //< number of cycles for differed ed
     ptime;     //< write time after er ep ed
    char logFileName[200];
	 /**
	bit 0 =1 if "not rated" or "spit not ok"
	bit 1 =1 if end in "error" or "not finished"
	bit 2=1 if not "diamond" "pearl" with that option
	bit 3=1 if split ok (bit 1 = 0)
	*/
	BF32 filters; // filters families activated  ed;ep;er;n
	OPSUDOCMD(); // constructor to put all values to 0;
	//void Init();  // overall initial values for command line

	int  pctl();  // contrôle print on console for test purpose
	// also check command line compatibility
	
};

