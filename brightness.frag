#version 330 core

out vec4 outColor;
uniform float uAlpha; // providnost, vrednost izme�u 0.0 i 0.5

void main() {
    outColor = vec4(0.0, 0.0, 0.0, uAlpha);
}