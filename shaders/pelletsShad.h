#ifndef __PELLETSSHAD_H_
#define __PELLETSSHAD_H_

#include <string>

static const std::string pelletVertexShaderSrc = R"(
#version 430 core

/** Inputs */
in vec3 pelPosition;

/** Outputs */
out mat4 vTransformation;

//We specify our uniforms. We do not need to specify locations manually, but it helps with knowing what is bound where.
uniform mat4 view;
uniform mat4 projection;

void main()
{
//We multiply our matrices with our position to change the positions of vertices to their final destinations.
	gl_Position = vec4(pelPosition, 1.0f);
	vTransformation = projection * view;	
	//Sends transformations to geometry shader to ensure all points are shifted correctly
}
)";

static const std::string pelletGeometryShaderSrc = R"(
#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices=500) out;

/** Constants */
const float M_PI = 3.1415926535897f;	//Hardcoded PI constant

in mat4  vTransformation[];				//Matrix containing the camera transformations

void main() {
	vec4	pos		= gl_in[0].gl_Position;
	float	radius  = 0.01f;
	mat4	t		= vTransformation[0];
	
	// Sphere vars
	const int	stackCount  = 6,			//Trying to go higher than 6 doesnt work on my computer 
				sectorCount = stackCount;	//if pellets dont load you might have to lower this
	
	float	stackStep	= M_PI / stackCount,
			sectorStep	= 2.f * M_PI / sectorCount;

	vec3	spherePoints[sectorCount][stackCount];
	vec3	sphereNormals[sectorCount][stackCount];

	// Loop through points in sphere and add them in spherepoints array
	for (int i=0; i<stackCount; i++) {
		float stackAngle = M_PI/2.f - i*stackStep,
			  xy		 = radius * cos(stackAngle),
			  z			 = radius * sin(stackAngle);

		for (int j=0; j<sectorCount; j++) {
			float sectorAngle = j * sectorStep,
				  x			  = xy * cos(sectorAngle),
				  y			  = xy * sin(sectorAngle);

			spherePoints[j][i] = vec3(x, y, z);
		}
	}

	// Iterate through that array and emit vertices
	for (int i=0; i<stackCount; i++) {
		for (int j=0; j<sectorCount; j++) {
			
			vec3 k1 = spherePoints[j][i],
				 k2 = spherePoints[(j+1)%sectorCount][i],
				 k3 = spherePoints[(j+1)%sectorCount][(i+1)%stackCount],
				 k4 = spherePoints[j][(i+1)%stackCount];

			// First triangle (0,1,2)
			gl_Position = t* vec4(pos.x+k1.x, pos.y+k1.y, pos.z+k1.z, pos.w);
			EmitVertex();

			gl_Position = t* vec4(pos.x+k2.x, pos.y+k2.y, pos.z+k2.z, pos.w);
			EmitVertex();

			gl_Position = t* vec4(pos.x+k3.x, pos.y+k3.y, pos.z+k3.z, pos.w);
			EmitVertex();

			// Second triangle (0,2,3)
			gl_Position = t* vec4(pos.x+k1.x, pos.y+k1.y, pos.z+k1.z, pos.w);
			EmitVertex();

			gl_Position = t* vec4(pos.x+k3.x, pos.y+k3.y, pos.z+k3.z, pos.w);
			EmitVertex();

			gl_Position = t* vec4(pos.x+k4.x, pos.y+k4.y, pos.z+k4.z, pos.w);
			EmitVertex();
		}
	}
	
	EndPrimitive();
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


#endif // __PELLETSSHAD_H_
