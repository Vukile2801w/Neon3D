#version 420 core

out vec4 FragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(1920.0, 1080.0);

    float zoom = 0.6; // manje = veći zoom

    uv = (uv * 3.5 - vec2(2.5, 1.75)) * zoom;

    vec2 z = vec2(0.0);
    vec2 c = uv;

    int i;
    const int MAX_ITER = 200;

    for(i = 0; i < MAX_ITER; i++)
    {
        float x = z.x * z.x - z.y * z.y + c.x;
        float y = 2.0 * z.x * z.y + c.y;

        z = vec2(x, y);

        if(dot(z, z) > 4.0)
            break;
    }

    float t = float(i) / float(MAX_ITER);

    FragColor = vec4(
        t,
        t,
        t,
        1.0);
}