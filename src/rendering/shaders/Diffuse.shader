#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ModelViewProjectionMatrix;

out vec3 v_Position;
out vec3 v_Normal;

void main(){
    gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0);
    v_Position = a_Position;
    v_Normal = a_Normal;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec3 v_Position;
in vec3 v_Normal;

uniform vec3 u_LightDirection;
uniform float u_Ambient;
uniform vec4 u_Color;

void main(){
    // All vectors reside in model space
    vec3 light = normalize(u_LightDirection);
    vec3 normal = normalize(v_Normal);
    float diffuse = max(dot(normal, light), 0.0);
    o_Color = (u_Ambient + diffuse * (1-u_Ambient)) * u_Color;
    o_Color[3] = u_Color[3]; // Transparency should not be changed
}