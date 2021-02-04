#include <sys/types.h>
#include <stdio.h>
#include <malloc.h>
#include <psxgte.h>
#include <psxgpu.h>
#include <inline_c.h>
#include <clip.h>

/* For easier handling of vertex indices */
typedef struct {
	short v0,v1,v2,v3;
} INDEX;

#define CRAZY 1
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

// direct access to POLY_F4.xy3
#define gte_stsxy_f4_3( r0 ) __asm__ volatile (\
    "swc2   $14, 0x14( %0 )"\
    :                       \
    : "r"( r0 )             \
    : "memory" )
	
namespace
{
	int fps;
	int fps_counter;
	int fps_measure;

	void vsync_cb(void)
	{
		fps_counter++;
		if (fps_counter >= 60)
		{
			fps = fps_measure;
			fps_measure = 0;
			fps_counter = 0;
		}
	}
} // namespace
template <int OrderingTableLength, int PrimitivesLength>
class GraphClass
{
	u_int OrderingTable[2][OrderingTableLength];
	u_char Primitives[2][PrimitivesLength];
	u_char *NextPrimitive;

	int OrderingTableCount;
	int DoubleBufferIndex;

	DISPENV DisplayEnvironment[2];
	DRAWENV DrawEnvironment[2];

public:
	GraphClass()
		: OT_LEN(OrderingTableLength)
	{
		//OrderingTable[0] = new u_int[OrderingTableLength];
		//OrderingTable[1] = new u_int[OrderingTableLength];

		DoubleBufferIndex = 0;
		OrderingTableCount = OrderingTableLength;
		ClearOTagR(OrderingTable[0], OrderingTableCount);
		ClearOTagR(OrderingTable[1], OrderingTableCount);

		//Primitives[0] = new u_char[PrimitivesLength];
		//Primitives[1] = new u_char[PrimitivesLength];

		NextPrimitive = Primitives[0];

		printf("GraphClass::GraphClass: Buffers allocated.\n");

	} /* GraphClass */

	virtual ~GraphClass()
	{
		/* free the OTs and primitive buffers */

		printf("GraphClass::GraphClass: Buffers freed.\n");

	} /* ~GraphClass */

	void SetResolution(short width, short heigth)
	{
		Width = width;
		Height = heigth;
		ResetGraph(0);

		SetDefDispEnv(&DisplayEnvironment[0], 0, heigth, width, heigth);
		SetDefDrawEnv(&DrawEnvironment[0], 0, 0, width, heigth);

		setRGB0(&DrawEnvironment[0], 63, 0, 127);
		DrawEnvironment[0].isbg = 1;
		DrawEnvironment[0].dtd = 1;

		SetDefDispEnv(&DisplayEnvironment[1], 0, 0, width, heigth);
		SetDefDrawEnv(&DrawEnvironment[1], 0, heigth, width, heigth);

		setRGB0(&DrawEnvironment[1], 63, 0, 127);
		DrawEnvironment[1].isbg = 1;
		DrawEnvironment[1].dtd = 1;

		PutDispEnv(&DisplayEnvironment[0]);
		PutDrawEnv(&DrawEnvironment[0]);

		// Initialize the GTE
		InitGeom();

		// Set GTE offset (recommended method  of centering)
		gte_SetGeomOffset(width >> 1, heigth >> 1);

		// Set screen depth (basically FOV control, W/2 works best)
		gte_SetGeomScreen(width >> 1);

		// Set light ambient color and light color matrix
		gte_SetBackColor(220, 220, 255);

		VSyncCallback(vsync_cb);

	} /* SetRes */

	template <typename T, typename U>
	struct is_same
	{
		static constexpr bool value = false;
	};

	template <typename T>
	struct is_same<T, T> //specialization
	{
		static constexpr bool value = true;
	};

	typedef struct   
	{   
	u_long limit;         /* divide limit */   
	long   hwd,vwd;       /* dummy */   
	int    shift;         /* OT bit shift */   
	u_long  *org;         /* OT org */   
	u_long   *pk;         /* packet base */   
	long     otz;         /* gte sz */   
	long   adivz;         /* dummy */   
	long   nearz;         /* PROJECTION/2 for Z clipping */   
	long    flg0;         /* gte flag */   
	long     flg;         /* gte flag */   
	short  minx,miny,maxx,maxy;   /* polygon distance min-max */   
	short  hwd0,vwd0;     /* resolution of screen (center orign) */   
	u_long   *tag;        /* work temprly for addPrim */   
	long    sz4[4];       /* work sz */   
	POLY_GT4  si;         /* work packet */   
	} IFO;
	
	#define USE_SET_FLAG 0

	static constexpr auto GTEFLG_ERROR = 0x80000000;
	template<typename PolygonType>
	inline void DrawFast(const SVECTOR(&values)[], const SVECTOR normal, const TIM_IMAGE* texture = nullptr)
	{
		/* Load the first 3 vertices of a quad to the GTE */
		gte_ldv3(&values[0], &values[1], &values[2]);

		/* Rotation, Translation and Perspective Triple */
		gte_rtpt_b();

		/* Compute normal clip for backface culling */
		gte_nclip_b();
		#if USE_SET_FLAG
		int flg;
		gte_stflg_m(flg);
		if (flg & GTEFLG_ERROR)
			return;
		#endif
		/* Get result*/
		int p;
		gte_stopz_m(p);

		/* Skip this face if backfaced */
		if (p <= 0)
			return; //OK ATE AQUI

		PolygonType *polygon = (PolygonType *)GetNextPri();
		gte_stsxy3_f4(polygon);
		gte_ldv0(&values[3]);
		gte_rtps_b();

		#if USE_SET_FLAG
		gte_stflg_m(flg);
		if (flg & GTEFLG_ERROR) return;
		#endif

		gte_stsxy_f4_3(polygon);
		/* Calculate average Z for depth sorting */
		gte_avsz4();

		gte_stotz_m(p);
		if (!(p >> 6)) return;

		/* Skip if clipping off */
		/* (the shift right operator is to scale the depth precision) */
		/*if (((p >> 2) <= 0) || (p >> 2) >= OT_LEN)
			return;*/

		/* Load the face normal */
		gte_ldv0(&normal);

		/* Normal Color Single */
		gte_ncs();

		/* Store result to the primitive */
		gte_strgb(&polygon->r0);
		
		/* Initialize the primitive */
		setPolyF4(polygon);

		/* Sort primitive to the ordering table */
		unsigned int *tag = &GetOt()[p >> 4];
		polygon->tag = (*tag & 0x00FFFFFF) | 0x0C000000;
		*tag = (unsigned int)polygon & 0x00FFFFFF;

		/* Advance to make another primitive */
		constexpr auto size = sizeof(PolygonType);
		IncPri(size);
	}



	template<typename PolygonType>
	inline void DrawFast(const SVECTOR(&vertices)[], const INDEX (&indices)[], const SVECTOR(&normals)[], const int faces)
	{
		for(int i=0; i<faces; i++ ) {
			Draw<PolygonType>({vertices[indices[i].v0]	,
				vertices[indices[i].v1],
				vertices[indices[i].v2],
				vertices[indices[i].v3]},
				normals[i]);
			
		}
	}
#if 0
	void FastTG4L(void *ob, void *packet, CVECTOR *rgb, u32* ot)
	{
		register u32 i, is, *tag;
	#if !CRAZY
		IFO ifo;
	#else
		int otz, flg;
	#endif
		register POLY_GT4 *sx;
		const MD1_QUADS *obj = (const MD1_QUADS*)ob;
		const MD1_QUAD *q = (const MD1_QUAD*)obj->quad_offset;
		const SVECTOR *vp = (const SVECTOR*)obj->vertex_offset;
		const SVECTOR *vn = (const SVECTOR*)obj->normal_offset;
	
		rgb->cd = (rgb->cd & 3) | CODE_PGT4;
		gte_ldrgb(rgb);
	
		sx = (POLY_GT4*)packet;
	
		for (i = 0, is = obj->quad_count; i < is; q++)
		{
			POLY_F4 *polygon = sx;
			gte_ldv3(&vp[q->v0], &vp[q->v1], &vp[q->v2]);

			i++;
			gte_rtpt_b();           /* RotTransPers3 */
	
	#if !CRAZY
			gte_stflg(&ifo.flg0);
			if (ifo.flg0 & GTEFLG_ERROR) { sx += 2; continue; }
			gte_nclip_b();          /* NormalClip */
			gte_stopz(&ifo.otz);    /* back clip */
			if (ifo.otz <= 0) { sx += 2; continue; } /* flipped, skip */
	#else
			gte_stflg_m(flg);
			if (flg & GTEFLG_ERROR) { sx += 2; continue; }
			gte_nclip_b();          /* NormalClip */
			gte_stopz_m(otz);   /* back clip */
			if (otz <= 0) { sx += 2; continue; } /* flipped, skip */
			/*** OK ATE AQUI **/
	#endif
			gte_stsxy3_gt4((u_long *)polygon);
			gte_ldv0(&vp[q->v3]);
			sx += 2;
			gte_nop();
			gte_rtps_b();           /* RotTransPers */
	#if !CRAZY
			gte_stflg(&ifo.flg);
			if (ifo.flg & GTEFLG_ERROR) continue;
	#else
			gte_stflg_m(flg);
			if (flg & GTEFLG_ERROR) continue;
	#endif
	
			gte_stsxy_gt4_3(polygon);
			gte_avsz4();
	#if !CRAZY
			gte_stotz(&ifo.otz);
			// limit range
			if (!(ifo.otz >> 6)) continue;
	#else
			gte_stotz_m(otz);
			if (!(otz >> 6)) continue;
	#endif
	
			gte_ldv3(&vn[q->n0], &vn[q->n1], &vn[q->n2]);
	#if !CRAZY
			tag = &ot[ifo.otz >> 4];
	#else
			tag = &ot[otz >> 4];
	#endif
			gte_ncct_b();
			gte_strgb3_gt4(polygon);
	
			gte_ldv0(&vn[q->n3]);
			polygon->tag = (*tag & 0x00FFFFFF) | 0x0C000000;
			gte_nccs_b();
			gte_strr3_gt4(polygon);
	
			// sort!!
			*tag = (u32)si & 0x00FFFFFF;
		}
	}
#endif
	/** @TODO FT4 **/
	template <typename PolygonType>
	inline void Draw(const SVECTOR (&values)[], const SVECTOR &normal, const TIM_IMAGE* texture = nullptr)
	{
		/* Load the first 3 vertices of a quad to the GTE */
		gte_ldv3(&values[0], &values[1], &values[2]);

		/* Rotation, Translation and Perspective Triple */
		gte_rtpt();

		/* Compute normal clip for backface culling */
		gte_nclip();

		/* Get result*/
		int p;
		gte_stopz(&p);

		/* Skip this face if backfaced */
		if (p < 0)
			return;

		/* Calculate average Z for depth sorting */
		if constexpr (is_same<PolygonType, POLY_F4>::value)
			gte_avsz4();
		else if constexpr (is_same<PolygonType, POLY_FT3>::value)
			gte_avsz3();

		gte_stotz(&p);

		/* Skip if clipping off */
		/* (the shift right operator is to scale the depth precision) */
		if (((p >> 2) <= 0) || (p >> 2) >= OT_LEN)
			return;

		const PolygonType *polygon = (PolygonType *)GetNextPri();
		/* Initialize the primitive */
		if constexpr (is_same<PolygonType, POLY_F4>::value)
			setPolyF4(polygon);
		if constexpr (is_same<PolygonType, POLY_F3>::value)
			setPolyF3(polygon);
		if constexpr (is_same<PolygonType, POLY_FT3>::value)
			setPolyFT3(polygon);

		/* Set the projected vertices to the primitive */
		gte_stsxy3(&polygon->x0, &polygon->x1, &polygon->x2);

		// Test if tri is off-screen, discard if so
		const RECT screen_clip{0, 0, Width, Height};
		if (tri_clip(&screen_clip,
					 (DVECTOR *)&polygon->x0, (DVECTOR *)&polygon->x1,
					 (DVECTOR *)&polygon->x2))
			return;

		if constexpr (is_same<PolygonType, POLY_F4>::value)
		{
			/* Compute the last vertex and set the result */
			gte_ldv0(&values[3]);
			gte_rtps();
			gte_stsxy(&polygon->x3);
		}

		/* Load primitive color even though gte_ncs() doesn't use it. */
		/* This is so the GTE will output a color result with the */
		/* correct primitive code. */
		gte_ldrgb(&polygon->r0);

		/* Load the face normal */
		gte_ldv0(&normal);

		/* Normal Color Single */
		gte_ncs();
		//gte_nccs(); ???

		/* Store result to the primitive */
		gte_strgb(&polygon->r0);

		if constexpr(is_same<PolygonType, POLY_FT3>::value)
		{
			gte_avsz4();
			gte_stotz( &p );

			// Set tpage
			polygon->tpage = getTPage(texture->mode&0x8, 0, texture->prect->x, texture->prect->y);
			
			// Set CLUT
			setClut(polygon, texture->crect->x, texture->crect->y);
			
			// Set texture coordinates
			constexpr auto U_out = 0;
			constexpr auto V_out = 0;
			constexpr auto W_out = 64;
			constexpr auto H_out = 64;
			setUV3(polygon, U_out+W_out,V_out, U_out+W_out,V_out+H_out, U_out,V_out);
		}

		/* Sort primitive to the ordering table */
		addPrim(GetOt() + (p >> 2), polygon);
		//setaddr( p, getaddr( ot ) ), setaddr( ot, p )

		/* Advance to make another primitive */
		//NextPrimitive+=sizeof(PolygonType);
		IncPri(sizeof(PolygonType));
	}

	void IncPri(int bytes)
	{
		NextPrimitive += bytes;

	} /* IncPri */

	void SetPri(u_char *ptr)
	{
		NextPrimitive = ptr;

	} /* SetPri */

	inline u_char *GetNextPri(void)
	{
		return (NextPrimitive);

	} /* GetNextPri */

	inline u_int *GetOt(void)
	{
		return (OrderingTable[DoubleBufferIndex]);

	} /* GetOt */

	template <bool UseVSync = true>
	void Display(void)
	{
		fps_measure++;
		printf("%d\n", fps);
		if constexpr (UseVSync)
		{
			VSync(0);
			DrawSync(0);
		}
		SetDispMask(1);

		DoubleBufferIndex = !DoubleBufferIndex;

		PutDispEnv(&DisplayEnvironment[DoubleBufferIndex]);
		PutDrawEnv(&DrawEnvironment[DoubleBufferIndex]);

		DrawOTag(OrderingTable[!DoubleBufferIndex] + (OrderingTableCount - 1));

		ClearOTagR(OrderingTable[DoubleBufferIndex], OrderingTableCount);
		NextPrimitive = Primitives[DoubleBufferIndex];

	} /* Display */

	const int OT_LEN;
	short Width;
	short Height;

}; /* GraphClass */