# -*- coding: utf-8 -*-
#
# This script makes emdf_primes.cpp and emdf_primes.h.
#
# These files meant as an aid to making hash tables.
#
# This script is in the Public Domain, as are its output files.
#
# Written by Ulrik Sandborg-Petersen
#
# Created: 10/19-2015
# Last update: 10/19-2015
#

import sys
import math

boilerplate = """/*
 *
 * Primes list suitable for making hash tables.
 *
 * DO NOT EDIT!  
 * This file was created with make_primes_list.py. 
 * Edit that script to alter this file.
 *
 * This file is in the Public Domain.
 *
 */
"""

# Sieve of Atkin
#
# Ulrik's implementation, based on the pseudocode in the
# WikiPedia article on Sieve of Atkin.
#
def get_prime_list(limit):
    s = [1, 7, 11, 13, 17, 19, 23, 29,
         31, 37, 41, 43, 47, 49, 53, 59]
    s_set = set(s)

    s_3_1 = set([1, 13, 17, 29, 37, 41, 49, 53])
    s_3_2 = set([7, 19, 31, 43])
    s_3_3 = set([11, 23, 47, 59])
    

    is_prime = {}
    
    for w in range(0, (limit/60)+1):
        for x in s:
            n = 60 * w + x
            is_prime[n] = False

    limit_sqrt = int(math.sqrt(limit))

    for x in range(1, limit_sqrt):
        if x % 500 == 1:
            sys.stderr.write(".")
        for y in range(1, limit_sqrt):
            # 3.1
            if y % 2 == 0:
                n = 4*x*x + y*y
                if n <= limit and n % 60 in s_3_1:
                    is_prime[n] = not is_prime[n]

            # 3.2
            if x % 2 == 1 and y % 2 == 0:
                n = 3*x*x + y*y
                if n <= limit and n % 60 in s_3_2:
                    is_prime[n] = not is_prime[n]

            # 3.3
            if x >= 2 and y % 2 == 1 and y <= x-1:
                n = 3*x*x - y*y
                if n >= 1 and n <= limit and n % 60 in s_3_3:
                    is_prime[n] = not is_prime[n]

    # Eliminate composites by sieving
    for w in range(0, (limit_sqrt+1)/60):
        for x in s_set:
            n = 60 * w + x
            if n < 7:
                continue
            n_squared = n*n
            if n_squared > limit:
                continue
            if is_prime[n]:
                for w2 in range(0, limit_sqrt):
                    for x2 in s_set:
                        c = n_squared * (60 * w2 + x2)
                        if c <= limit:
                            is_prime[c] = False

    sys.stderr.write("!\n\n")

    sys.stderr.write("Now reducing list of primes...")
                            
    result = []

    prev_n = 0
    for n in sorted(is_prime):
        if is_prime[n]:
            if n <= 3:
                pass
            elif n < 23:
                result.append(n)
                prev_n = n
            elif 1.25*prev_n < n:
                result.append(n)
                prev_n = n

    sys.stderr.write("\nDone!\n")
                
    return result



def write_primes_cpp(prime_list, outfilename):
    fout = open(outfilename, "wb")

    fout.write("""%(boilerplate)s

const int nNumberOfPrimes = %(prime_list_length)d;

const long g_arr_prime_list[%(prime_list_length)d] = {
%(prime_list_as_string)s
};

/** Get the next prime larger than or equal to n in the list of primes.
 *
 * If n is greater than the largest prime in the list,
 * then fake it by returning n * 2 + 1.
 *
 * When doubling the size of a hash table, the idea is to pass size*2 to 
 * get the new size, alternatively to pass size*1.5 to get the new size.
 *
 * @param n The lower bound to find.
 *
 * @return The lowest prime which is >= n in the list.
 *
 */
long prime_list_get_next_higher_prime(long n)
{
    /* Lower bound search on prime >= n.
     * Finds the least prime a for which a >= n.
     *
     * if n > the largest prime in the table,
     * fake it by returning n * 2 + 1.
     */
    int high = nNumberOfPrimes - 1;
    int low = 0;
    int mid = 0;
    long a = 0;
        

    while (low < high) {
        mid = low + ((high - low) / 2);

        a = g_arr_prime_list[mid];

        if (a >= n) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }

    if ((low > (nNumberOfPrimes - 1))
        || ((a = g_arr_prime_list[low]) < n)) {
        /* 
         * n is larger than the largest prime in the table, 
         * so fake it instead.
         */
        return n*2 + 1;
    } else {
        return a;
    }
}
""" % {
    "prime_list_length" : len(prime_list),
    "prime_list_as_string" : ", ".join([str(x) for x in prime_list]),
    "boilerplate" : boilerplate,
})

    fout.close()

def write_primes_h(outfilename):
    fout = open(outfilename, "wb")

    fout.write("""%(boilerplate)s
#ifndef EMDF_PRIMES_H_
#define EMDF_PRIMES_H_

extern long prime_list_get_next_higher_prime(long n);

#endif /* if !defined(EMDF_PRIMES_H_) */
""" % {
    "boilerplate" : boilerplate,
    })
    fout.close()
    
               
def doIt(limit):
    prime_list = get_prime_list(limit)

    base_name = "emdf_primes"

    write_primes_cpp(prime_list, "%s.cpp" % base_name)
    write_primes_h("../include/%s.h" % base_name)

# Just for testing...
#limit = 10000

# 16 Gigabytes of memory, divided by the expected number of bytes per entry
limit = (16*1024*1024*1024)/64

print "limit =", limit

doIt(limit)


