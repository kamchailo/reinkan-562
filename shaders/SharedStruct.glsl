
struct PushConstant
{
    mat4 modelMatrix;
    mat4 normalMatrix;
	int objectId;
    int materialId;
    uint debugFlag;
    float debugFloat;
    float debugFloat2;
    int debugInt;
};

struct Material
{    
    vec3 diffuse;
    uint diffuse_padding;
    vec3 specular;
    uint specular_padding;
    uint normalMapId;
    uint heightMapId;
    float shininess;
    uint diffuseMapId;
    uint pyramidalHeightMapId;
};

struct UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 viewInverse;
    mat4 proj;
    mat4 shadowProjectionViewMatrix;
    vec3 globalLightPosition;
    uint globalLightPosition_padding;
    vec2 screenExtent;
};

struct LightObject
{
    vec3	position;
    uint    position_padding;
    vec3	color;
    uint    color_padding;
    float	intensity;
    float   radius;
};

struct ClusterGrid
{
    vec3	minPosition;
    uint    minPos_padding;
    vec3	maxPosition;
    uint    maxPos_padding;
};

struct ClusterPlane
{
    float	zNear;
    uint    zNear_padding;
    float	zFar;
    uint    zFar_padding;
};

struct LightGrid
{
    uint offset;
    uint size;
};

struct PushConstantShadow
{
    mat4 modelMatrix;
    vec2 screenExtent;
};

struct PushConstantVLight
{
    vec4 lightPosition;
    vec4 lightConstant;
    vec4 cameraPosition;
    vec2 shadowMapExtent;
};

struct PushConstantPost
{
    vec2 screenExtent;
    uint debugFlag;
    float debugFloat;
    float debugFloat2;
    float debugFloat3;
};