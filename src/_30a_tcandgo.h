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

/* In nested mode, the track back process is not so easy.
   To achieve it relatively simply, we use a side table filled in the forward process.
   for each new strong link found we open a new entry
   for each new generation  in the forward process, we open a new entry
   reached thru a direct index
   but the new entry is done in advance if a side exclusion is found
   each entry contains
      a BFTAG containing all tags used to come to a decision
      the count if the source is a nested rule.
	  the index in CHAINSTORE to find back the chain at print time

*/

class CANDGOFORWARD { // one entry per step.
public:
	BFTAG source;
	USHORT count, index;
	void Add(USHORT ind, const BFTAG &bf, USHORT cpt) {
		index = ind;
		source = bf;
		count = cpt;
	}
};
class CANDGOSTRONG { // no need to sort the key, entry is doubled
public:
	BFTAG source;
	USHORT count,key1,key2;
	void Add(USHORT k1, USHORT k2, const BFTAG &bf, USHORT cpt) {
		key1 = k1;
		key2 = k2;
		source = bf;
		count = cpt;
	}
};

class TCANDGO {
public:
	CANDGOFORWARD tt[5000];
	CANDGOSTRONG ts[600];
	USHORT itt, its;
	void Init() {
		itt = its = 1;
	} // keep 0 as invalid return
	void AddStrong(USHORT k1, USHORT k2, const BFTAG &bf, USHORT cpt) {
		if(its >= 598)
			return;
		ts[its++].Add(k1, k2, bf, cpt);
		ts[its++].Add(k2, k1, bf, cpt);
	}
	const CANDGOSTRONG * Get(USHORT t1, USHORT t2) const {
		USHORT c1 = t1 >> 1, c2 = t2 >> 1;
		for(int i = 1; i < its; i++) {
			const CANDGOSTRONG * w = &ts[i];
			if(w->key1 - c1)
				continue;
			if(w->key2 - c2)
				continue;
			return w;
		}
		return 0;
	} 

};//tcandgo;

/* storage of the nested chains for pring purpose
  storing is done in a buffer with a double index
  primary for a chain (start,number=
  secondary for a nested object: start index, end index
  */
  
class  CHAINSTORE {
public:
	USHORT buf[10000], ibuf,        
		starts[2000], ends[2000],
		ise,
		s2[500], e2[500], ise2;

	void Init() {
		ibuf=0;
		starts[0] = ends[0] = 0;
		ise = 1; 
		s2[0] = 0;
		e2[0] = 0;
		ise2 = 1;
	} // 0 is "empty"
	USHORT AddChain(USHORT * tch, USHORT n) {
		if(ibuf + n > 10000)
			return 0;
		starts[ise] = ibuf; 
		for(int i = 0; i < n; i++)
			buf[ibuf++] = tch[i];
		ends[ise] = ibuf; 
		if(ise >= 499)
			return ise; // don't pass the limit
		else
			return ise++;
	}
	USHORT AddOne(USHORT * tch, USHORT n) {
		if(ise2 >= 2000)
			return 0;
		s2[ise2] = e2[ise2] = AddChain(tch, n);
		return ise2++;
	}
	USHORT AddMul(USHORT d, USHORT f) {
		if(ise2 >= 2000)
			return 0;
		s2[ise2] = d;
		e2[ise2] = f;
		return ise2++;
	}
	void Print(USHORT index);
}tstore;

