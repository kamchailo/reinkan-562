// Helper function to calculate clustered grid

vec4 screenToView(vec4 screen, vec2 screenDimensions, mat4 inverseProjection)
{
    //Convert to NDC [0.0, 1.0]
    vec2 ndcCoord = screen.xy / screenDimensions.xy;

    //Convert to clipSpace [-1.0, 1.0]
    // vec4 clip = vec4(vec2(ndcCoord.x, 1.0 - ndcCoord.y) * 2.0 - 1.0, screen.z, screen.w);
    vec4 clip = vec4(vec2(ndcCoord.x, ndcCoord.y) * 2.0 - 1.0, screen.z, screen.w);

    //View space transform
    vec4 view = inverseProjection * clip;

    //Perspective projection
    view = view / view.w;

    // view.z = -view.z;

    return view;
}

//Creates a line segment from the eye to the screen point, then finds its intersection
//With a z oriented plane located at the given distance to the origin
vec3 lineIntersectionToZPlane(vec3 A, vec3 B, float zDistance)
{
    //all clusters planes are aligned in the same z direction
    vec3 normal = vec3(0.0, 0.0, -1.0);

    //getting the line from the eye to the tile
    vec3 ab =  B - A;

    //Computing the intersection length for the line and the plane
    float t = (zDistance - dot(normal, A)) / dot(normal, ab); // ab.z

    //Computing the actual xyz position of the point along the line
    vec3 result = A + t * ab;

    return result;
}