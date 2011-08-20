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
// Batch Start
// routines calles by "Main" and specific functions used
//==== specific data
char finput_name[128]; // string file names if given
char * version="V1.0.0 dated June 24th 2011";

// =====================functions written below

int Convert_String_to_string(char * str,System::String  ^Str);
int Search_ccd(char * ww,int n);  // search commands 2 or 3  bytes
int Search_long(char * ww);   // search for extended commands eg: input=FILE
int GetRating(char * z);     // look for xx.y
void Filter_quasi_ed(char * ww); // finish command -n(?)>
int CallOpenLog(char *);
//==========================================
//! Start the batch of puzzle rating
/**
 * Batch_Start analyzes the command line.<br>
 * Command line uses Unicode char that are converted to null terminated string.
 * It looks for options and control coherence (min & max).<br>
 * Open input outputs files and 
 * makes all batch initialisations.
 * \param args arguments from the command line
 * \return 0 if an error occured (incoherence between min max options 
 * or error when opening files), 1 if all is ok
 */
int Batch_Start(array<System::String ^> ^args)
{
	char ww[512];
	cout <<"Version: "<< version<<endl;
	int narg=args->GetLength(0);
	for(int i=0;i<narg;i++)
	{
		if(!Convert_String_to_string(ww,args[i])) continue;// ignore if error
		int val1;
		int ir=Search_ccd(ww,3);	// look for 3 char command
		if(ir>=0)
		{
			if(Op.o1 && Op.o1<3) continue; // ignored if -d -p
			Op.o1=3; // special filter on
			if(ir<5) 
			{
				val1=GetRating(&ww[3]); // get rating threshold
				if (val1<0) continue;
			}
			switch (ir)
			{
			case 0:Op.maxed=val1; break;  //-d<
			case 1:Op.mined=val1; break;  //-d>
			case 2:Op.maxep=val1; break;  //-p<
			case 3:Op.minep=val1; break;  //-p>
			case 4:Op.maxer=val1; break;  //-r<
			case 5:Filter_quasi_ed(ww);   break;  // -n(?)>
			}
			continue;// don't try command 2
		}// end command 3 bytes
		ir=Search_ccd(ww,2);
		if(ir>=0)
		{
			if(ir==6)
				{ir=Search_long(&ww[2]);  //"--" 
				 if(ir>5) // this is a command to limit the process
				 {Op.oexclude= ir-5; continue;}
			    }
			if(ir>=0)
				switch (ir)
				{
				case 0:Op.o1=1; break;  //-d
				case 1:if(Op.o1-1)Op.o1=2; break;  //-p
				case 2: 
					if(ww[1]=='-')strcpy_s(finput_name,128,&ww[8]);  
					else  strcpy_s(finput_name,128,&ww[2]); 
					finput.SetName(finput_name);
					cout <<"input asked   "<<finput_name<<endl;
					break;  //-i
				case 3: Op.os=1; break;  //-s
				case 4: Op.ptime=1; break;  //-e
				case 5: Op.ot=1; break;  //-t
				case 7: Op.o1=1;Op.delta=2; break;  //-D
				case 8: if(Op.o1-1)Op.o1=2;Op.delta=2; break;  //-P
				case 9: Op.oq=1; break; //-Q 
				}
		}// end command 2 bytes
	}// end loop on options
	// output options parsing on console and verify coherence between min and max
	if(Op.pctl()) return 0;	//return if incoherence in options
	// open input and outputs files (rated and non rated files)
	if(finput.OpenFI()) return 0; // 1 if error open
	if(foutput.OpenFO1()) return 0; // 1 if error open
	if(se_refus.OpenFO2()) return 0; // 1 if error open
	// if debugging option is on, call open ficlog with the appropriate name
	if(Op.ot) {finput.GetLogName(Op.logFileName); 
	           if(CallOpenLog(Op.logFileName)) return 0;
			   cout << "open log fait"<<endl;}
	// set option to the rating engine
   setParamC (Op.o1, Op.delta, Op.os,Op.oq,Op.ot , Op.oexclude, Op.edcycles);
   setMinMaxC(Op.mined,Op.maxed, Op.minep, Op.maxep, Op.miner, Op.maxer,Op.filters.f);
 //  if(PrintOptionsOpsudo()) return 0;
   return 1;
} 

//========================================
//! try to locate a command length n (2 or 3)
/**
 * Can recognize 2 char command only if no additional char
 * ie "-i" is good "-i3" is not recognized
 * \param ww an option
 * \param n 2 or 3 length of searched option
 * \return index of command, -1 if not found
 */
int Search_ccd(char * ww,int n)
{
	// List of 3 char command, 6 commands
	char * ccd3[]={"-d<" , "-d>" , "-p<" , "-p>" , "-r<" , "-n("  };
	// List of 2 char command, 9 commands
	char * ccd2[]={"-d" ,  "-p" , "-i" , "-s" ,  "-e" , "-t" ,"--", 
	               "-D" , "-P", "-Q"  };
	char wt[4]; 
	strncpy_s(wt,4,ww,n);
	wt[3]=0;
	for(int i=0;i<((n==2)?10:6);i++)
		if(!strcmp(wt,(n==3)?ccd3[i]:ccd2[i])) 
			return i;
	return -1;
}

//=============================================
//! Try to identify long form of options beginning with "--"
/**
 * \param ww
 * \return index of command, -1 if none recognized
 */
int Search_long(char * ww)
{
	char * cc[]={"diamond" ,  "pearl" , "input=" , "split" ,
               "elapsed" , "test",  "NoMul",
			   "NoDyn", "NoDPlus","NoNest1","NoNest2"};
	char wt[20]; 
	for(int i=0;i<11;i++) // loop on the 11 possible commands
	{
		// copy options and truncate at possible option length
		int j=strlen(cc[i]); 
		for(int k=0;k<j;k++)wt[k]=ww[k];
		wt[j]=0;
		if(!strcmp(wt,cc[i])) 
			return i;
	}
	return -1;
}

//=================================================
//! Get a rating from an options string
/**
 * Command like "-d<xx.x" include a rating "xx.x" that 
 * is converted to an int (x10)
 * rating are not accepted above 11.9 and cannot begin with 0
 * so there are between 1.0 and 11.9
 * \param z part of the option that represent a rating "xx.x" or "x.x"
 * \return rating x 10, -1 if error
 */
int GetRating(char * z)
{	int val=0;
	if(z[0]<'1' || z[0]>'9') return -1;
	val=z[0]-'0';	// get first digit
	if(z[1]-'.') 
	{	// 2 digits
		if(val-1) return -1;	// error if first digit != 1
        if(z[1]<'0' || z[1]>'1') return -1;  //10.x or 11.x
		val=10*val+z[1]-'0';
		z++; //skip one position in inflow
	} 
	if(z[1]-'.')return -1;// now must be the '.'
	if(z[2]<'0' || z[2]>'9') return -1; 
	val=10*val+z[2]-'0';  // now val is in the range 010 to 120 for 1.0 to 11.9
	return val;}

//========================================
//! Converting a String unicode to a classical string
/** 
 * \param str a buffer large enough to get the converted string
 * \param Str unicode string
 * \return 0 if an error occured, 1 OK
 */
int Convert_String_to_string(char * str,System::String  ^Str)
{
	pin_ptr<const wchar_t> wch = PtrToStringChars(Str);
	size_t convertedChars = 0;
	size_t sizeInBytes = ((Str->Length + 1) * 2);
	errno_t err = 0;
	err = wcstombs_s(&convertedChars, str, 512,  wch, sizeInBytes);
	if (err != 0)return 0; 
	return 1;
}
//===========================================
//! Finish command  command -n(y)xx.x
/** 
 * y must be in the range [2 9]<br>
 * xx.x must be in the range [4.0 11.0]
 * update <code>Op.edcycles, Op.miner, Op.os</code> 
 */
void Filter_quasi_ed(char * z) // finish command  command -n(?)
{
	if(z[3]<'2' || z[3]>'9') return ;
	int w=GetRating(&z[5]);
	if(w<40 || w>110) return;
	Op.edcycles=z[3]-'0';
	Op.miner=w; 
	Op.os=1;
}

