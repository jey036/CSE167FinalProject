#version 330 core

// This is a sample fragment shader.
in vec3 Normal;
in vec3 Position;
in vec2 texCoord;

out vec4 color;

uniform vec3 cameraPos;

uniform sampler2D dudvMap;
uniform samplerCube skybox;

uniform float moveFactor;

const float waveStrength = 0.005;

void main()
{
    vec2 distortion1 = (texture(dudvMap, vec2(texCoord.x + moveFactor, texCoord.y)).rg * 2.0 - 1.0) * waveStrength;
    
    //reflection
    vec3 I = normalize(Position - ( - cameraPos));
    vec3 R = reflect(I, normalize(- Normal));
    
    // R += distortion1;
    R += vec3(distortion1.x,distortion1.y,1.0);
    
    vec4 reflectColor = texture(skybox, R);
    
    //refraction
    vec3 refracI = normalize(Position - ( - cameraPos));
    vec3 refracR = refract(refracI, normalize( Normal), 0.9);
    
    // refracR += distortion1;
    refracR += vec3(distortion1.x,distortion1.y,1.0);
    
    vec4 refractColor = texture(skybox, refracR);
    
    // color = texture(dudvMap, texCoord);
    
    color = mix(reflectColor, refractColor, 0.5);
}