#ifndef _FAST_DRAW_FUNCTIONS_H_
#define _FAST_DRAW_FUNCTIONS_H_
#include <psxgte.h>

inline void gte_ldv3_f(const VECTOR& v0, const VECTOR& v1, const VECTOR& v2)
{
    const int xy00 = ((v0.vx&0xFFFF)|(v0.vy<<16));
    const int xy01 = ((v1.vx&0xFFFF)|(v1.vy<<16));
    const int xy10 = ((v2.vx&0xFFFF)|(v2.vy<<16));
    const int z00 = v0.vz;
    const int z01 = v1.vz;
    const int z10 = v2.vz;

    asm volatile ("mtc2 %0, $0\n" :  : "r"(xy00): );
    asm volatile ("mtc2 %0, $1\n" :  : "r"(z00): );
    asm volatile ("mtc2 %0, $2\n" :  : "r"(xy01) : );
    asm volatile ("mtc2 %0, $3\n" :  : "r"(z01): );
    asm volatile ("mtc2 %0, $4\n" :  : "r"(xy10): );
    asm volatile ("mtc2 %0, $5\n" :  : "r"(z10): );
}

inline void gte_ldv3_f(const SVECTOR& v0, const SVECTOR& v1, const SVECTOR& v2)
{
    const int xy00 = ((v0.vx&0xFFFF)|(v0.vy<<16));
    const int xy01 = ((v1.vx&0xFFFF)|(v1.vy<<16));
    const int xy10 = ((v2.vx&0xFFFF)|(v2.vy<<16));
    const int z00 = v0.vz;
    const int z01 = v1.vz;
    const int z10 = v2.vz;

    asm volatile ("mtc2 %0, $0\n" :  : "r"(xy00): );
    asm volatile ("mtc2 %0, $1\n" :  : "r"(z00): );
    asm volatile ("mtc2 %0, $2\n" :  : "r"(xy01) : );
    asm volatile ("mtc2 %0, $3\n" :  : "r"(z01): );
    asm volatile ("mtc2 %0, $4\n" :  : "r"(xy10): );
    asm volatile ("mtc2 %0, $5\n" :  : "r"(z10): );
}


inline void gte_ldv0_f(const VECTOR& v0)
{
    int xy11 = ((v0.vx&0xFFFF)|(v0.vy<<16));
    int z11 = v0.vz;
    asm volatile ("mtc2 %0, $0\n" : "+r"(xy11) : : );
    asm volatile ("mtc2 %0, $1\nnop\n" : "+r"(z11) : : );   
}

inline void gte_ldv0_f(const SVECTOR& v0)
{
    int xy11 = ((v0.vx&0xFFFF)|(v0.vy<<16));
    int z11 = v0.vz;
    asm volatile ("mtc2 %0, $0\n" : "+r"(xy11) : : );
    asm volatile ("mtc2 %0, $1\nnop\n" : "+r"(z11) : : );   
}

// a few extra macros for faster code, needs testing
// these don't abuse the stack to store GTE calculation results
#define gte_stotz_m( r0 ) __asm__ volatile (    \
    "mfc2   %0, $7;"        \
    : "=r"( r0 )            \
    : )
 
#define gte_stflg_m( r0 ) __asm__ volatile (    \
    "mfc2   %0, $31;"       \
    : "=r"( r0 )            \
    : )
 
#define gte_stopz_m( r0 ) __asm__ volatile (    \
    "mfc2   %0, $24;"       \
    : "=r"( r0 )            \
    : )
 
// direct access to POLY_GT4.rgb3
#define gte_strr3_gt4( r0 ) __asm__ volatile (  \
    "swc2   $22, 40( %0 );" \
    :                       \
    : "r"( r0 )             \
    : "memory" )
 
// direct access to POLY_GT4.xy3
#define gte_stsxy_gt4_3( r0 ) __asm__ volatile (\
    "swc2   $14, 0x2C( %0 )"\
    :                       \
    : "r"( r0 )             \
    : "memory" )

// direct access to POLY_GT4.xy3
#define gte_stsxy_ft4_3( r0 ) __asm__ volatile (\
    "swc2   $14, 0x20( %0 )"\
    :                       \
    : "r"( r0 )             \
    : "memory" )

// direct access to POLY_GT4.rgb3
#define gte_strr3_ft4( r0 ) __asm__ volatile (  \
    "swc2   $22, 8( %0 );" \
    :                       \
    : "r"( r0 )             \
    : "memory" )

#define gte_strgb3_ft4( r0 ) __asm__  (			\
	"swc2	$20, 4( %0 );"					\
	:							\
	: "r"( r0 )						\
	: "memory" )

#endif //_FAST_DRAW_FUNCTIONS_H_