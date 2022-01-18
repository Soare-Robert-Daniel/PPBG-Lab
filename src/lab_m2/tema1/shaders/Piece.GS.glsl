#version 430

// Input and output topologies
// TODO(student): First, generate a curve (via line strip),
// then a rotation/translation surface (via triangle strip)
layout(lines) in;
layout(triangle_strip, max_vertices = 30) out;

// Uniform properties
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform vec3 control_p0, control_p1, control_p2, control_p3;
uniform int no_of_instances;
uniform int no_of_generated_points;
uniform float max_translate;
uniform float max_rotate;
// Input
in int instance[2];
layout(location = 1) out vec3 world_position;
layout(location = 2) out vec3 world_normal;

vec3 rotateY(vec3 point, float u)
{
    float x = point.x * cos(u) - point.z *sin(u);
    float z = point.x * sin(u) + point.z *cos(u);
    return vec3(x, point.y, z);
}


vec3 translateX(vec3 point, float t)
{
    return vec3(point.x + t, point.y, point.z);
}


// This function computes B(t) with 4 control points. For a visual example,
// see [1]. For an interactive Javascript example with the exact points in
// this code, see [2].
//
// [1] https://www.desmos.com/calculator/cahqdxeshd
// [2] https://jsfiddle.net/6yuv9htf/
vec3 bezier(float t)
{
    return  control_p0 * pow((1 - t), 3) +
            control_p1 * 3 * t * pow((1 - t), 2) +
            control_p2 * 3 * pow(t, 2) * (1 - t) +
            control_p3 * pow(t, 3);
}


// TODO(student): If you want to take things a step further, try drawing a
// Hermite spline. Hint: you can repurpose two of the control points. For a
// visual example, see [1]. For an interactive Javascript example with the
// exact points in this code, see [2].
// 
// Unlike the Javascript function, you MUST NOT call the Bezier function.
// There is another way to draw a Hermite spline, all you need is to find
// the formula.
//
// [1] https://www.desmos.com/calculator/5knm5tkr8m
// [2] https://jsfiddle.net/6yuv9htf/

void main()
{
    const int SURFACE_TYPE_ROTATION     = 0;
    const int SURFACE_TYPE_TRANSLATION  = 1;

    // You can change the value of SURFACE_TYPE to experiment
    // with different transformation types.
    const int SURFACE_TYPE = SURFACE_TYPE_ROTATION;

    if (instance[0] < no_of_instances)
    {
        float delta_t = 1.0f / no_of_generated_points;
        float dist = max_translate / no_of_instances;
        float rot_step = max_rotate / no_of_instances;

        for (int t = 0; t < no_of_generated_points; ++t) {
            vec3 p = bezier(t * delta_t);

            vec3 p1 = ( Model * vec4(translateX( rotateY(p, instance[0] * rot_step), instance[0] * dist), 1)).xyz;
            vec3 p2 = ( Model * vec4(translateX( rotateY(p, (instance[0] + 1 ) * rot_step), (instance[0] + 1) * dist), 1)).xyz;
            vec3 p3 = ( Model * vec4(translateX( rotateY(p, (instance[0] + 2 ) * rot_step), (instance[0] + 2) * dist), 1)).xyz;
            vec3 v1 = normalize(p2 - p1);
            vec3 v2 = normalize(p3 - p1);

            world_normal = cross(v2, v1);
            world_position = (Model * vec4(p1, 1)).xyz;
            gl_Position = Projection * View * Model * vec4(translateX( rotateY(p, instance[0] * rot_step), instance[0] * dist), 1);
            EmitVertex();

            // vec3 p4 = translateX(rotateY(p, (instance[0] + 3 ) * rot_step), (instance[0] + 3) * dist);
            vec3 v3 = normalize(p1 - p2);
            vec3 v4 = normalize(p3 - p2);
            world_normal = cross(v4, v3);
            world_position = (Model * vec4(p2, 1)).xyz;
            gl_Position = Projection * View * Model * vec4(translateX(rotateY(p, (instance[0] + 1 ) * rot_step), (instance[0] + 1) * dist), 1);
            EmitVertex();
        }

        EndPrimitive();
    }

}
