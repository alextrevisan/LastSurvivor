/* Polygon clip detection code
 *
 * The polygon clipping logic is based on the Cohen-Sutherland algorithm, but
 * only the off-screen detection logic is used to determine which polygon edges
 * are off-screen.
 *
 * In tri_clip, the following edges are checked as follows:
 *
 *  |\
 *  |  \
 *  |    \
 *  |      \
 *  |--------
 *
 * In quad_clip, the following edges are checked as follows:
 *
 *  |---------|
 *  | \     / |
 *  |   \ /   |
 *  |   / \   |
 *  | /     \ |
 *  |---------|
 *
 * The inner portion of the quad is checked, otherwise the quad will be
 * culled out if the camera faces right into it, where all four edges
 * are off-screen at once.
 *
 */
 
#include "clip.h"

constexpr char CLIP_LEFT = 1;
constexpr char CLIP_RIGHT = 2;
constexpr char CLIP_TOP	= 3;
constexpr char CLIP_BOTTOM	= 4;
//1858
inline const unsigned char test_clip(const RECT* clip, const DVECTOR* rect)
{
	
	unsigned char ret = int(rect->vx < clip->x) << CLIP_LEFT;
	ret |=  int(rect->vx > (clip->x+(clip->w)))<<CLIP_RIGHT;
	ret |=  int(rect->vy < clip->y)<<CLIP_TOP;
	ret |=  int(rect->vy > (clip->y+(clip->h)))<<CLIP_BOTTOM;

	return ret;
}

//1858
int tri_clip(const RECT* clip, const DVECTOR* v0, const DVECTOR* v1, const DVECTOR* v2) {
	
	// Returns non-zero if a triangle is outside the screen boundaries
	
	const auto c0 = test_clip(clip, v0);
	const auto c1 = test_clip(clip, v1);
	if ( ( c0 & c1 ) == 0 )
		return 0;

	const auto c2 = test_clip(clip, v2);
	if ( ( c0 & c2 ) == 0 )
		return 0;
	
	if ( ( c1 & c2 ) == 0 )
		return 0;

	return 1;
}
