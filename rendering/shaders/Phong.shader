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
uniform vec3 u_ViewPosition;

uniform float u_AmbientIntensity;
uniform float u_DiffuseIntensity;
uniform float u_SpecularIntensity;

uniform vec4 u_DiffuseColor;
uniform vec4 u_SpecularColor;
uniform float u_SpecularPower;

void main(){

    // All vectors reside in model space

    // Compute the diffuse component
    vec3 light = normalize(u_LightDirection);
    vec3 normal = normalize(v_Normal);
    float DiffuseFactor = max(dot(normal, light), 0.0);

    // Compute the specular component
    vec3 viewDirection = normalize(u_ViewPosition - v_Position);
    vec3 reflectionDirection = reflect(-light, normal);
    float specular = pow(max(dot(viewDirection, reflectionDirection), 0.0), u_SpecularPower);

    // Combine the components
    o_Color = u_DiffuseColor * (u_AmbientIntensity + u_DiffuseIntensity * DiffuseFactor)
            + u_SpecularColor * (u_SpecularIntensity * specular);

    o_Color[3] = u_DiffuseColor[3]; // Transparancy should not be changed
}