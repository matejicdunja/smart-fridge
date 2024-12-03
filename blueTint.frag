#version 330 core

in vec2 chTex; // koordinate teksture
out vec4 outCol;

uniform sampler2D uTex;   // originalna tekstura
uniform float blueTint;   // intenzitet plave boje (0.0 - 1.0)

void main()
{
    vec4 texColor = texture(uTex, chTex); // ucitavanje originalne boje iz teksture
    vec4 tintColor = vec4(0.0, 0.0, blueTint, 0.5); // plava tinta

    // kombinacija boja teksture i tinte
    outCol = mix(texColor, tintColor, tintColor.a);
}