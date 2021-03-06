#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : require
#include "guiStructs.glsl"

layout(binding = 0) readonly uniform GuiUniformBufferObjectStruct { GuiUniformBufferObject uniformBuffer; };
layout(push_constant) uniform GuiObjectInfoStruct { GuiObjectInfo objectInfo; };

layout(location = 0) out vec4 outFragColor;
layout(location = 1) out vec2 outFragPos;
layout(location = 2) out vec2 outTexCoord;

vec2 positions[6] = vec2[](
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
);

void main() 
{
	vec2 objPos = objectInfo.position.xy*2-vec2(1.0,1.0);// [0,1] to [-1,1] (top left position)
	vec2 objSize = objectInfo.size*2;// width from 1 to 2
	vec2 rectCorner = objPos+positions[gl_VertexIndex]*objSize;
    gl_Position = vec4(rectCorner,objectInfo.position.z,1);

    outFragColor = objectInfo.color;
    outFragPos = rectCorner;

	if(objectInfo.isLetter == 1)
    	outTexCoord = objectInfo.offsetLetter+positions[gl_VertexIndex]*objectInfo.sizeLetter;
	else
    	outTexCoord = positions[gl_VertexIndex];
}
