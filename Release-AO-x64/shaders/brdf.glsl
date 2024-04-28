#define PI 3.14159265358979323846

float G1(vec3 w, vec3 m, vec3 N, float alpha_suqared)
{
    float wN = dot(w, N);
    float wm = dot(w, m);
    float tan_square_theta_w = (1.0 - wN * wN) / (wN * wN);
    float G1;
    if(wN > 1.0 || sqrt(tan_square_theta_w) == 0) 
    {
        G1 = 1.0;
    } 
    else 
    {
        G1 = clamp(wm / wN, 0.0, 1.0) * (2 / (1 + sqrt(1 + alpha_suqared * tan_square_theta_w)));
    }
    return G1;
}

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

    // G factor
    float G = G1(V, H, N, alpha_square) * G1(L, H, N, alpha_square);

    return ( (Kd * max(dot(N, L), 0.0) )/PI + ((D*F*G) / ( 4 * abs(dot(V,N)) * abs(dot(L,N)) )) );
}   