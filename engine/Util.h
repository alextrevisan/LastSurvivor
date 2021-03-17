#ifndef _UTIL_H_
#define _UTIL_H_

#define min2(a,b) ((a<b)?a:b)
#define max2(a,b) ((a>b)?a:b)

template<typename T>
int min(T a,T b) { return (a<b?a:b);}

template<typename T>
int min(T a,T b, T c) { return min(a,min(b,c));}

template<typename T>
int max(T a,T b) { return (a>b?a:b);}   

template<typename T>
int max(T a,T b, T c) { return max(a,max(b,c));}

#endif