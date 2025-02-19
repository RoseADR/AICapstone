#version 450
#extension GL_ARB_separate_shader_objects : enable

// Input attributes from vertex buffer
layout(location = 0) in vec4 vVertex;   // Vertex position (x, y, z, w)
layout(location = 1) in vec3 vNormal;   // Vertex normal (direction of surface)
layout(location = 2) in vec2 uvCoord;   // Texture coordinates (u, v)

// Camera matrices for projection and view
layout (std140, binding = 0) uniform CameraMatricies {
    mat4 projection;    // Projection matrix (perspective or orthographic)
    mat4 view;          // View matrix (camera position and orientation)
};

// Light data (position and color)
layout (std140, binding = 1) uniform LightData {
    vec3 position;  // Light source position (x, y, z)
    vec4 colour;    // Light color (r, g, b, a)
};

// Model transformation matrix (for transforming the object's vertices)
layout(location = 2) uniform mat4 modelMatrix;  // Object's model transformation matrix

// Output variables to pass data to the fragment shader
layout(location = 0) out vec3 vertNormal;   // Transformed normal vector
layout(location = 1) out vec3 lightDir;     // Direction to the light source
layout(location = 2) out vec3 eyeDir;       // Direction to the camera (viewing direction)
layout(location = 3) out vec2 texCoord;     // Texture coordinates

// Uniform variables (parameters passed to the shader)
// For Sprite Sheet Animations
uniform float index;        // Index for texture animation or variation
uniform bool movingUp;
uniform bool movingDown;
uniform bool facingLeft;    // Is the object facing left
uniform bool facingRight;   // Is the object facing right
uniform bool movingTexture; // Is the texture moving/animating



void main() {
    texCoord = uvCoord; // Set the texture coordinates to the incoming ones
    texCoord.y *= -1.0; // Invert the y-coordinate for texture mapping (GL's texture coords start from the top-left)
   
   // If the texture is moving (or animating), adjust the x texture coordinate based on the index
   // if(movingTexture){
    texCoord.x = texCoord.x * 0.125 + (index * 0.125);
    texCoord.y = texCoord.y * 0.125 + (index * 0.125);
    
    // for flipping the texture coordinates when the object is facing left or right
    if(facingLeft){
        // Move the texture horizontally based on the 'index' value for animation 
    texCoord.x = (index * 1) * 0.125 - texCoord.x;

   }

    // Create a normal matrix by transforming the normal vectors using the model and view matrix (correcting for non-uniform scaling)
    mat3 normalMatrix = mat3(transpose(inverse(view * modelMatrix)));
    
    // Normalize and transform the vertex normal to the correct orientation in world space
    vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
    
    // Compute the vertex position in view space (camera space)
    vec3 vertPos = vec3(view * modelMatrix * vVertex);
    
    // Calculate the direction from the vertex to the camera (eye direction)
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;  // Eye direction is the opposite of vertex direction
    
    // Calculate the direction from the vertex to the light source
    lightDir = normalize(position - vertPos); // Direction from the vertex to the light position
    
    // Final position of the vertex in clip space (after applying projection, view, and model transformations)
    gl_Position = projection * view * modelMatrix * vVertex;
}