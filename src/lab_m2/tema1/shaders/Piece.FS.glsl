#version 330

uniform vec3 object_color;
uniform int object_id;
// Input
in vec3 color;
// Output
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(object_color, object_id / 255.0f);
}
