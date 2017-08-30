#include <cstdio>

typedef unsigned long long uLL;
typedef unsigned int uint;

FILE *KEY, *VER, *USER;
uLL user_Key;

namespace MD5
{
	const uint table[4][16]= 
	{
		{0xD76AA478,0xE8C7B756,0x242070DB,0xC1BDCEEE,0xF57C0FAF,0x4787C62A,0xA8304613,0xFD469501,0x698098D8,0x8B44F7AF,0xFFFF5BB1,0x895CD7BE,0x6B901122,0xFD987193,0xA679438E,0x49B40821},
		{0xF61E2562,0xC040B340,0x265E5A51,0xE9B6C7AA,0xD62F105D,0x02441453,0xD8A1E681,0xE7D3FBC8,0x21E1CDE6,0xC33707D6,0xF4D50D87,0x455A14ED,0xA9E3E905,0xFCEFA3F8,0x676F02D9,0x8D2A4C8A},
		{0xFFFA3942,0x8771F681,0x6D9D6122,0xFDE5380C,0xA4BEEA44,0x4BDECFA9,0xF6BB4B60,0xBEBFBC70,0x289B7EC6,0xEAA127FA,0xD4EF3085,0x04881D05,0xD9D4D039,0xE6DB99E5,0x1FA27CF8,0xC4AC5665},
		{0xF4292244,0x432AFF97,0xAB9423A7,0xFC93A039,0x655B59C3,0x8F0CCC92,0xFFEFF47D,0x85845DD1,0x6FA87E4F,0xFE2CE6E0,0xA3014314,0x4E0811A1,0xF7537E82,0xBD3AF235,0x2AD7D2BB,0xEB86D391}
	};

	const uint Move[4][16]=
	{
		{7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22},
		{5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20},
		{4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23},
		{6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21}
	};

	const uint start[4]={0, 1, 5, 0};

	const uint step[4]={1, 5, 3, 7};

	const uint Result_init[4]={0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476};

	uint result[4], M[16];

	uint calc(uint x, uint y, uint z, uint nid)
	{
		switch (nid)
		{
			case 0:
				return (x&y)|((~x)&z);

			case 1:
				return (x&z)|(y&(~z));

			case 2:
				return x^y^z;

			case 3:
				return y^(x|(~z));
		}
		return -1;
	}

	void fill(uLL data)
	{
		M[0]=data&(0xffff);
		M[1]=(data>>32)&(0xffff);
		M[2]=(0x80);
		for (int i=3; i<14; ++i) M[i]=0;
		M[14]=64;
		M[15]=0;
	}

	void encode()
	{
		for (int i=0; i<4; ++i) result[i]=Result_init[i];
		for (int k=0; k<4; ++k)
			for (int i=0, nid=start[k]; i<16; ++i, nid=(nid+step[k])&(0xf))
			{
				uint tmp=(result[1]+calc(result[1], result[2], result[3], k)+M[nid]+table[k][i]);
				result[0]=result[1]+((tmp&((1uLL<<(32-Move[k][i]))-1))|(tmp>>(32-Move[k][i])));
				tmp=result[3];
				for (int j=1; j<4; ++j) result[j]=result[j-1];
				result[0]=tmp;
			}
		for (int i=0; i<4; ++i) result[i]+=Result_init[i];
	}

	void print()
	{
		VER=fopen("verification.ini", "w");
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				fprintf(VER, "%u", (result[i]>>(j*8))&(0xff));
		fclose(VER);
	}

	void user_code()
	{
		USER=fopen("USER.ini", "w");
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				fprintf(USER, "%u", (result[i]>>(j*8))&(0xff));
		fclose(USER);
	}
}

int main(int argc, char *argv[2])
{
	KEY=fopen("KEY", "r");
	fscanf(KEY, "%I64u", &user_Key);
	fclose(KEY);
	if (argv[1][0]=='0')
	{
		MD5::fill(user_Key);
		MD5::encode();
		MD5::print();
	}
	else
	{
		MD5::fill(user_Key);
		MD5::encode();
		MD5::user_code();
	}
	return 0;
}
