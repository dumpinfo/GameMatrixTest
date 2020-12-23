SHADER version 1
@OpenGL2.Vertex
#version 400
#define VIRTUAL_TEXTURE_STAGES 7
#define MAX_INSTANCES 256

//Uniforms
uniform vec4 materialcolordiffuse;
uniform mat4 projectioncameramatrix;
uniform mat4 camerainversematrix;
uniform instancematrices { mat4 matrix[MAX_INSTANCES];} entity;
uniform float terrainsize;
uniform float texturerange[VIRTUAL_TEXTURE_STAGES];
//uniform float terrainheight;
uniform vec2 renderposition[8];
uniform sampler2D texture0;

//Attributes
in vec3 vertex_position;
in vec4 vertex_color;
in vec3 vertex_normal;

//Outputs
out vec4 ex_position;
out vec4 ex_color;
out float ex_selectionstate;
out vec3 ex_VertexCameraPosition;
out vec3 ex_normal;
out vec2 ex_texcoords[VIRTUAL_TEXTURE_STAGES];
//out vec2 ex_texcoords1;
//out vec2 ex_texcoords2;
//out vec2 ex_texcoords3;

void main()
{
	mat4 entitymatrix = entity.matrix[gl_InstanceID];
	mat4 entitymatrix_=entitymatrix;
	entitymatrix_[0][3]=0.0;
	entitymatrix_[1][3]=0.0;
	entitymatrix_[2][3]=0.0;
	entitymatrix_[3][3]=1.0;
	//entitymatrix_ * 
	//ex_texcoords0.x = 1.0 - ex_texcoords0.x;
	
	vec4 modelvertexposition = entitymatrix_ * (vec4(vertex_position,1.0));
	
	ex_texcoords[0] = (modelvertexposition.xz) / terrainsize + 0.5;
	for (int i=0; i<VIRTUAL_TEXTURE_STAGES; ++i)
	{
		ex_texcoords[i] = (modelvertexposition.xz - renderposition[i]) / texturerange[i] + 0.5;
	}
	
	/*ex_texcoords[1] = (modelvertexposition.xz - renderposition[1]) / texturerange[1] + 0.5;
	ex_texcoords[2] = (modelvertexposition.xz - renderposition[2]) / texturerange[2] + 0.5;
	ex_texcoords[3] = (modelvertexposition.xz - renderposition[3]) / texturerange[3] + 0.5;
	ex_texcoords[4] = (modelvertexposition.xz - renderposition[4]) / texturerange[4] + 0.5;
	ex_texcoords[5] = (modelvertexposition.xz - renderposition[5]) / texturerange[5] + 0.5;
	ex_texcoords[6] = (modelvertexposition.xz - renderposition[6]) / texturerange[6] + 0.5;
	ex_texcoords[7] = (modelvertexposition.xz - renderposition[7]) / texturerange[7] + 0.5;*/
	
	float terrainheight = length(entitymatrix_[1].xyz);
	modelvertexposition.y = texture(texture0, (modelvertexposition.xz+0.5)/ terrainsize + 0.5).r * terrainheight;	
	
	ex_position = modelvertexposition;
	
	ex_VertexCameraPosition = vec3(camerainversematrix * modelvertexposition);
	gl_Position = modelvertexposition;
	//ex_VertexCameraPosition = vec3(camerainversematrix * vec4(vertex_position, 1.0));
	//gl_Position = projectioncameramatrix * entitymatrix_ * vec4(vertex_position, 1.0);
	
	mat3 nmat = mat3(camerainversematrix[0].xyz,camerainversematrix[1].xyz,camerainversematrix[2].xyz);//39
	nmat = nmat * mat3(entitymatrix[0].xyz,entitymatrix[1].xyz,entitymatrix[2].xyz);//40
	ex_normal = (nmat * vertex_normal);	
	
	ex_color = vec4(entitymatrix[0][3],entitymatrix[1][3],entitymatrix[2][3],entitymatrix[3][3]);
	
	//If an object is selected, 10 is subtracted from the alpha color.
	//This is a bit of a hack that packs a per-object boolean into the alpha value.
	ex_selectionstate = 0.0;
	if (ex_color.a<-5.0)
	{
		ex_color.a += 10.0;
		ex_selectionstate = 1.0;
	}
	ex_color *= vec4(1.0-vertex_color.r,1.0-vertex_color.g,1.0-vertex_color.b,vertex_color.a) * materialcolordiffuse;
}
@OpenGL2.Fragment
#version 400
#define MAX_INSTANCES 256

layout(vertices = 3) out;

uniform vec2 camerarange;
uniform vec2 buffersize;
uniform float cameratheta;
uniform vec3 cameraposition;
uniform mat4 camerainversematrix;
uniform float tessstrength;
uniform int LODLevel;

/*in vec2 ex_texcoords0[];
in vec2 ex_texcoords1[];
in vec2 ex_texcoords2[];
in vec2 ex_texcoords3[];
in vec2 ex_texcoords4[];
in vec2 ex_texcoords5[];
in vec2 ex_texcoords6[];
in vec2 ex_texcoords7[];*/
//in mat3 nmat[];
//in float clipdistance0[];
//flat in int ex_instanceID[];
in vec4 ex_position[];
//in vec3 vertexposminuscamerapos[];

/*out vec2 e_texcoords0[];
out vec2 e_texcoords1[];
out vec2 e_texcoords2[];
out vec2 e_texcoords3[];
out vec2 e_texcoords4[];
out vec2 e_texcoords5[];
out vec2 e_texcoords6[];
out vec2 e_texcoords7[];*/
//out mat3 e_nmat[];
//out float e_clipdistance0[];
//flat out int instanceID[];
out vec4 e_position[];
//out vec3 e_vertexposminuscamerapos[];

void main()
{
//	e_vertexposminuscamerapos[gl_InvocationID]=vertexposminuscamerapos[gl_InvocationID];
	e_position[gl_InvocationID]=ex_position[gl_InvocationID];
	//instanceID[gl_InvocationID]=ex_instanceID[gl_InvocationID];
	/*e_texcoords0[gl_InvocationID] = ex_texcoords0[gl_InvocationID];
	e_texcoords1[gl_InvocationID] = ex_texcoords1[gl_InvocationID];
	e_texcoords2[gl_InvocationID] = ex_texcoords2[gl_InvocationID];
	e_texcoords3[gl_InvocationID] = ex_texcoords3[gl_InvocationID];
	e_texcoords4[gl_InvocationID] = ex_texcoords4[gl_InvocationID];
	e_texcoords5[gl_InvocationID] = ex_texcoords5[gl_InvocationID];
	e_texcoords6[gl_InvocationID] = ex_texcoords6[gl_InvocationID];
	e_texcoords7[gl_InvocationID] = ex_texcoords7[gl_InvocationID];*/
	//e_nmat[gl_InvocationID] = nmat[gl_InvocationID];
	//e_clipdistance0[gl_InvocationID] = clipdistance0[gl_InvocationID];	
	
	if (gl_InvocationID==0)
	{
		vec3 pos = (ex_position[gl_InvocationID]).xyz;
		float dist = length(cameraposition - pos);	
		
		if (tessstrength==0.0 || dist>50)
		{
			gl_TessLevelInner[0] = 1.0;
			gl_TessLevelInner[1] = 1.0;
			gl_TessLevelOuter[0] = 1.0;
			gl_TessLevelOuter[1] = 1.0;
			gl_TessLevelOuter[2] = 1.0;
			gl_TessLevelOuter[3] = 1.0;
		}
		else
		{
			//Screen-space tessellation - displays roughly constant sized polys
			float tess;
			float maxtess = 8.0 * tessstrength;	
			maxtess = 8.0 * tessstrength;
			float polygonsize=0.02 / 3.0 * tessstrength;
			float pixelspertessellation = 0.25;	
			float screensize = polygonsize / (camerarange.y*2.0) * buffersize.y / (dist / camerarange.y) / cameratheta / pixelspertessellation;//camerarange.y);//3.0;//pos.z * 64.0;//polygonsize / pos.z /  /** buffersize.x*/ / cameratheta;
			screensize=max(1.0,screensize);
			tess = max(screensize,1);
			tess = min(tess,maxtess);
			gl_TessLevelInner[0] = tess;
			gl_TessLevelInner[1] = tess;			
			gl_TessLevelOuter[0] = maxtess;
			gl_TessLevelOuter[1] = maxtess;
			gl_TessLevelOuter[2] = maxtess;
			gl_TessLevelOuter[3] = maxtess;
		}
	}
}
@OpenGLES2.Vertex
#version 400
#define MAX_INSTANCES 256
#define VIRTUAL_TEXTURE_STAGES 7

layout(triangles,fractional_odd_spacing,ccw) in;//, equal_spacing, ccw) in;

uniform instancematrices { mat4 matrix[MAX_INSTANCES];} entity;
uniform mat4 projectioncameramatrix;
uniform mat4 camerainversematrix;
uniform float terrainsize;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform sampler2D texture8;
uniform sampler2D texture9;
uniform sampler2D texture10;
uniform sampler2D texture11;
uniform sampler2D texture12;
uniform sampler2D texture13;
uniform sampler2D texture14;
uniform sampler2D texture15;
uniform float texturerange[VIRTUAL_TEXTURE_STAGES];
uniform vec3 cameraposition;

in vec2 e_texcoords0[];
in vec2 e_texcoords1[];
in vec2 e_texcoords2[];
in vec2 e_texcoords3[];
in vec2 e_texcoords4[];
in vec2 e_texcoords5[];
in vec2 e_texcoords6[];
in vec2 e_texcoords7[];
in mat3 e_nmat[];
in float e_clipdistance0[];
flat in int instanceID[];
in vec4 e_position[];
in vec3 e_vertexposminuscamerapos[];

void main()
{
	vec4 pos = e_position[0] * gl_TessCoord.x + e_position[1] * gl_TessCoord.y + e_position[2] * gl_TessCoord.z;
	gl_Position = projectioncameramatrix * pos;	
}
@OpenGLES2.Fragment

@OpenGL4.Vertex
#version 400
#define VIRTUAL_TEXTURE_STAGES 7
#define MAX_INSTANCES 256

//Uniforms
uniform vec4 materialcolordiffuse;
uniform mat4 projectioncameramatrix;
uniform mat4 camerainversematrix;
uniform instancematrices { mat4 matrix[MAX_INSTANCES];} entity;
uniform float terrainsize;
uniform float texturerange[VIRTUAL_TEXTURE_STAGES];
//uniform float terrainheight;
uniform vec2 renderposition[8];
uniform sampler2D texture0;

//Attributes
in vec3 vertex_position;
in vec4 vertex_color;
in vec3 vertex_normal;

//Outputs
out vec4 ex_position;
out vec4 ex_color;
out float ex_selectionstate;
out vec3 ex_VertexCameraPosition;
out vec3 ex_normal;
out vec2 ex_texcoords[VIRTUAL_TEXTURE_STAGES];
//out vec2 ex_texcoords1;
//out vec2 ex_texcoords2;
//out vec2 ex_texcoords3;

void main()
{
	mat4 entitymatrix = entity.matrix[gl_InstanceID];
	mat4 entitymatrix_=entitymatrix;
	entitymatrix_[0][3]=0.0;
	entitymatrix_[1][3]=0.0;
	entitymatrix_[2][3]=0.0;
	entitymatrix_[3][3]=1.0;
	//entitymatrix_ * 
	//ex_texcoords0.x = 1.0 - ex_texcoords0.x;
	
	vec4 modelvertexposition = entitymatrix_ * (vec4(vertex_position,1.0));
	
	ex_texcoords[0] = (modelvertexposition.xz) / terrainsize + 0.5;
	for (int i=0; i<VIRTUAL_TEXTURE_STAGES; ++i)
	{
		ex_texcoords[i] = (modelvertexposition.xz - renderposition[i]) / texturerange[i] + 0.5;
	}
	
	/*ex_texcoords[1] = (modelvertexposition.xz - renderposition[1]) / texturerange[1] + 0.5;
	ex_texcoords[2] = (modelvertexposition.xz - renderposition[2]) / texturerange[2] + 0.5;
	ex_texcoords[3] = (modelvertexposition.xz - renderposition[3]) / texturerange[3] + 0.5;
	ex_texcoords[4] = (modelvertexposition.xz - renderposition[4]) / texturerange[4] + 0.5;
	ex_texcoords[5] = (modelvertexposition.xz - renderposition[5]) / texturerange[5] + 0.5;
	ex_texcoords[6] = (modelvertexposition.xz - renderposition[6]) / texturerange[6] + 0.5;
	ex_texcoords[7] = (modelvertexposition.xz - renderposition[7]) / texturerange[7] + 0.5;*/
	
	float terrainheight = length(entitymatrix_[1].xyz);
	modelvertexposition.y = texture(texture0, (modelvertexposition.xz+0.5)/ terrainsize + 0.5).r * terrainheight;	
	
	ex_position = modelvertexposition;
	
	ex_VertexCameraPosition = vec3(camerainversematrix * modelvertexposition);
	gl_Position = modelvertexposition;
	//ex_VertexCameraPosition = vec3(camerainversematrix * vec4(vertex_position, 1.0));
	//gl_Position = projectioncameramatrix * entitymatrix_ * vec4(vertex_position, 1.0);
	
	mat3 nmat = mat3(camerainversematrix[0].xyz,camerainversematrix[1].xyz,camerainversematrix[2].xyz);//39
	nmat = nmat * mat3(entitymatrix[0].xyz,entitymatrix[1].xyz,entitymatrix[2].xyz);//40
	ex_normal = (nmat * vertex_normal);	
	
	ex_color = vec4(entitymatrix[0][3],entitymatrix[1][3],entitymatrix[2][3],entitymatrix[3][3]);
	
	//If an object is selected, 10 is subtracted from the alpha color.
	//This is a bit of a hack that packs a per-object boolean into the alpha value.
	ex_selectionstate = 0.0;
	if (ex_color.a<-5.0)
	{
		ex_color.a += 10.0;
		ex_selectionstate = 1.0;
	}
	ex_color *= vec4(1.0-vertex_color.r,1.0-vertex_color.g,1.0-vertex_color.b,vertex_color.a) * materialcolordiffuse;
}
@OpenGL4.Control
#version 400
#define MAX_INSTANCES 256

layout(vertices = 3) out;

uniform vec2 camerarange;
uniform vec2 buffersize;
uniform float cameratheta;
uniform vec3 cameraposition;
uniform mat4 camerainversematrix;
uniform float tessstrength;
uniform int LODLevel;

/*in vec2 ex_texcoords0[];
in vec2 ex_texcoords1[];
in vec2 ex_texcoords2[];
in vec2 ex_texcoords3[];
in vec2 ex_texcoords4[];
in vec2 ex_texcoords5[];
in vec2 ex_texcoords6[];
in vec2 ex_texcoords7[];*/
//in mat3 nmat[];
//in float clipdistance0[];
//flat in int ex_instanceID[];
in vec4 ex_position[];
//in vec3 vertexposminuscamerapos[];

/*out vec2 e_texcoords0[];
out vec2 e_texcoords1[];
out vec2 e_texcoords2[];
out vec2 e_texcoords3[];
out vec2 e_texcoords4[];
out vec2 e_texcoords5[];
out vec2 e_texcoords6[];
out vec2 e_texcoords7[];*/
//out mat3 e_nmat[];
//out float e_clipdistance0[];
//flat out int instanceID[];
out vec4 e_position[];
//out vec3 e_vertexposminuscamerapos[];

void main()
{
//	e_vertexposminuscamerapos[gl_InvocationID]=vertexposminuscamerapos[gl_InvocationID];
	e_position[gl_InvocationID]=ex_position[gl_InvocationID];
	//instanceID[gl_InvocationID]=ex_instanceID[gl_InvocationID];
	/*e_texcoords0[gl_InvocationID] = ex_texcoords0[gl_InvocationID];
	e_texcoords1[gl_InvocationID] = ex_texcoords1[gl_InvocationID];
	e_texcoords2[gl_InvocationID] = ex_texcoords2[gl_InvocationID];
	e_texcoords3[gl_InvocationID] = ex_texcoords3[gl_InvocationID];
	e_texcoords4[gl_InvocationID] = ex_texcoords4[gl_InvocationID];
	e_texcoords5[gl_InvocationID] = ex_texcoords5[gl_InvocationID];
	e_texcoords6[gl_InvocationID] = ex_texcoords6[gl_InvocationID];
	e_texcoords7[gl_InvocationID] = ex_texcoords7[gl_InvocationID];*/
	//e_nmat[gl_InvocationID] = nmat[gl_InvocationID];
	//e_clipdistance0[gl_InvocationID] = clipdistance0[gl_InvocationID];	
	
	if (gl_InvocationID==0)
	{
		vec3 pos = (ex_position[gl_InvocationID]).xyz;
		float dist = length(cameraposition - pos);	
		
		if (tessstrength==0.0 || dist>50)
		{
			gl_TessLevelInner[0] = 1.0;
			gl_TessLevelInner[1] = 1.0;
			gl_TessLevelOuter[0] = 1.0;
			gl_TessLevelOuter[1] = 1.0;
			gl_TessLevelOuter[2] = 1.0;
			gl_TessLevelOuter[3] = 1.0;
		}
		else
		{
			//Screen-space tessellation - displays roughly constant sized polys
			float tess;
			float maxtess = 8.0 * tessstrength;	
			maxtess = 8.0 * tessstrength;
			float polygonsize=0.02 / 3.0 * tessstrength;
			float pixelspertessellation = 0.25;	
			float screensize = polygonsize / (camerarange.y*2.0) * buffersize.y / (dist / camerarange.y) / cameratheta / pixelspertessellation;//camerarange.y);//3.0;//pos.z * 64.0;//polygonsize / pos.z /  /** buffersize.x*/ / cameratheta;
			screensize=max(1.0,screensize);
			tess = max(screensize,1);
			tess = min(tess,maxtess);
			gl_TessLevelInner[0] = tess;
			gl_TessLevelInner[1] = tess;			
			gl_TessLevelOuter[0] = maxtess;
			gl_TessLevelOuter[1] = maxtess;
			gl_TessLevelOuter[2] = maxtess;
			gl_TessLevelOuter[3] = maxtess;
		}
	}
}
@OpenGL4.Evaluation
#version 400
#define MAX_INSTANCES 256
#define VIRTUAL_TEXTURE_STAGES 7

layout(triangles,fractional_odd_spacing,ccw) in;//, equal_spacing, ccw) in;

uniform instancematrices { mat4 matrix[MAX_INSTANCES];} entity;
uniform mat4 projectioncameramatrix;
uniform mat4 camerainversematrix;
uniform float terrainsize;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform sampler2D texture4;
uniform sampler2D texture5;
uniform sampler2D texture6;
uniform sampler2D texture7;
uniform sampler2D texture8;
uniform sampler2D texture9;
uniform sampler2D texture10;
uniform sampler2D texture11;
uniform sampler2D texture12;
uniform sampler2D texture13;
uniform sampler2D texture14;
uniform sampler2D texture15;
uniform float texturerange[VIRTUAL_TEXTURE_STAGES];
uniform vec3 cameraposition;

in vec2 e_texcoords0[];
in vec2 e_texcoords1[];
in vec2 e_texcoords2[];
in vec2 e_texcoords3[];
in vec2 e_texcoords4[];
in vec2 e_texcoords5[];
in vec2 e_texcoords6[];
in vec2 e_texcoords7[];
in mat3 e_nmat[];
in float e_clipdistance0[];
flat in int instanceID[];
in vec4 e_position[];
in vec3 e_vertexposminuscamerapos[];

void main()
{
	vec4 pos = e_position[0] * gl_TessCoord.x + e_position[1] * gl_TessCoord.y + e_position[2] * gl_TessCoord.z;
	gl_Position = projectioncameramatrix * pos;	
}
@OpenGL4.Fragment
#version 400

out vec4 fragData0;

in float ty;

uniform sampler2D texture0;

void main(void)
{
	fragData0 = vec4(0.0,0.5,0.0,1.0);
}
