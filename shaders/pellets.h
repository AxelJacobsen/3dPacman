#ifndef __PELLETS_H_
#define __PELLETS_H_

#include <string>

static const std::string pelletVertexShaderSrc = R"(
#version 430 core

layout(location = 0) in vec3 p_Position;

//We specify our uniforms. We do not need to specify locations manually, but it helps with knowing what is bound where.
uniform mat4 view;
uniform mat4 projection;


void main()
{
//We multiply our matrices with our position to change the positions of vertices to their final destinations.
gl_Position = projection * view * vec4(p_Position, 1.0f);
}
)";

static const std::string pelletFragmentShaderSrc = R"(
#version 430 core

out vec4 color;

uniform vec4 u_Color;

void main()
{
color = u_Color;
}
)";


#endif // __PELLETS_H_
