#version 430

// Input
in vec3 color;
uniform int object_id;
uniform sampler2D texture_1;
uniform samplerCube texture_cubemap;
layout(location = 0) in vec3 world_position;
layout(location = 1) in vec3 world_normal;

uniform vec3 camera_position;
// Output
layout(location = 0) out vec4 out_color;

vec3 myReflect()
{
    // TODO(student): Compute the reflection color value
    vec3 pos_reflect = reflect( world_position - camera_position, world_normal);
    return texture(texture_cubemap, pos_reflect).xyz;
}


vec3 myRefract(float refractive_index)
{
    // TODO(student): Compute the refraction color value
    vec3 pos_refract = refract(world_position - camera_position, world_normal, refractive_index);
    return texture(texture_cubemap, pos_refract).xyz;
}


void main()
{
    out_color = vec4(mix(color, myReflect(), 0.3f), object_id / 255.0f);
}
