#version 450

layout(location = 0) in vec3 worldPos;
layout(location = 1) in vec3 viewDir;
layout(location = 2) in vec3 vertexNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outPosition;
layout(location = 2) out vec4 outNormal;
layout(location = 3) out vec4 outSpecular;

void main() {

    // vec2 coord = gl_PointCoord - vec2(0.5);
    // outColor = vec4(fragColor, 0.5 - length(coord));
    vec3 color = worldPos * 500.0;
    outColor = vec4(worldPos, 1.0);

    outColor = vec4(vec3(worldPos.z * 0.5), 1.0);

    outPosition = vec4(worldPos, 1.0);

    outNormal = vec4(0.0, 0.0, 1.0, 1.0);

    outSpecular = vec4(0.5, 0.5, 0.5, 1.0);

}