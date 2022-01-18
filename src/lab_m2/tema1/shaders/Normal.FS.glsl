#version 430

// Input
layout(location = 0) in vec3 text_coord;

// Uniform properties
uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

uniform vec3 camera_position;
// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec3 color = texture(texture_cubemap, text_coord).xyz;
    out_color = vec4(color, 0);
}
