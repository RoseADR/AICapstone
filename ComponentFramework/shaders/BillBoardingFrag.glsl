#version 450
#extension GL_ARB_separate_shader_objects : enable

// Input variables: attributes passed from the vertex shader
layout(location = 0) in vec3 vertNormal; // Vertex normal (used for lighting calculations)
layout(location = 1) in vec3 lightDir;	// Direction of the light source
layout(location = 2) in vec3 eyeDir;	// Direction from the fragment to the camera/eye
layout(location = 3) in vec2 texCoord; // Texture coordinates

// Light data uniform: information about the light (position and color)
layout (std140, binding = 1) uniform LightData {
    vec3 position; // Light position in world space
    vec4 colour;	// Light color (RGBA)
};

// Texture uniform: texture to sample from
uniform sampler2D myTexture;

layout(location = 0) out vec4 fragColor; // Output color for the fragment

void main() {
// Diffuse, specular, and ambient material properties (used for lighting)
	vec4 kd = colour;		// Diffuse color (based on light's color)
	vec4 ks = 0.7 * kd;		// Specular color (scaled to 70% of the diffuse)
	vec4 ka = 0.01 * kd;	// Ambient color (very dim)

	// Sample the texture at the given texture coordinates
	vec4 textureColor = texture(myTexture,texCoord); 

	// Diffuse lighting component: how much light hits the surface
	float diff = max(dot(vertNormal, lightDir), 0.0); // Calculate the dot product of the normal and light direction

	// Specular reflection: simulate shiny highlights on the surface
    // Reflection vector is based on the incident light direction, not the direction to the light source
	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source
	vec3 reflection = normalize(reflect(-lightDir, vertNormal)); // Reflect the light direction around the normal
	
	float spec = max(dot(eyeDir, reflection), 0.0); // Calculate the specular component based on the eye direction and reflection vector
	
	spec = pow(spec,14.0); // Raise specular value to a power for sharpness of the highlight
	
	// Final color computation: ambient + diffuse + specular lighting effects
	vec4 outColor = ka + (textureColor * kd * diff) + (ks * spec); // Calculate the total fragment color with texture, diffuse, and specular
	
	// Set the alpha value of the final color to 0.5 (semi-transparent)
	outColor.a = 1.0;
	
	// Output the final color to the fragment shader's output
	fragColor =  ka + (textureColor * kd * diff) + (ks * spec);
}