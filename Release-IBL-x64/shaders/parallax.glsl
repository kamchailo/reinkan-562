// Parallax Occlusion Funcitons

ivec2 GetTile (vec2 uv, int maxLevel, int level)
{
    float pixelSize = pow(2, maxLevel - level);

    return ivec2(uv * pixelSize);
}

ivec2 GetTileDifferent (vec2 uvA, vec2 uvB, int maxLevel, int level)
{
    float pixelSize = pow(2, maxLevel - level);

    //
    // By Multiply with pixelSize 
    // will determine which pixel
    // the uv is located
    //
    ivec2 tileA = ivec2(floor(uvA * pixelSize));
    ivec2 tileB = ivec2(floor(uvB * pixelSize));

    return tileA - tileB;
}

vec3 StopAtTileBorder(vec3 pPrime, vec3 pTemp, int maxLevel, int level)
{
    float pixelSize = pow(2, maxLevel - level);
    ivec2 tilePPrime = ivec2(pPrime * pixelSize);
    float scale;
    vec3 direction = pTemp - pPrime;
    float offset = 0.001;
    // Horizontal Vector
    if(abs(direction.x) > abs(direction.y))
    {
        float borderX;

        // Right Vector
        if(direction.x > 0)
        {
            // borderX in UV
            borderX = (tilePPrime.x + 1) / pixelSize;
        }
        // Left Vector
        else
        {
            // borderX in UV
            borderX = (tilePPrime.x) / pixelSize;
        }
        
        scale = borderX / pPrime.x + offset;
    }
    // Vertical Vector
    else // abs(y) > abs(x)
    {
        float borderY;

        // Up Vector
        if(direction.y > 0)
        {
            borderY = (tilePPrime.y + 1) / pixelSize;
        }
        // Down Vector
        else
        {
            borderY = (tilePPrime.y) / pixelSize;
        }
        
        scale = borderY / pPrime.y + offset;
    }

    return  pPrime * scale;
}

vec3 AcrossNode(vec3 pPrime, vec3 pTemp, int maxLevel, int level)
{
    float pixelSize = pow(2, maxLevel - level);
    ivec2 tilePPrime = ivec2(pPrime * pixelSize);

    vec3 direction = normalize(pTemp - pPrime);

    ivec3 iDirection = ivec3(ceil(direction));

    float xBorder = (tilePPrime.x + iDirection.x) / pixelSize;
    float yBorder = (tilePPrime.y + iDirection.y) / pixelSize;

    float scaleX = xBorder / pPrime.x;
    float scaleY = yBorder / pPrime.y;

    float deltaX = abs(xBorder - pPrime.x);
    float deltaY = abs(yBorder - pPrime.y);
    // return vec3(scaleX, scaleY, 0.0);
    // return iDirection;
    
    if(deltaX < deltaY)
    {
        return pPrime * scaleX;
    }
    else
    {
        return pPrime * scaleY;
    }
}