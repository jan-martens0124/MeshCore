#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ModelViewProjectionMatrix;
uniform vec4 u_Color;

out vec4 v_Color;

void main(){
   gl_Position = u_ModelViewProjectionMatrix * vec4(a_Position, 1.0);
   v_Color = u_Color;
}

#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec4 v_Color;

void main(){
   o_Color = v_Color;
}