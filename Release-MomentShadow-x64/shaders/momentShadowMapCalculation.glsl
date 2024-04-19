vec2 QuadraticEquation(float a, float b, float c) 
{
    float epsilon = 1e-6;

    float discriminant = b * b - 4.0 * a * c;

    if (discriminant > epsilon) 
    {
        // Two real roots
        float root1 = (-b - sqrt(discriminant)) / (2.0 * a);
        float root2 = (-b + sqrt(discriminant)) / (2.0 * a);
        return vec2(min(root1, root2), max(root1, root2));
    } 
    
    if (discriminant > 0.0 && discriminant < epsilon) 
    {
        // One real root (double root)
        float root = -b / (2.0 * a);
        return vec2(root, root);
    }

    // Complex roots
    float realPart = -b / (2.0 * a);
    float imaginaryPart = sqrt(-discriminant) / (2.0 * a);
    return vec2(realPart, imaginaryPart);
}

vec3 CholeskyDecomposition(float m11, float m12, float m13, float m22, float m23, float m33, float z1, float z2, float z3)
{
    float a = sqrt(m11);
    float b = m12 / a;
    float c = m13 / a;
    float d = sqrt(m22 - (b * b));
    float e = (m23 - (b * c)) / d;
    float f = sqrt(m33 - (c * c) - (e * e));

    // Keep a, d, f positive
    a = max(a, 1e-4);
    d = max(d, 1e-4);
    f = max(f, 1e-4);

    float cHat1 = z1 / a;
    float cHat2 = (z2 - b * cHat1) / d;
    float cHat3 = (z3 - c * cHat1 - e * cHat2) / f;

    float c3 = cHat3 / f;
    float c2 = (cHat2 - e * c3) / d;
    float c1 = (cHat1 - b * c2 - c * c3) / a;

    return vec3(c1, c2, c3);
}

float MomentShadowMapCalculation(sampler2D shadowMap, vec4 shadowCoord)
{
    float epsilon = 1e-3;

    // homogeneuos division
    vec3 projectedFragCoordFromLight = shadowCoord.xyz / shadowCoord.w;
    // Ignore outside shadow map area
    if(projectedFragCoordFromLight.x < 0 || projectedFragCoordFromLight.x > 1 || projectedFragCoordFromLight.y < 0 || projectedFragCoordFromLight.y > 1)
    {
        return 1.0;
    }

    float minLength = 3.0;
    float maxLength = 25.0;

    float zF = (shadowCoord.w - minLength) / (minLength + maxLength);
    float zFSquared = zF * zF;

    // Step 1
    vec4 b = texture(shadowMap, projectedFragCoordFromLight.xy);
    
    // Cutoff light bias
    float shadowBias = 1e-3;
    if (abs(b.x - shadowBias) < epsilon)
    {
        return 0.0;
    }

    float alpha = 1e-3;
    vec4 bPrime = (1.0 - alpha) * b + (alpha * vec4(0.5));

    // Step 2
    vec3 c = CholeskyDecomposition(1.0, bPrime.x, bPrime.y, bPrime.y, bPrime.z, bPrime.w, 1, zF, zFSquared);

    // Step 3
    vec2 solutions = QuadraticEquation(c.z, c.y, c.x);
    float z2 = solutions.x;
    float z3 = solutions.y;

    // Step 4
    if (zF < z2 || abs(zF - z2) < epsilon)
    {
        return 0.0;
    }

    // Step 5
    if (zF < z3 || abs(zF - z3) < epsilon)
    {
        float numerator = zF * z3 - bPrime.x * (zF + z3) + bPrime.y;
        float denominator = (z3 - z2) * (zF - z2);
        return numerator / denominator;
    }
    
    // Step 6
    float numerator = z2 * z3 - bPrime.x * (z2 + z3) + bPrime.y;
    float denominator = (zF - z2) * (zF - z3);
    return 1.0;
}

float VarianceShadowMapCalculation(sampler2D shadowMap, vec4 shadowCoord)
{
    // homogeneuos division
    vec3 projectedFragCoordFromLight = shadowCoord.xyz / shadowCoord.w;
    // Ignore outside shadow map area
    if(projectedFragCoordFromLight.x < 0 || projectedFragCoordFromLight.x > 1 || projectedFragCoordFromLight.y < 0 || projectedFragCoordFromLight.y > 1)
    {
        return 1.0;
    }
    vec4 b = texture(shadowMap, projectedFragCoordFromLight.xy);

    float M1 = b.x;
    float M2 = b.y;

    float minLength = 3.0;
    float maxLength = 25.0;
    float relativeDepth = (shadowCoord.w - minLength) / (minLength + maxLength);

    // One-tailed inequality valid if t > Moments.x
    bool p = relativeDepth <= M1;

    // Compute variance.
    float variance = M2 - (M1 * M1);
    float minVariance = 3e-6;
    variance = max(variance, minVariance);

    // Compute probabilistic upper bound.
    float d = relativeDepth - M1;
    float pMax = variance / (variance + d * d);

    return 1.0 - max(float(p), pMax);
}
