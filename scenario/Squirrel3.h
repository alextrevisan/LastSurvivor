//Based on https://github.com/sublee/squirrel3-python/blob/master/squirrel3.py

#ifndef _SQUIRREL3_H_
#define _SQUIRREL3_H_

struct Squirrel3Random
{
    static int generate(const int value, const int seed = 0)
    {
        /*Returns an unsigned integer containing 32 reasonably-well-scrambled
        bits, based on a given (signed) integer input parameter `n` and optional
        `seed`.  Kind of like looking up a value in an infinitely large
        non-existent table of previously generated random numbers.
        */
        int n = value;
        n *= NOISE1;
        n += seed;
        n ^= n >> 8;
        n += NOISE2;
        n ^= n << 8;
        n *= NOISE3;
        n ^= n >> 8;
        // Cast into uint32 like the original `Squirrel3`.
        return n % CAP;
    }

    static int generate(const int x, const int y, const int seed)
    {
        const int part1 = generate(x, seed);
        return generate(part1 + y, seed);
    }

    // The base bit-noise constants were crafted to have distinctive and interesting
    // bits, and have so far produced excellent experimental test results.
    static constexpr auto NOISE1 = 0xb5297a4d;  // 0b0110'1000'1110'0011'0001'1101'1010'0100
    static constexpr auto NOISE2 = 0x68e31da4;  // 0b1011'0101'0010'1001'0111'1010'0100'1101
    static constexpr auto NOISE3 = 0x1b56c4e9;  // 0b0001'1011'0101'0110'1100'0100'1110'1001
    static constexpr auto CAP    = 1 << 32;
};




#endif //_SQUIRREL3_H_