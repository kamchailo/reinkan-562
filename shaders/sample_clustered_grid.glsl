//Input
uint tileSizePx;  // How many pixels in x does a square tile use
uint tileIndex;   // Linear ID of the thread/cluster
float zFar;       // Far plane distance in view space depth
float zNear;      // Near plane distance in view space depth

//Output
struct cluster{   // A cluster volume is represented using an AABB
   vec4 minPoint; // We use vec4s instead of a vec3 for memory alignment purposes
   vec4 maxPoint;
}
cluster[];        // A linear list of AABB's with size = numclusters

//Each cluster has it's own thread ID in x, y and z
//We dispatch 16x9x24 threads, one thread per cluster
// !!WRONG layout (local_size_x = 16, local_size_y = 9, local_size_z = 24) in;
// We already dispatch vkCmdDispatch(commandBuffer, 16, 9, 24);
// We could do somethig like 
// - vkCmdDispatch(commandBuffer, 16, 9, 1);
// - layout (local_size_x = 1, local_size_y = 1, local_size_z = 24) in;
// - has to benchmark the performance

void main(){
    //Eye position is zero in view space
    const vec3 eyePos = vec3(0.0);

    //Per cluster variables
    uint tileSizePx = tileSizes[3];
    uint tileIndex  = gl_GlobalInvocationID;

    //Calculating the min and max point in screen space
    vec4 maxPoint_sS = vec4(vec2(gl_WorkGroupID.x + 1,
                                 gl_WorkGroupID.y + 1) * tileSizePx,
                                 -1.0, 1.0); // Top Right
    vec4 minPoint_sS = vec4(gl_WorkGroupID.xy * tileSizePx,
                            -1.0, 1.0); // Bottom left

    //Pass min and max to view space
    vec3 maxPoint_vS = screen2View(maxPoint_sS).xyz;
    vec3 minPoint_vS = screen2View(minPoint_sS).xyz;

    //Near and far values of the cluster in view space
    //We use equation (2) directly to obtain the tile values
    float tileNear  = -zNear * pow(zFar/ zNear, gl_WorkGroupID.z/float(gl_NumWorkGroups.z));
    float tileFar   = -zNear * pow(zFar/ zNear, (gl_WorkGroupID.z + 1) /float(gl_NumWorkGroups.z));

    //Finding the 4 intersection points made from each point to the cluster near/far plane
    vec3 minPointNear = lineIntersectionToZPlane(eyePos, minPoint_vS, tileNear );
    vec3 minPointFar  = lineIntersectionToZPlane(eyePos, minPoint_vS, tileFar );
    vec3 maxPointNear = lineIntersectionToZPlane(eyePos, maxPoint_vS, tileNear );
    vec3 maxPointFar  = lineIntersectionToZPlane(eyePos, maxPoint_vS, tileFar );

    vec3 minPointAABB = min(min(minPointNear, minPointFar),min(maxPointNear, maxPointFar));
    vec3 maxPointAABB = max(max(minPointNear, minPointFar),max(maxPointNear, maxPointFar));

    //Saving the AABB at the tile linear index
    //Cluster is just a SSBO made of a struct of 2 vec4's
    cluster[tileIndex].minPoint  = vec4(minPointAABB , 0.0);
    cluster[tileIndex].maxPoint  = vec4(maxPointAABB , 0.0);
}

//Input:
vec2 screenDimensions;  // Size of the screen in pixels for width and height
mat4 inverseProjection; // The inverse of the projection matrix to get us to view space

//Output:
vec4 view; //The output vector transformed to view space

//Changes a points coordinate system from screen space to view space
vec4 screen2View(vec4 screen){
    //Convert to NDC
    vec2 texCoord = screen.xy / screenDimensions.xy;

    //Convert to clipSpace
    vec4 clip = vec4(vec2(texCoord.x, 1.0 - texCoord.y)* 2.0 - 1.0, screen.z, screen.w);

    //View space transform
    vec4 view = inverseProjection * clip;

    //Perspective projection
    view = view / view.w;

    return view;
}