//
//   Copyright 2013 Pixar
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//

layout(std140) uniform Transform {
    mat4 ModelViewMatrix;
    mat4 ProjectionMatrix;
    mat4 ModelViewProjectionMatrix;
};

//--------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------
#ifdef VERTEX_SHADER

layout (location=0) in vec4 position;
layout (location=1) in vec4 data;

out block {
    vec4 position;
    vec4 data;
} outpt;

void main()
{
    outpt.position = ModelViewMatrix * position;
    outpt.data = data;
}

#endif


//--------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------
#ifdef GEOMETRY_SHADER

layout(points) in;

#define NVERTS 4

layout(triangle_strip, max_vertices = NVERTS) out;

in block {
    vec4 position;
    vec4 data;
} inpt[NVERTS];

out block {
    vec4 position;
    centroid vec2 uv;
    flat int colorId;
} outpt;

void emit(int index, vec2 offset, vec2 uv)
{
    outpt.position = inpt[0].position;
    outpt.uv = uv;
    outpt.colorId = int(inpt[0].data.w);

    gl_Position = (ProjectionMatrix * inpt[0].position) + vec4(offset, -0.01, 0.0);
    EmitVertex();
}

#define FONT_TEXTURE_WIDTH 128
#define FONT_TEXTURE_HEIGHT 128
#define FONT_TEXTURE_COLUMNS 16
#define FONT_TEXTURE_ROWS 8

#define FONT_CHAR_WIDTH (FONT_TEXTURE_WIDTH/FONT_TEXTURE_COLUMNS)
#define FONT_CHAR_HEIGHT (FONT_TEXTURE_HEIGHT/FONT_TEXTURE_ROWS)

vec2 computeUV( int c ) 
{
    c = c % 0x7f;

    return vec2( float(c%FONT_TEXTURE_COLUMNS)/float(FONT_TEXTURE_COLUMNS),
                 float(c/FONT_TEXTURE_COLUMNS)/float(FONT_TEXTURE_ROWS)     );

}

uniform float scale=0.03;

void main()
{
    gl_PrimitiveID = gl_PrimitiveIDIn;

    vec2 uv = computeUV(int(inpt[0].data.z));

    vec2 dim = vec2(1.0/FONT_TEXTURE_COLUMNS, 
                    1.0/FONT_TEXTURE_ROWS);

    vec2 ofs = inpt[0].data.xy;

    emit(0, scale * (vec2( 1.0, -2.0)+ofs), uv + dim);
    emit(1, scale * (vec2( 1.0,  2.0)+ofs), vec2(uv.x+dim.x, uv.y));
    emit(2, scale * (vec2(-1.0, -2.0)+ofs), vec2(uv.x,       uv.y+dim.y));
    emit(3, scale * (vec2(-1.0,  2.0)+ofs), uv);

    EndPrimitive();
}

#endif

//--------------------------------------------------------------
// Fragment Shader
//--------------------------------------------------------------
#ifdef FRAGMENT_SHADER

in block {
    vec4 position;
    centroid vec2 uv;
    flat int colorId;
} inpt;

uniform sampler2D font;

out vec4 outColor;
out vec3 outNormal;

const vec4 colors[4] = vec4[4](vec4(0.9,0.9,0.9,1.0),
                               vec4(1.0,0.3,0.3,1.0),
                               vec4(0.3,1.0,0.3,1.0),
                               vec4(0.3,0.3,1.0,1.0));

void main()
{
    vec4 color = texture2D(font, inpt.uv);
    if (color.a == 0.0) discard;

    outColor = color * colors[inpt.colorId];
    outNormal = vec3(0.0,0.0,1.0);
    //outColor = vec4(inpt.v.uv,0.0,1.0);
}

#endif
