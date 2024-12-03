#version 330 core

in vec2 chTex; // koordinate teksture iz vertex shader-a
out vec4 outColor;

uniform sampler2D uTex; // tekstura
uniform float colorIntensity; // intenzitet boje (vrednost izmeðu 0.0 i 1.0)

void main()
{
    //vec4 textureColor = texture(uTex, chTex); // boja teksture
    vec4 displayColor = vec4(0.0, 0.0, 0.0, colorIntensity); // boja displeja sa postepenim intenzitetom
    outColor = displayColor; // mešanje teksture i boje displeja
}