#version 330 core

layout (location = 0) out vec4 o_Color;

in vec4 vertexColor;
in vec2 v_TexCoord;
in float v_TexIndex;

uniform sampler2D u_Textures[32];
uniform sampler2D Palette;

void main()
{
    int index = int(round(v_TexIndex));
    vec4 pix = texture2D(u_Textures[index], v_TexCoord);
    vec4 texel = texture2D(Palette, pix.xy);
    o_Color = texel * vertexColor;
} 

    // // Pick up a color index
    // vec4 index = texture2D(image, TexCoords);
    // // Retrieve the actual color from the palette
    // vec4 texel = texture2D(Palette, index.xy);
    // gl_FragColor = texel;   //Output the color