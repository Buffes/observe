void MAIN()
{
    float scale = INSTANCE_DATA.x;

    mat4 modelview_matrix_billboard = VIEW_MATRIX * INSTANCE_MODEL_MATRIX;
    modelview_matrix_billboard[0][0] = scale;
    modelview_matrix_billboard[0][1] = 0;
    modelview_matrix_billboard[0][2] = 0;

    modelview_matrix_billboard[1][0] = 0;
    modelview_matrix_billboard[1][1] = scale;
    modelview_matrix_billboard[1][2] = 0;

    modelview_matrix_billboard[2][0] = 0;
    modelview_matrix_billboard[2][1] = 0;
    modelview_matrix_billboard[2][2] = scale;

    mat4 mvp = PROJECTION_MATRIX * modelview_matrix_billboard;

    vec4 pos = mvp * vec4(VERTEX, 1.0);
    POSITION = pos;
}
