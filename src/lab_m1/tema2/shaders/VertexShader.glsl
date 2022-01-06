#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output
out vec3 frag_normal;
out vec3 frag_color;
out vec2 tex_coord;

uniform float time;



float hash(vec2 p) {
  return fract(sin(dot(p * 17.17, vec2(14.91, 67.31))) * 4791.9511);
}

float noise(vec2 x) {
  vec2 p = floor(x);
  vec2 f = fract(x);
  f = f * f * (3.0 - 2.0 * f);
  vec2 a = vec2(1.0, 0.0);
  return mix(mix(hash(p + a.yy), hash(p + a.xy), f.x),
         mix(hash(p + a.yx), hash(p + a.xx), f.x), f.y);
}

float fbm(vec2 x) {
  float height = 0.0;
  float amplitude = 0.5;
  float frequency = 3.0;
  for (int i = 0; i < 6; i++){
    height += noise(x * frequency) * amplitude;
    amplitude *= 0.5;
    frequency *= 2.0;
  }
  return height;
}

void main()
{
    frag_normal = v_normal;
    frag_color = v_color;
    tex_coord = v_texture_coord;

    vec3 p = vec3( 
        clamp(fbm(v_position.xy * time), 0 , 2),
        clamp(fbm(v_position.xz * time), 0, 2),
        clamp(fbm(v_position.yz * time), 0, 2)
    );

    gl_Position = Projection * View * Model * vec4(v_position + p, 1.0) ;
}
