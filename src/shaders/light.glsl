//--------------------------------------------------
// Atta GLSL
// light.glsl
// Date: 2021-02-15
// By Breno Cunha Queiroz
//--------------------------------------------------
#ifndef LIGHT_GLSL
#define LIGHT_GLSL

const uint LIGHT_TYPE_NONE 			= 0;
const uint LIGHT_TYPE_POINT	 		= 1;
const uint LIGHT_TYPE_SPOT 			= 2;
const uint LIGHT_TYPE_DISTANT 		= 3;
const uint LIGHT_TYPE_PROJECTION 	= 4;
const uint LIGHT_TYPE_INFINITE 		= 5;
const uint LIGHT_TYPE_GONIOMETRIC 	= 6;
const uint LIGHT_TYPE_DIFFUSE 		= 7;
const uint LIGHT_TYPE_AREA_TRIANGLE = 8;

struct Light 
{
	uint type;

	uint nSamples;

	mat4 lightToWorld;
	mat4 worldToLight;

	// General data about the light
	uint datai[5];
	float dataf[2];
	vec4 datav[4];
};

//---------- Data description ----------//
// Point light
// - datav
// 		[0](vec3) -  I: Light intensity (amount of power per unit solid angle, constant for isotropic point lights)
//
// Spot light
// - datav
// 		[0](vec3) - I
// - dataf
//		[0] - cosFalloffStart
//		[1] - cosTotalWidth
//
// Distant light
// - datav
// 		[0](vec3) - L: Emitted radiance by a disk illuminating the scene (5km radius)
// 		[1](vec3) - w: Incident ray from the object being illuminated to the disk
//
// Infinite light
// - datav
// 		[0](vec3) - World center
//		[1](vec3) - Precomputed power approximation
// - dataf
//		[0] - World radius
// - datai
//		[0] - Texture index
//		[1] - Precomputed 2D Pdf texture index (rt)
//		[2] - Precomputed IBL diffuse (rast)
//		[3] - Precomputed IBL specular (rast)
//		[4] - Precomputed brdfLUT (rast)
//
// Area Triangle light
// - datav
// 		[0](vec3) -  I: Light Intensity
// 		[1](vec3) -  v0: Vertex 0
// 		[2](vec3) -  v1: Vertex 1
// 		[3](vec3) -  v2: Vertex 2

#endif// LIGHT_GLSL
