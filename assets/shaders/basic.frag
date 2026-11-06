#version 330 core
// assets/shaders/basic.frag
// OWNER: Member 1
// Fragment shader for colored/textured shapes with damage tint and grayscale

in vec2 TexCoord;
out vec4 FragColor;

uniform vec4 u_Color;
uniform vec4 u_Tint;
uniform bool u_UseTexture;
uniform sampler2D u_Texture;
uniform bool u_Grayscale;

void main() {
    vec4 baseColor = u_Color;
    if (u_UseTexture) {
        baseColor = texture(u_Texture, TexCoord) * u_Color;
    }

    vec4 tintedColor = baseColor * u_Tint;

    if (u_Grayscale) {
        // Luminance weights (Rec. 601)
        float gray = dot(tintedColor.rgb, vec3(0.299, 0.587, 0.114));
        FragColor = vec4(vec3(gray), tintedColor.a);
    } else {
        FragColor = tintedColor;
    }
}
