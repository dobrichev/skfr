/*              <<<<<<  ZGS table   >>>>>>
        list of all BF81 in use
		can disappear in an optimised code for skfr
		inherited from GP solver where more pattern were used


*/


ZGROUPE::ZGROUPE (PUZZLE * parent) {
	parentpuz = parent;
	int i,j;
	BF81 z0,z1; 
	z0.SetAll_0(); 
	z1.SetAll_1();
	for(i=0;i<81;i++)
	{
		z[i]=z0;
		z[i].Set(i);
	}  // les z de 81 cases
	int k,n=0,il=81;                    // puis les z de groupe
	for(i=0;i<18;i++)
		for(j=0;j<3;j++)
		{
			z[il].SetAll_0();
			if(i<9)
				for(k=0;k<3;k++)
					z[il].Set(n++);
			else
			{
				int c=i-9; 
				for(k=0;k<3;k++)
					z[il].Set(c+27*j+9*k);
			}
		il++; 
		}
	ReInit();
}

//------ pour les additions
USHORT ZGROUPE::Charge(const BF81 &ze)
{
	if(ze.IsEmpty())
	{
		parentpuz->Estop( "zgs groupe IsEmpty");
		return 0;
	}
	for(int i=0;i<iz;i++) 
		if(z[i]==ze) 
			return i;
	if(iz<zgs_lim) 
	{
		z[iz++]=ze ;
		return (iz-1);
	}
	parentpuz->Elimite( "ZGS");return 0;
}

//BF81 * CANDIDATE::GetZ() const {
//	return &zgs.z[ig];
//}     
