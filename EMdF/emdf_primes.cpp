/*
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


const int nNumberOfPrimes = 71;

const long g_arr_prime_list[71] = {
7, 11, 19, 31, 43, 67, 103, 139, 191, 251, 331, 419, 547, 691, 883, 1123, 1423, 1783, 2239, 2803, 3511, 4423, 5543, 6967, 8719, 10903, 13687, 17111, 21391, 26807, 33527, 41911, 52463, 65587, 82003, 102523, 128203, 160387, 200491, 250619, 313343, 391679, 489623, 612043, 765091, 956371, 1195483, 1494359, 1867951, 2334959, 2918719, 3648419, 4560547, 5700691, 7125887, 8907391, 11134307, 13917907, 17397427, 21746807, 27183511, 33979471, 42474343, 53092931, 66366179, 82957783, 103697267, 129621587, 162026999, 202533791, 253167239
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
