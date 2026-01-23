#version 460 core

out vec4 FragColor;

in vec2 texCoord;

uniform float uCameraAspect;
uniform float uScreenAspect;

uniform sampler2D uTexture;

void main()
{
    vec2 uv = texCoord;

    if (uCameraAspect > uScreenAspect)
    {
        // letterbox: top & bottom bars
        float scaleY = uScreenAspect / uCameraAspect;
        uv.y = (uv.y - 0.5) / scaleY + 0.5;
    }
    else
    {
        // pillarbox: left & right bars
        float scaleX = uCameraAspect / uScreenAspect;
        uv.x = (uv.x - 0.5) / scaleX + 0.5;
    }

    if (uv.x < 0.0 || uv.x > 1.0 ||
        uv.y < 0.0 || uv.y > 1.0)
    {
        FragColor = vec4(0,0,0,1);
        return;
    }

    FragColor = texture(uTexture, uv);
}
