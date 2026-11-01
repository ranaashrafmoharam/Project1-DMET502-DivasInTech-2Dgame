#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform vec4  uColor;
uniform float uRed;          // 0..1 damage tint (redder as health drops, US-43)
uniform float uGray;         // 0..1 grayscale (locked cards, US-19)
uniform int   uUseTexture;   // 1 if drawing a textured sprite, 0 if solid color
uniform sampler2D uTexture;

void main() {
    vec4 c = uColor;
    if (uUseTexture == 1) {
        vec4 texCol = texture(uTexture, TexCoord);
        c *= texCol;
    }
    
    // Discard nearly fully transparent pixels for clean pixel-art sprites
    if (c.a < 0.05) {
        discard;
    }
    
    // Damage feedback: blend towards bright damage red based on uRed (US-43)
    vec3 tinted = mix(c.rgb, vec3(1.0, 0.12, 0.12), uRed * 0.85);
    
    // Locked card grayscale: luminance formula (US-19)
    float gray = dot(tinted, vec3(0.299, 0.587, 0.114));
    vec3 finalRgb = mix(tinted, vec3(gray), uGray);
    
    FragColor = vec4(finalRgb, c.a);
}
