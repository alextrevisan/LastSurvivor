#ifndef _NOISE_GENERATOR_H_
#define _NOISE_GENERATOR_H_
struct NoiseGenerator
{
    static int Hash32Shift(int key)
    {
        key = ~key + (key << 15); // key = (key << 15) - key - 1;
        key = key ^ (key >> 12);
        key = key + (key << 2);
        key = key ^ (key >> 4);
        key = key * 2057; // key = (key + (key << 3)) + (key << 11);
        key = key ^ (key >> 16);
        return key;
    }

    static int Generate(int x, int y, int seed)
    {
        int part1 = x + Hash32Shift(y);
        return Hash32Shift(seed + part1);
    }
};


#endif //_NOISE_GENERATOR_H_