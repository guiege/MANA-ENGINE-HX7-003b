#version 330 core
in vec2 TexCoords;

uniform sampler2D image;
uniform vec4 spriteColor;
uniform sampler2D Palette; // A palette of 256 colors(256 x 1)

void main()
{
    // Pick up a color index
    vec4 index = texture2D(image, TexCoords);
    // Retrieve the actual color from the palette
    vec4 texel = texture2D(Palette, index.xy);
    vec4 finalColor = texel * spriteColor;
    
    gl_FragColor = finalColor;  //Output the color
}