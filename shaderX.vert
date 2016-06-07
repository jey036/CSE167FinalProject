#version 330 core

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;

out vec3 Normal;
out vec3 Position;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float tiling = 6.0;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    
    // out
    Normal = mat3(transpose(inverse(model))) * vec3(0.0f, 1.0f, 0.0f);
    Position = vec3(model * vec4(position, 1.0f));
    texCoord = vec2(position.x/2.0f + 0.5f,position.y/2.0f + 0.5f) * tiling;
}