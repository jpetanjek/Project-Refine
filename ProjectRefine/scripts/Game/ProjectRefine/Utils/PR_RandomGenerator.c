class PR_RandomGenerator
{
	int m_iValue;
	
	void Init(int value)
	{
		m_iValue = value;
		
		// Shift it many times to scramble values
		for (int i = 0; i < 173; i++)
			GetNextInt();
	}
	
	int GetNextInt()
	{
		// https://en.wikipedia.org/wiki/Linear-feedback_shift_register
		// 32,22,2,1 from Xilinx XAPP 052 July 7,1996 (Version 1.1)
		int lfsr = m_iValue;
		int bit = ((lfsr >> 1) ^ (lfsr >> 4)) & 1;
        lfsr = (lfsr >> 1) | (bit << 30);
		m_iValue = lfsr;
		return lfsr;
	}
	
	int GetRandomIndex(int size)
	{
		int a = GetNextInt();
		a = (a & 0x7FFFFFFF) % (size);
		return a;
	}
}