#version 400 core

layout(vertices = 1) out;

in Data {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
} inData[];

uniform vec3 eyePosition;
uniform float tessFactor;

struct outputPatch
{
    vec3 worldPos_B030;
    vec3 worldPos_B021;
    vec3 worldPos_B012;
    vec3 worldPos_B003;
    vec3 worldPos_B102;
    vec3 worldPos_B201;
    vec3 worldPos_B300;
    vec3 worldPos_B210;
    vec3 worldPos_B120;
    vec3 worldPos_B111;
    vec3 normal[3];
    vec3 tangent[3];
    vec2 texCoord[3];
};

 patch out outputPatch oPatch;

 vec3 projectToPlane(vec3 point, vec3 planePoint, vec3 planeNormal)
 {
     vec3 v = point - planePoint;
     float len = dot(v, planeNormal);
     vec3 d = len * planeNormal;
     return (point - d);
 }




 float edgesLengthSum = 0;
 float edgesLength0 = 0;
 float edgesLength1 = 0;
 float edgesLength2 = 0;


/*

            *B300
           * *
          *   *
         *B210 *B201
        *       *
       *         *
      *120  *111  *102
     *             *
    *               *
   *                 *
  *03   *021   *012   *003


  */


 void CalcPositions()
 {
     // The original vertices stay the same
     oPatch.worldPos_B030 = inData[0].pos;
     oPatch.worldPos_B003 = inData[1].pos;
     oPatch.worldPos_B300 = inData[2].pos;

     // Edges are names according to the opposing vertex
     vec3 edgeB300 = oPatch.worldPos_B003 - oPatch.worldPos_B030;
     vec3 edgeB030 = oPatch.worldPos_B300 - oPatch.worldPos_B003;
     vec3 edgeB003 = oPatch.worldPos_B030 - oPatch.worldPos_B300;

     // Generate two midpoints on each edge
     oPatch.worldPos_B021 = oPatch.worldPos_B030 + edgeB300 / 3.0;
     oPatch.worldPos_B012 = oPatch.worldPos_B030 + edgeB300 * 2.0 / 3.0;
     oPatch.worldPos_B102 = oPatch.worldPos_B003 + edgeB030 / 3.0;
     oPatch.worldPos_B201 = oPatch.worldPos_B003 + edgeB030 * 2.0 / 3.0;
     oPatch.worldPos_B210 = oPatch.worldPos_B300 + edgeB003 / 3.0;
     oPatch.worldPos_B120 = oPatch.worldPos_B300 + edgeB003 * 2.0 / 3.0;

     // Project each midpoint on the plane defined by the nearest vertex and its normal
     oPatch.worldPos_B021 = projectToPlane(oPatch.worldPos_B021, oPatch.worldPos_B030,
                                           oPatch.normal[0]);
     oPatch.worldPos_B012 = projectToPlane(oPatch.worldPos_B012, oPatch.worldPos_B003,
                                          oPatch.normal[1]);
     oPatch.worldPos_B102 = projectToPlane(oPatch.worldPos_B102, oPatch.worldPos_B003,
                                          oPatch.normal[1]);
     oPatch.worldPos_B201 = projectToPlane(oPatch.worldPos_B201, oPatch.worldPos_B300,
                                          oPatch.normal[2]);
     oPatch.worldPos_B210 = projectToPlane(oPatch.worldPos_B210, oPatch.worldPos_B300,
                                          oPatch.normal[2]);
     oPatch.worldPos_B120 = projectToPlane(oPatch.worldPos_B120, oPatch.worldPos_B030,
                                          oPatch.normal[0]);

     // Handle the center
     vec3 Center = (oPatch.worldPos_B003 + oPatch.worldPos_B030 + oPatch.worldPos_B300) / 3.0;
     oPatch.worldPos_B111 = (oPatch.worldPos_B021 + oPatch.worldPos_B012 + oPatch.worldPos_B102 +
                           oPatch.worldPos_B201 + oPatch.worldPos_B210 + oPatch.worldPos_B120) / 6.0;
     oPatch.worldPos_B111 += (oPatch.worldPos_B111 - Center) / 2.0;


     edgesLengthSum += abs(length(oPatch.worldPos_B030 - oPatch.worldPos_B003));
     edgesLengthSum += abs(length(oPatch.worldPos_B003 - oPatch.worldPos_B300));
     edgesLengthSum += abs(length(oPatch.worldPos_B300 - oPatch.worldPos_B030));

     edgesLength0 += abs(length(oPatch.worldPos_B030 - oPatch.worldPos_B003));
     edgesLength1 += abs(length(oPatch.worldPos_B003 - oPatch.worldPos_B300));
     edgesLength2 += abs(length(oPatch.worldPos_B300 - oPatch.worldPos_B030));
 }


 float GetTessLevel(float Distance0, float Distance1)
 {
     float tessLevel = tessFactor * edgesLengthSum / ((Distance0 + Distance1) / 2.0);

     return tessLevel;
 }

 float GetTessLevel0(float Distance0, float Distance1)
 {
     float tessLevel = tessFactor * edgesLength0 / ((Distance0 + Distance1) / 2.0);

     return tessLevel;
 }
 float GetTessLevel1(float Distance0, float Distance1)
 {
     float tessLevel = tessFactor * edgesLength1 / ((Distance0 + Distance1) / 2.0);

     return tessLevel;
 }
 float GetTessLevel2(float Distance0, float Distance1)
 {
     float tessLevel = tessFactor * edgesLength2 / ((Distance0 + Distance1) / 2.0);

     return tessLevel;
 }


void main(void)
{
    //UNROLL LOOP FOR MS INTEL DRIVER COMPLIANCE...
    oPatch.normal[0] = inData[0].normal;
    oPatch.texCoord[0] = inData[0].texcoord;
    oPatch.tangent[0] = inData[0].tangent;

    oPatch.normal[1] = inData[1].normal;
    oPatch.texCoord[1] = inData[1].texcoord;
    oPatch.tangent[1] = inData[1].tangent;

    oPatch.normal[2] = inData[2].normal;
    oPatch.texCoord[2] = inData[2].texcoord;
    oPatch.tangent[2] = inData[2].tangent;


    CalcPositions();

    // Calculate the distance from the camera to the three control points
     float dist0 = distance(eyePosition, inData[0].pos);
     float dist1 = distance(eyePosition, inData[1].pos);
     float dist2 = distance(eyePosition, inData[2].pos);

//     gl_TessLevelOuter[0] = GetTessLevel(dist1, dist2);
//     gl_TessLevelOuter[1] = GetTessLevel(dist2, dist0);
//     gl_TessLevelOuter[2] = GetTessLevel(dist0, dist1);
//     gl_TessLevelInner[0] = gl_TessLevelOuter[2];

     gl_TessLevelOuter[0] = GetTessLevel1(dist1, dist2);
     gl_TessLevelOuter[1] = GetTessLevel2(dist2, dist0);
     gl_TessLevelOuter[2] = GetTessLevel0(dist0, dist1);
     gl_TessLevelInner[0] = GetTessLevel(dist0, dist1);;

}
