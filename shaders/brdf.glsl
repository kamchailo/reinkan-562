#define PI 3.141592

vec3 EvalBrdf(vec3 N, vec3 L, vec3 V, Material material)
{
    vec3 Kd = material.diffuse;
    vec3 Ks = material.specular;
    const float alpha = material.shininess;

    vec3 H = normalize(L + V);
    float LH = dot(L, H);

    // L = Wi
    // V = Wo
    // m = H

    // F factor
    vec3 F = Ks + (vec3(1.0) - Ks) * pow((1 - LH), 5);

    // D factor GGX
    float mN = dot(H, N);
    float tan_square_theta_m = (1.0 - mN * mN) / (mN * mN);
    float alpha_square = alpha * alpha;
    float D = clamp(mN, 0.0, 1.0) * alpha_square
           / (PI * pow(mN, 4) * pow(alpha_square + tan_square_theta_m, 2));
    // float D = alpha_square
        //    / (PI * pow(mN, 4) * pow(alpha_square + tan_square_theta_m, 2));


    // G factor with V
    float mV = dot(H, V);
    float NV = dot(N, V);
    float tan_square_theta_v = (1.0 -  NV * NV) /  (NV * NV);
    float GV;
    if(NV > 1.0 || sqrt(tan_square_theta_v) == 0)
    {
        GV = 1.0;
    }
    else
    {
        int x;
        if (mV / NV > 0) x = 1;
        else x = 0;
        GV = x * 2 / (1.0 + sqrt(1 + alpha_square * tan_square_theta_v));
    }

    // G factor with L
    float mL = LH; // dot(L, H);
    float NL = dot(N, L);
    float tan_square_theta_l = (1.0 -  NL * NL) /  (NL * NL);
    float GL;
    if(NL > 1.0 || sqrt(tan_square_theta_l) == 0)
    {
        GL = 1.0;
    }
    else
    {
        int x;
        if (mL / NL > 0) x = 1;
        else x = 0;
        GL = x * 2 / (1.0 + sqrt(1 + alpha_square * tan_square_theta_l));
    }
    float G = GV * GL;

    return ( Kd/PI + ((D*F*G) / ( 4 * abs(dot(V,N)) * abs(dot(L,N)) )) );
}   