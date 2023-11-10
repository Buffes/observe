VARYING vec3 vGlobalPosition;

void MAIN()
{
    vec4 pos = INSTANCE_MODEL_MATRIX * vec4(VERTEX, 1.0);
    vGlobalPosition = pos.xyz;
}
