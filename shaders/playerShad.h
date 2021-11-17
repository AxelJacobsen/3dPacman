#ifndef __PLAYERSHAD_H_
#define __PLAYERSHAD_H_

#include <string>

static const std::string playerVertexShaderSrc = R"(
#version 430 core

/** Inputs */
in vec3 pPosition;
in vec2 pTexcoord;

uniform mat4 view;
uniform mat4 projection;

/** Outputs */
out vec2 vsTexcoord;

/** Uniform */
layout(location=0) uniform mat4 u_TransformationMat = mat4(1);

void main()
{
vsTexcoord = pTexcoord;
gl_Position = projection * view * u_TransformationMat * vec4(pPosition, 1.0f);
}
)";

static const std::string playerFragmentShaderSrc = R"(
#version 430 core

/** Inputs */
in vec2 vsTexcoord;

/** Outputs */
out vec4 color;

uniform sampler2D u_PlayerTexture;

void main()
{
	vec4 textColorP = texture(u_PlayerTexture, vsTexcoord);
	if(textColorP.a < 0.1)
        discard;
	color = textColorP;
}
)";

#endif // __PLAYERSHAD_H_
