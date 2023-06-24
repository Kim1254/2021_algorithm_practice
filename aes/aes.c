#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#pragma warning(disable:4996)

#if defined _DEBUG
#define assertion(expression) if (!(expression)) { printf("assertion: "); printf(#expression); printf("\n"); exit(0); }
#else
#define assertion(expression) (void*)0
#endif

typedef struct egcd_s
{
	int d;
	int x;
	int y;
} egcd_t;

int euclid(int a, int b)
{
	int tmp;

	if (a < b) // a > b
	{
		tmp = a;
		a = b;
		b = tmp;
	}

	while (b) // gcd(a, b) = gcd(b, a % b)
	{
		tmp = a % b;
		a = b;
		b = tmp;
	}
	return a;
}

// Extended euclidean algorithm
//
// d = ax + by
// x = s, y = t
//
egcd_t eeuclid(int a, int b)
{
	egcd_t ret = { 0, 1, 0 };

	// this, next
	int s0 = 1, s1 = 0;
	int t0 = 0, t1 = 1;

	int tmp, q;

	if (a < b) // a > b
	{
		tmp = a;
		a = b;
		b = tmp;
	}

	while (b)
	{
		q = a / b; // quotient

		tmp = a; // gcd(a, b) = gcd(b, a % b)
		a = b;
		b = tmp % b;

		tmp = s0;
		s0 = s1;
		s1 = tmp - s1 * q;

		tmp = t0;
		t0 = t1;
		t1 = tmp - t1 * q;
	}

	ret.d = a;
	ret.x = s0;
	ret.y = t0;

	return ret;
}

int isPrime(int n)
{
	// check for range (2 - sqrt(n)), avoid repeating 1 and numbers large than sqrt(n) cannot be factor of n.
	for (int i = 2; i <= sqrt(n); i++)
	{
		if (!(n % i))
			return 0;
	}
	return 1;
}

// A prime factorization function
// 
// N = p * q = ((p + q) / 2)^2 - ((p - q) / 2)^2
// a = (p + q) / 2
// b = (p - q) / 2
//
// N = a^2 - b^2
// a + b = p
// a - b = q
int fermat(int n)
{
	// Find an integer that is same appropriately bigger than sqrt(n).
	int a = ceil(sqrt(n));

	// b^2 = a^2 - x
	int doubleb = a * a - n;

	// the reason rounding: for making it clear of b^2 != doubleb, it uses round instead of ceil and floor
	int b = round(sqrt(doubleb));

	// Loop this sequence until we find a,b satisfy 'x = a^2 - b^2'
	while (b * b != doubleb)
	{
		a++;
		doubleb = a * a - n;
		b = round(sqrt(doubleb));
	}
	return a + b;
}

// When n = p * q and p, q are relatively prime numbers,
// pi(n) = (p - 1) * (q - 1)
int euler(int n)
{
	int p = fermat(n);
	int q = n / p;

	return (p - 1) * (q - 1);
}

// Modular function for huge integer
// f(0) = 1
// N % M = K ... f(1)
// (N * N) % M = ((N % M) * (N % M)) % M = K^2 ... f(2)
// (N * N * N) % M = ((N % M) * (N % M) * (N % M)) % M = K^3 = f(2)*f(1) ... f(3)
// f(n) = (N * ... * N) % M = ((N % M) * ... * (N % M)) % M = f(n - 1) * K
int powmod(int x, int y, int m)
{
	int r = 1; // f(0)

	for (int i = 0; i < y; i++) // repeat for y times
	{
		r *= x; // * N
		r %= m; // % M
	}

	return r;
}

int main()
{
	printf("Enter public key(format: e, n, c):");

	int e, n, c;
	scanf("%d, %d, %d", &e, &n, &c);

	assertion(!isPrime(n)); // N should be a composite number
	assertion(e > 0);

	// Since we already checked that n is a prime number(n = pq, p and q is prime number).
	// We uses it euler function without checks.
	int pi = euler(n);

	assertion(euclid(e, pi) == 1); // e, pi are relatively prime numbers.

	// e * d = 1 (mod pi)
	// We can get d by using extended euclid(pi, e)
	egcd_t res = eeuclid(pi, e);

	int d = res.y; // d = t
	if (d < 0) // add pi when t < 0
		d += pi;

	printf("m: %d", powmod(c, d, n));

	return 0;
}
