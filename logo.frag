#version 330 core

in vec2 chTex; // koordinate teksture
out vec4 outCol;

uniform sampler2D uTex; // teksturna jedinica
uniform float time;     // vreme za animaciju
uniform bool isPulsing;
uniform bool isKey5;
uniform float logoPosX; // logo_x (od -0.7 do 0.7)

void main()
{
    float baseBrightness = 1.0;         // osvetljenost bez pulsiranja
    float pulsingRange = isPulsing ? 1.0 : 0.0; // opseg pulsiranja (0 ako nije aktivno)
    float pulse = baseBrightness + pulsingRange * sin(time*2.0);

    float distanceFromCenter = abs(logoPosX) / 0.7; // opseg [0, 1]
    // alpha [1.0, 0.3] ili 1.0 ako je Key5
    float alpha = isKey5 ? 1.0 - (distanceFromCenter * 0.7) : 1.0; 

    // boja iz teksture
    vec4 texColor = texture(uTex, chTex);

    // skaliranje boje teksture pulsirajucim intenzitetom (menja osvetljenost)
    outCol = texColor * vec4(vec3(pulse), alpha);
}