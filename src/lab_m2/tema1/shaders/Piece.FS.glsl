#version 430

uniform vec3 object_color;
uniform int object_id;
uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;

uniform vec3 camera_position;
// Input
in vec3 color;
// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) in vec3 world_position;
layout(location = 2) in vec3 world_normal;

vec3 myReflect()
{
    vec3 pos_reflect = reflect( world_position - camera_position, world_normal);
    return texture(texture_cubemap, pos_reflect).xyz;
}


void main()
{
    out_color = vec4(mix(object_color, myReflect(), 0.25f), object_id / 255.0f);
}
