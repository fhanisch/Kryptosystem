#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <intrin.h>

//#pragma intrinsic(_umul128)

unsigned int isPrime(unsigned int x)
{
	for (unsigned int i = 2; i <= sqrt((double)x); i++)
	{
		if (x % i == 0) return 0;
	}
	return 1;
}

unsigned int getPrime(unsigned int x)
{
	while (!isPrime(x))
	{
		x++;
	}
	return x;
}

unsigned int calcInverse(unsigned int e, unsigned int phi)
{
	unsigned long long k = 0;
	while (1)
	{
		k++;
		if ((1 + k * (unsigned long long)phi) % (unsigned long long)e == 0) return (unsigned int)((1 + k * (unsigned long long)phi) / (unsigned long long)e);
	}
}

unsigned int crypt(unsigned int x, unsigned int key, unsigned int N)
{
	unsigned long long y = 1;
	for (unsigned int i = 0; i < key; i++)
	{
		y = y * x;
		y = y % N;
	}
	return (unsigned int)y;
}

unsigned int crypt2(unsigned int x, unsigned int key, unsigned int N)
{
	unsigned long long y = 1;
	unsigned int test = 1 << 31;
	for (unsigned int i = 0; i < 32; i++)
	{
		y = y * y;
		y = y % N;
		if (key & (test >> i))
		{
			y = y * x;
			y = y % N;
		}
	}
	return (unsigned int)y;
}

unsigned long long binMult64(unsigned int a, unsigned int  b)
{
	int bits = 32;
	unsigned int y[] = { 0,0 };
	unsigned int test = 1 << (bits-1);
	unsigned int tmp;
	for (int i = 0; i < bits; i++)
	{
		if (b & (test >> i))
		{
			tmp = y[0];
			y[0] += a << ((bits-1) - i);
			if (y[0] < tmp) y[1]++;
			y[1] += a >> (i+1);
		}
	}
	return *((unsigned long long*)y);
}

unsigned int main(unsigned int argc, char* argv[])
{
	printf("RSA-Test\n\n");
	char msg[] = "Hallo";
	printf("Character c = %c\n", msg[0]);
	printf("Character c = %u\n", msg[0]);

	unsigned int test = 1 << 31;
	printf("test = %x\n", test>>31);

	printf("y = %llu\n", binMult64(2376876375, 4198724862));

	unsigned __int64 m1 = 0xffffffffffffffffI64, m2 = 0xffffffffffffffffI64, pH, pL;
	pL = _umul128(m1, m2, &pH);
	printf("%llu%llu\n", pH, pL);
	printf_s("%#I64x * %#I64x = %#I64x%I64x\n", m1, m2, pH, pL);
	exit(1);
	//unsigned int p = 11;
	unsigned int p = getPrime(64000);
	printf("Primzahl p = %u\n", p);
	if (!isPrime(p))
	{
		printf("p ist keine Primzahl!\n");
		exit(1);
	}

	//unsigned int q = 13;
	unsigned int q = getPrime(66000);
	printf("Primzahl q = %u\n", q);
	if (!isPrime(q))
	{
		printf("q ist keine Primzahl!\n");
		exit(1);
	}

	unsigned int N = p * q;
	printf("N = %u\n", N);
	
	//exit(0);

	unsigned int phi = (p - 1) * (q - 1);
	printf("phi(N) = %u\n", phi);
	
	unsigned int e, d, c;
	for (unsigned int i = 2; i < phi; i++)
	{
		if (phi % i == 0) continue;
		if (isPrime(i) && i != p && i != q)
		{
			e = i;
			printf("e = %u",e);
			d = calcInverse(e, phi);
			if (d)
			{
				printf(" ; d = %u", d);
				c = crypt2(msg[0] * 3 + 15, e, N);
				printf(" | c = %u", c);
				printf(" ; m = %u\n", (crypt2(c, d, N) - 15) / 3);
			}
		}
		if (e >= 5000) break;
	}

	unsigned int i = 0;
	unsigned int* cmsg = malloc(strlen(msg) * sizeof(unsigned int));
	while (msg[i])
	{
		cmsg[i] = crypt2(msg[i] * 3 + 15, e, N);
		i++;
	}

	int m;
	time_t t_start = clock();
	for (int i = 0; i < pow(2, 32); i++)
	{
		m = (crypt2(c, i, N) - 15) / 3;
		if (m == msg[0])
		{
			printf("d = %u\n", i);
			break;
		}
	}
	time_t t_end = (clock() - t_start)/1000;
	printf("Elapsed Time = %llu min %llu s\n", t_end / 60, t_end % 60);
	printf("Fertsch.\n");

	return 0;
}