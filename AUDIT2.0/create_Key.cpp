#include <string>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <time.h>
#include <Windows.h>
using namespace std;

int main()
{
	LARGE_INTEGER  t;
    QueryPerformanceCounter(&t) ;	 
	srand(t.QuadPart);

	unsigned long long code=0;
	for (int i=0; i<8; ++i)
		code=(code<<8) | (rand() & (0xff));
	printf("%I64u\n", code);
	system("pause");
	return 0;
}
