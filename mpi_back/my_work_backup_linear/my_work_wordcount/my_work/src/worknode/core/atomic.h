/*========================================================================
#   FileName: atomic.h
#     Author: wangjian
#      Email: kuangrenyx@qq.com
#    Version: 1.0.0
#   HomePage: http://www.xxxxxxxxx.cn
# LastChange: 2013-10-24 12:12:24
========================================================================*/

/**
 * define __x86_64__ for x86-64
 * define CPU_V9 for sparc
 */
#ifndef ATOMIC_H
#define ATOMIC_H

#include <stdint.h>

static inline void spin_wait(int n)
{
    int    tmp = n;
    while(tmp > 0) { tmp--; asm("" ::: "memory", "cc"); }
}

#define set_and_flush(x,y)    \
    do {                \
        void*    p;        \
        p = &(x);        \
        asm("" ::: "memory");    \
        (x) = (y);        \
        flush(p);        \
    } while (0)

/* a bunch of atomic ops follow... */
#if defined(__x86_64__)

static inline void flush(void* addr) {asm("":::"memory");}

static inline uintptr_t atomic_read(void* addr) { return *((uintptr_t*)addr); }

/* returns zero if already set, returns nonzero if not set */
/* we don't use bts because it is super-slow */
static inline int test_and_set(uintptr_t* n)
{
    uintptr_t    not_set;
    __asm__ __volatile__(
        "xorq        %%rdx, %%rdx    \n"
        "xorq        %%rax, %%rax    \n"
        "incq        %%rdx        \n"
        "lock cmpxchgq    %%rdx, (%1)    \n"
        "decq        %%rax        \n"
    : "=a" (not_set) : "r" (n) : "%rdx");

    return not_set;
}

/* adds 1 to value pointed to in 'n', returns old value */
static inline unsigned int fetch_and_inc(unsigned int* n)
{
    unsigned int    oldval;

    __asm__ __volatile__(
        "movl        $1, %0        \n"
        "lock xaddl    %0, (%1)    \n"
    : "=a" (oldval) : "b" (n));

    return oldval;
}

/* returns true on swap */
static inline int cmp_and_swp(uintptr_t v, uintptr_t* cmper, uintptr_t matcher)
{
    int    swapped;
    __asm__ __volatile__(
        "xorl        %%edx, %%edx    \n"
        "lock cmpxchgq    %2, (%3)    \n"
        "setz        %%dl        \n"
    : "=d" (swapped) : "a" (matcher), "b" (v), "c" (cmper) : "cc");
    return swapped;
}

/**
 * @n - value to store
 * @v - location to store into
 * @return previous value in v
 */
static inline uintptr_t atomic_xchg(uintptr_t n, uintptr_t* v)
{
    __asm__ __volatile__(
        "lock xchgq    %1, (%2)    \n"
    : "=r" (n) : "0" (n), "r" (v));
    return n;
}
#elif (defined(CPU_V9))

static inline unsigned int fetch_and_inc(unsigned int* n)
{
    unsigned int    v, old_v;
    __asm__ __volatile__(
        "1:                    \n"
        "membar    #StoreLoad | #LoadLoad        \n"
        "lduw    [%2], %1            \n"
        "add    %1, 1, %0            \n"
        "cas    [%2], %1, %0            \n"
        "cmp    %1, %0                \n"
        "bne,pn    %icc, 1b            \n"
        " nop                    \n"
        "membar #StoreLoad | #StoreStore    \n"
    : "=r" (v), "=r" (old_v)
    : "r" (n)
    : "cc", "memory");
    return old_v;
}

static inline int cmp_and_swp(uintptr_t v, uintptr_t* cmper, uintptr_t matcher)
{
    int    swapped;
    __asm__ __volatile__(
        "membar    #StoreLoad | #LoadLoad        \n"
        "cas    [%4], %2, %1            \n"
        "cmp    %1, %2                \n"
        "bne,pn    %icc, 1f            \n"
        " mov    1, %0                \n"
        "mov    2, %0                \n"
        "1:                    \n"
        "sub    %0, 1, %0            \n"
        "membar    #StoreLoad | #StoreStore    \n"
    : "=r" (swapped), "=r" (v) 
    : "r" (matcher), "1" (v), "r" (cmper)
    : "memory", "cc");

    return swapped;
}

static inline int test_and_set(uintptr_t* n)
{
    return cmp_and_swp(1, n, 0);
}

static inline uintptr_t atomic_xchg(uintptr_t n, uintptr_t* v)
{
    uintptr_t    swped;

    __asm__ __volatile(
        "1:                    \n"
        "membar    #StoreLoad | #LoadLoad        \n"
        "ld    [%3], %0            \n"
        "cas    [%3], %0, %1            \n"
        "cmp    %0, %1                \n"
        "bne,pn    %icc, 1b            \n"
        " mov    %4, %1                \n"
        "membar #StoreLoad | #StoreStore    \n"
    : "=r" (swped), "=r" (n) 
    : "1" (n), "r" (v), "r" (n)
    : "cc", "memory");

    return swped;
}

static inline void flush(void* addr)
{
//    __asm__ __volatile__("flush %0\n" :: "r" (addr) : "memory");
    __asm__ __volatile__("" ::: "memory");
}

static inline uintptr_t atomic_read(void* addr)
{
    uintptr_t    v;
    __asm__ __volatile__(
        "membar #StoreLoad | #LoadLoad        \n"
        "ld    [%1], %0            \n"
    : "=r" (v) : "r" (addr));
    return v;
}

#elif (defined(__x86__))

static inline void flush(void* addr) {asm("":::"memory");}

static inline uintptr_t atomic_read(void* addr) { return *((uintptr_t*)addr); }

/* returns zero if already set, returns nonzero if not set */
/* we don't use bts because it is super-slow */
static inline int test_and_set(uintptr_t* n)
{
    uintptr_t    not_set;
    __asm__ __volatile__(
        "xorq        %%rdx, %%rdx    \n"
        "xorq        %%rax, %%rax    \n"
        "incq        %%rdx        \n"
        "lock cmpxchgq    %%rdx, (%1)    \n"
        "decq        %%rax        \n"
    : "=a" (not_set) : "r" (n) : "%rdx");

    return not_set;
}

/* adds 1 to value pointed to in 'n', returns old value */
static inline unsigned int fetch_and_inc(unsigned int* n)
{
    unsigned int    oldval;

    __asm__ __volatile__(
        "movl        $1, %0        \n"
        "lock xaddl    %0, (%1)    \n"
    : "=a" (oldval) : "b" (n));

    return oldval;
}

/* returns true on swap */
static inline int cmp_and_swp(uintptr_t v, uintptr_t* cmper, uintptr_t matcher)
{
    int    swapped;
    __asm__ __volatile__(
        "xorl        %%edx, %%edx    \n"
        "lock cmpxchgq    %2, (%3)    \n"
        "setz        %%dl        \n"
    : "=d" (swapped) : "a" (matcher), "b" (v), "c" (cmper) : "cc");
    return swapped;
}

/**
 * @n - value to store
 * @v - location to store into
 * @return previous value in v
 */
static inline uintptr_t atomic_xchg(uintptr_t n, uintptr_t* v)
{
    __asm__ __volatile__(
        "lock xchgq    %1, (%2)    \n"
    : "=r" (n) : "0" (n), "r" (v));
    return n;
}

#elif (defined(__ARM__))

static inline void flush(void* addr) {asm("":::"memory");}

static inline uintptr_t atomic_read(void* addr) { return *((uintptr_t*)addr); }

/* returns zero if already set, returns nonzero if not set */
/* we don't use bts because it is super-slow */
static inline int test_and_set(uintptr_t* n)
{
#if 0
    uintptr_t    not_set;
    __asm__ __volatile__(
        "xorq        %%rdx, %%rdx    \n"
        "xorq        %%rax, %%rax    \n"
        "incq        %%rdx        \n"
        "lock cmpxchgq    %%rdx, (%1)    \n"
        "decq        %%rax        \n"
    : "=a" (not_set) : "r" (n) : "%rdx");


    return not_set;
#endif

	if(*n == 1)
	{
		return	1;
	}
	else
	{
		return 0;
	}

}

/* adds 1 to value pointed to in 'n', returns old value */
static inline unsigned int fetch_and_inc(unsigned int* n)
{
    unsigned int    oldval;

#if 0
    __asm__ __volatile__(
        "movl        $1, %0        \n"
        "lock xaddl    %0, (%1)    \n"
    : "=a" (oldval) : "b" (n));

    return oldval;
#endif

	oldval = *n;
	(*n)++;

	return oldval;
}

#if 0
/* returns true on swap */
static inline int cmp_and_swp(uintptr_t v, uintptr_t* cmper, uintptr_t matcher)
{
    int    swapped;
#if 0
    __asm__ __volatile__(
        "xorl        %%edx, %%edx    \n"
        "lock cmpxchgq    %2, (%3)    \n"
        "setz        %%dl        \n"
    : "=d" (swapped) : "a" (matcher), "b" (v), "c" (cmper) : "cc");
    return swapped;
#endif
	
}
#endif

/**
 * @n - value to store
 * @v - location to store into
 * @return previous value in v
 */
static inline uintptr_t atomic_xchg(uintptr_t n, uintptr_t* v)
{
#if 0
    __asm__ __volatile__(
        "lock xchgq    %1, (%2)    \n"
    : "=r" (n) : "0" (n), "r" (v));
    return n;
#endif
	int chg;
	chg = *v;
	*v = n;
	return chg;
}

#else
#error unknown arch
#endif
#endif

// vim: ts=8 sw=4 sts=4 smarttab smartindent
