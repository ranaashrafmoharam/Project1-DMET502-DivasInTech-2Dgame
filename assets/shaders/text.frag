#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec4  uColor;
uniform int   uUseTexture;
uniform sampler2D uFontTexture;

void main() {
    if (uUseTexture == 1) {
        float alpha = texture(uFontTexture, TexCoord).r;
        if (alpha < 0.1) discard;
        FragColor = vec4(uColor.rgb, uColor.a * alpha);
    } else {
        FragColor = uColor;
    }
}
