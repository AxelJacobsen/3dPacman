#ifndef __MAPSHAD_H_
#define __MAPSHAD_H_

#include <string>

static const std::string mapVertexShaderSrc = R"(
#version 430 core

/** Inputs */
in vec3 mPosition;
in vec2 mTexcoord;

/** Uniform */
uniform mat4 view;
uniform mat4 projection;

/** Outputs */
out vec2 mapTexcoord;

void main()
{
//We multiply our matrices with our position to change the positions of vertices to their final destinations.
mapTexcoord = mTexcoord;
gl_Position = projection * view * vec4(mPosition, 1.0f);
}
)";

static const std::string mapFragmentShaderSrc = R"(
#version 430 core

/** Inputs */
in vec2 mapTexcoord;

/** Outputs */
out vec4 color;

uniform sampler2D u_mapTexture;

void main()
{
	vec4 textColorM = texture(u_mapTexture, mapTexcoord);
	color = textColorM;
}
)";


#endif // __MAPSHAD_H_
