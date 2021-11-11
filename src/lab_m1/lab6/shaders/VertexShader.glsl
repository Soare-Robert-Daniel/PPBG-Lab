#version 330

// Input
// TODO(student): Get vertex attributes from each location
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec3 v_texture;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float time;

// Output
// TODO(student): Output values to fragment shader
out vec3 frag_color;
out vec3 normal;

void main()
{
    // TODO(student): Send output to fragment shader
    vec3 color = v_color + +vec3(0, 1, 0) * sin(time); // BONUS
    frag_color = color;
    normal = v_normal;
    // TODO(student): Compute gl_Position
    vec3 pos = v_position + vec3(0, 1, 0) * sin(time); // BONUS
    gl_Position = Projection * View * Model * vec4(pos, 1.0);
}
