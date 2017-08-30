#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <io.h>
using namespace std;

//#define Debug

typedef unsigned long long uLL;

FILE *plain, *secret, *bill, *KEY, *debug;
uLL user_Key;

namespace DES
{
	const int turns=17;

	const uLL Kchange[56]={57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36, 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};
	
	const uLL SAL[16]={1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1}; 

	const uLL Kmerge[48]={14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

	const uLL change_table[64]={58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4, 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8, 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3, 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

	const uLL extend_change[48]={32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9, 8, 9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17, 16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25, 24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1};

	const uLL Bchange[32]={16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18, 31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22, 11, 4, 25};
	
	const uLL S[8][4][16]={
    {
    	{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7}, 
	    {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8}, 
		{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0}, 
	    {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
	},
	{
    	{15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
    	{3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5}, 
    	{0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15}, 
    	{13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {
    	{10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
    	{13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1}, 
    	{13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7}, 
    	{1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {
    	{7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
	    {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9}, 
	    {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4}, 
	    {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14} 
	},
	{
	    {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9}, 
	    {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6}, 
	    {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14}, 
	    {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}, 
	},
	{
	    {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11}, 
	    {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8}, 
	    {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6}, 
	    {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13} 
	},
	{
	    {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1}, 
	    {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6}, 
	    {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2}, 
	    {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12} 
	},
	{
	    {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7}, 
	    {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2}, 
	    {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8}, 
	    {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11},
	}};

	uLL Kc[turns], Kd[turns], K[turns];
	uLL L[turns], R[turns];

	void init()
	{
		memset(Kc, 0, sizeof(Kc));
		memset(Kd, 0, sizeof(Kd));
		memset(K, 0, sizeof(K));
		memset(L, 0, sizeof(L));
		memset(R, 0, sizeof(R));
	}

	void calcK(uLL key)
	{
		memset(Kc, 0, sizeof(Kc));
		memset(Kd, 0, sizeof(Kd));
		memset(K, 0, sizeof(K));
		for (int i=0; i<28; ++i)
			Kc[0]|=((key>>(Kchange[i]-1)) & 1uLL)<<i;
		for (int i=28; i<56; ++i)
			Kd[0]|=((key>>(Kchange[i]-1)) & 1uLL)<<(i-28);
		for (int i=1; i<=16; ++i)
		{
			Kc[i]=((Kc[i-1]&((1uLL<<(28-SAL[i-1]))-1))<<SAL[i-1])|(Kc[i-1]>>(28-SAL[i-1]));
			Kd[i]=((Kd[i-1]&((1uLL<<(28-SAL[i-1]))-1))<<SAL[i-1])|(Kd[i-1]>>(28-SAL[i-1]));
			uLL Ktmp=Kc[i]|(Kd[i]<<28);
			for (int j=0; j<48; ++j)
				K[i]|=((Ktmp>>(Kmerge[j]-1)) & 1uLL)<<j;
		}
	}

	uLL extend(uLL num)
	{
		uLL code=0;
		for (int i=0; i<48; ++i)
			code|=((num>>(extend_change[i]-1)) & 1uLL)<<i;
		return code;
	}

	uLL plain_to_secret(uLL plain_text)
	{
		memset(L, 0, sizeof(L));
		memset(R, 0, sizeof(R));
		for (int i=0; i<32; ++i)
			L[0]|=((plain_text>>(change_table[i]-1)) & 1uLL)<<i;
		for (int i=32; i<64; ++i)
			R[0]|=((plain_text>>(change_table[i]-1)) & 1uLL)<<(i-32);
		for (int i=1; i<=16; ++i)
		{
			uLL ecode=extend(R[i-1]);
			ecode^=K[i];
			uLL tcode=0;
			for (int j=0; j<8; ++j)
			{
				uLL num=ecode&(uLL(0x3f));
				ecode>>=6;
				tcode|=S[j][num&(uLL(0x21))][num&(uLL(0x1e))]<<(j*4);
			}
			for (int j=0; j<32; ++j)
				R[i]|=(((tcode>>(Bchange[j]-1)) & 1uLL)^((L[i-1]>>j) & 1uLL))<<j;
			L[i]=R[i-1];
		}
#ifdef Debug
		for (int i=0; i<=16; ++i)
			fprintf(debug, "%I64u %I64u\n", L[i], R[i]);
		fprintf(debug, "\n");
#endif
		uLL cipher_text=0, tcode=(L[16]<<32) | R[16];
		for (int i=0; i<64; ++i)
			cipher_text|=((tcode>>i) & 1uLL)<<(change_table[i]-1);
		return cipher_text;
	}

	uLL secret_to_plain(uLL cipher_text)
	{
		memset(L, 0, sizeof(L));
		memset(R, 0, sizeof(R));
		for (int i=0; i<32; ++i)
			L[16]|=((cipher_text>>(change_table[i]-1)) & 1uLL)<<i;
		for (int i=32; i<64; ++i)
			R[16]|=((cipher_text>>(change_table[i]-1)) & 1uLL)<<(i-32);
		for (int i=16; i>0; --i)
		{
			uLL ecode=extend(R[i]);
#ifdef Debug
		fprintf(debug, "%I64u %I64u\n", L[i], R[i]);
		fprintf(debug, "%I64u\n", ecode);
		fprintf(debug, "%I64u\n", K[i]);
#endif
			ecode^=K[i];
			uLL tcode=0;
			for (int j=0; j<8; ++j)
			{
				uLL num=ecode&(uLL(0x3f));
				ecode>>=6;
				tcode|=S[j][num&(uLL(0x21))][num&(uLL(0x1e))]<<(j*4);
			}
			for (int j=0; j<32; ++j)
				R[i-1]|=(((tcode>>(Bchange[j]-1)) & 1uLL)^((L[i]>>j) & 1uLL))<<j;
			L[i-1]=R[i];
		}

		uLL plain_text=0, tcode=(L[0]<<32) | R[0];
		for (int i=0; i<64; ++i)
			plain_text|=((tcode>>i) & 1uLL)<<(change_table[i]-1);
		return plain_text;
	}
}

void check_Key()
{
	system("MD5.exe 1");
	FILE *VER=fopen("verification.ini", "r");
	FILE *USER=fopen("USER.ini", "r");
	while (1)
	{
		char chv=getc(VER), chu=getc(USER);
		if (chv==EOF && chu==EOF) break;
		if (chv!=chu)
		{
			printf("KEY is wrong.");
			Sleep(1000);
			fclose(VER);
			fclose(USER);
			system("del USER.ini");
			exit(0);
		}
	}
	fclose(VER);
	fclose(USER);
	system("del USER.ini");
}

void get_key(int mode)
{
	while ((KEY=fopen("KEY", "r"))==NULL)
	{
		printf("The KEY dosen't exist\n");
		printf("Please confirm that you have put the right KEY in this path\n");
		printf("Hit any key to continue...\n");
		getchar();
	}
	fscanf(KEY, "%I64u", &user_Key);
	if (mode==0) check_Key();
	else system("MD5.exe 0");
	DES::calcK(user_Key);
	fclose(KEY);
}
void plain_to_secret()
{
	secret=fopen("secret.txt", "a+");
	fprintf(secret, "#");
	while (1)
	{
		uLL text=0;
		bool done=false;
		char ch;
		if ((ch=getc(plain))==EOF) break;
		text=uLL(ch);
		for (int i=1; i<8; ++i)
		{
			if ((ch=getc(plain))==EOF) { done=true; break; }
			text|=(uLL(ch))<<(i*8);
		}
		uLL code=DES::plain_to_secret(text);
		for (int i=63; i>=0; --i) 
			fprintf(secret, "%I64u", ((code>>i) & 1uLL));
		if (done) break;
	}
	fclose(secret);
}
void secret_to_plain()
{
	bill=fopen("bill.txt", "w");
	while (1)
	{
		uLL code=0;
		char ch=getc(secret);
		if (ch==EOF) break;
		if (ch=='#') continue;
		code=uLL(ch-'0');
		for (int i=62; i>=0; --i)
			code=(code<<1)|(uLL(getc(secret)-'0'));
		uLL text=DES::secret_to_plain(code);
		for (int i=0; i<8; ++i)
		{
			if (text==0) break;
			putc(text&(uLL(0xff)), bill);
			text>>=8;
		}
	}
	fclose(bill);
}
int getnum(int sum)
{
	string st;
	getline(cin, st);
	while (st.length()!=1 || atoi(st.c_str())==0 || atoi(st.c_str())>sum)
	{
		printf("The order doesn't exist.\n");
		printf("Please enter the right number.\n");
		Sleep(500);
		getline(cin, st);
	}
	return atoi(st.c_str());
}
void DOS_to_TXT()
{
	printf("Please write your bills.(End with an empty line.)\n");
	string st;
	while (1)
	{
		getline(cin, st);
		if (st=="") break;
		fprintf(plain, "%s\n", st.c_str());
	}
}
void add_bills()
{
	get_key(0);
	printf("Please choose the way you like to add the bills.\n");
	printf("1.newbills.txt.    2.MS-DOS.\n");
	int nid=getnum(2);
	if (nid==2)
	{
		plain=fopen("newbills.txt", "w");
		DOS_to_TXT();
		fclose(plain);
	}
	plain=fopen("newbills.txt", "r");
	plain_to_secret();
	fclose(plain);
	printf("Add successfully.\n");
	system("del newbills.txt");
}
void check_bills()
{
	get_key(0);
	secret=fopen("secret.txt", "r");
	secret_to_plain();
	system(".\\bill.txt");
	system("del bill.txt");
}
void change_Key()
{
	get_key(0);
	secret=fopen("secret.txt", "r");
	secret_to_plain();
	fclose(secret);
	while ((_access("newbills.txt", 0))!=-1)
	{
		printf("Please remove the newbills.txt\n");
		printf("Hit any key to continue\n");
		getchar();
	}
	system("ren bill.txt newbills.txt");
	printf("Please change the Key\n");
	Sleep(5000);
	printf("After you changed the Key, hit any key to continue\n");
	getchar();
	get_key(1);
	plain=fopen("newbills.txt", "r");
	plain_to_secret();
	fclose(plain);
	system("del newbills.txt");
	printf("Change successfully");
	Sleep(1000);
}
int main()
{
#ifdef Debug
	debug=fopen("debug.txt", "w");
#endif

	bool flag=true;
	if (flag) user_Key=1;
	user_Key=flag^1;
	printf("Welcome to use our audit system\n");
	while (1)
	{
		printf("What do you want to do next?\n");
		printf("1.Add bills.    2.Check the bills.    3.Change KEY.    4.Exit\n");
		int nid=getnum(4);
		bool done=false;
		switch (nid)
		{
			case 1:
				add_bills();
				break;
			case 2:
				check_bills();
				break;
			case 3:
				change_Key();
				break;
			case 4:
				done=true;
				break;
		}
		if (done) break;
	}
	printf("Thanks for your using.\n");
	Sleep(500);
	return 0;
}
