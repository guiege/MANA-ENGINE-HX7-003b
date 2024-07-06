#version 330 core

layout (location = 0) out vec4 o_Color;

in vec4 vertexColor;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[32];

void main()
{
    int index = int(round(v_TexIndex));
    o_Color = texture2D(u_Textures[index], v_TexCoord) * vertexColor;
} 