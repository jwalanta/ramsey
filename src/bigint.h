#include <gmpxx.h>

#ifndef BIGINT_H
#define BIGINT_H

///
/// -- IMPORTANT!!! ---
/// 
/// Comment or uncomment below to determine data type to use
/// 

// gcc builtin uint128 for 64-bit processors
//
//typedef __uint128_t BIGINT;

//
// unsigned long long
//
//typedef unsigned long long BIGINT;

// 
// GMP Library
//
// (Uncomment two lines below to enable gmp)
//
typedef mpz_class BIGINT;
#define MPZ_BIGINT 1

// max vertex that can be possibly used
#define MAX_VERTEX 64

// precision for lhs part, PRECISION x 64 bits
#define PRECISION 1

// for nauty.h
#define MAXN 64

#endif