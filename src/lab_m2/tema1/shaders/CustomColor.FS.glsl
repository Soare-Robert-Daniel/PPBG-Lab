#version 330

// Input
in vec3 color;
uniform int object_id;
// Output
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(color, object_id / 255.0f);
}
