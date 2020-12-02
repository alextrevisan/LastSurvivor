#ifndef _MESH_H_
#define _MESH_H_

#include <psxgte.h>
#include <psxgpu.h>
#include <inline_c.h>

#define OT_LEN			1024

class Mesh
{
    static char* RenderMesh(const SVECTOR* mesh, int size, char* db_nextpri)
    {
        int p;
        POLY_F3* pol4 = (POLY_F3*)db_nextpri;
		for(int i=0; i < size;i+=3)
		{
			gte_ldv3( 
				&mesh[i], 
				&mesh[i+1], 
				&mesh[i+2] );
			
			gte_rtpt();
			
			gte_avsz3();
			gte_stotz( &p );
			
			if( ( (p>>2) >= OT_LEN ) || ( (p>>2) <= 0 ) )
				continue;
			
			setPolyF3( pol4 );
			
			// Set the projected vertices to the primitive
			gte_stsxy0( &pol4->x0 );
			gte_stsxy1( &pol4->x1 );
			gte_stsxy2( &pol4->x2 );

			setRGB0(pol4, 128,128,128);
			addPrim( db[db_active].ot+(p>>2), pol4 );
			pol4++;
		}
    }    
};

#endif
