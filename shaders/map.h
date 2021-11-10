#ifndef __MAP_H_
#define __MAP_H_

#include <string>

static const std::string mapVertexShaderSrc = R"(
#version 430 core

layout(location = 0) in vec3 m_Position;

//We specify our uniforms. We do not need to specify locations manually, but it helps with knowing what is bound where.
uniform mat4 view;
uniform mat4 projection;


void main()#ifndef __MAP_H_
#define __MAP_H_

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
out vec2 vsTexcoord;

void main()
{
//We multiply our matrices with our position to change the positions of vertices to their final destinations.
vsTexcoord = mTexcoord;
gl_Position = projection * view * vec4(mPosition, 1.0f);
}
)";

static const std::string mapFragmentShaderSrc = R"(
#version 430 core

/** Inputs */
in vec2 vsTexcoord;

/** Outputs */
out vec4 color;

uniform sampler2D u_MapTexture;

uniform vec4 u_Color;

void main()
{
	vec4 textColorM = texture(u_MapTexture, vsTexcoord);
	color = u_Color; //textColorM;
}
)";


#endif // __MAP_H_

{
//We multiply our matrices with our position to change the positions of vertices to their final destinations.
gl_Position = view * vec4(m_Position, 1.0f);
}
)";

static const std::string mapFragmentShaderSrc = R"(
#version 430 core

out vec4 color;

uniform vec4 u_Color;

void main()
{
color = u_Color;
}
)";


#endif // __MAP_H_
