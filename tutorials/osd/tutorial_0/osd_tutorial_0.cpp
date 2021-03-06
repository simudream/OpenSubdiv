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


//------------------------------------------------------------------------------
// Tutorial description:
//
// This tutorial demonstrates the manipulation of Osd 'Compute' 'Contexts' and
// 'Controllers'.
//

#include <far/topologyRefinerFactory.h>
#include <far/stencilTablesFactory.h>
#include <osd/cpuComputeContext.h>
#include <osd/cpuComputeController.h>
#include <osd/cpuVertexBuffer.h>

#include <cstdio>
#include <cstring>

//------------------------------------------------------------------------------
// Cube geometry from catmark_cube.h
static float g_verts[24] = {-0.5f, -0.5f,  0.5f,
                             0.5f, -0.5f,  0.5f,
                            -0.5f,  0.5f,  0.5f,
                             0.5f,  0.5f,  0.5f,
                            -0.5f,  0.5f, -0.5f,
                             0.5f,  0.5f, -0.5f,
                            -0.5f, -0.5f, -0.5f,
                             0.5f, -0.5f, -0.5f};

static int g_nverts = 8,
           g_nfaces = 6;

static int g_vertsperface[6] = { 4, 4, 4, 4, 4, 4 };

static int g_vertIndices[24] = { 0, 1, 3, 2,
                                 2, 3, 5, 4,
                                 4, 5, 7, 6,
                                 6, 7, 1, 0,
                                 1, 7, 5, 3,
                                 6, 0, 2, 4  };

using namespace OpenSubdiv;

static Far::TopologyRefiner const * createTopologyRefiner(int maxlevel);

//------------------------------------------------------------------------------
int main(int, char **) {

    int maxlevel=2,
        nCoarseVerts=0,
        nRefinedVerts=0;

    Osd::CpuComputeContext * context=0;

    Far::KernelBatchVector batches;

    //
    // Setup phase
    //
    { // Setup Context
        Far::TopologyRefiner const * refiner = createTopologyRefiner(maxlevel);

        // Setup a factory to create FarStencilTables (for more details see
        // Far tutorials)
        Far::StencilTablesFactory::Options options;
        options.generateOffsets=true;
        options.generateAllLevels=false;

        Far::StencilTables const * stencilTables =
            Far::StencilTablesFactory::Create(*refiner, options);

        // We need a kernel batch to dispatch Compute launches
        batches.push_back(Far::StencilTablesFactory::Create(*stencilTables));

        // Create an Osd Compute Context from the stencil tables
        context = Osd::CpuComputeContext::Create(stencilTables);

        nCoarseVerts = refiner->GetNumVertices(0);
        nRefinedVerts = stencilTables->GetNumStencils();

        // We are done with Far: cleanup tables
        delete refiner;
        delete stencilTables;
    }

    // Setup Controller
    Osd::CpuComputeController controller;

    // Setup a buffer for vertex primvar data:
    Osd::CpuVertexBuffer * vbuffer =
        Osd::CpuVertexBuffer::Create(3, nCoarseVerts + nRefinedVerts);

    //
    // Execution phase (every frame)
    //
    {
        // Pack the control vertex data at the start of the vertex buffer
        // and update every time control data changes
        vbuffer->UpdateData(g_verts, 0, nCoarseVerts);

        // Launch the computation
        controller.Compute(context, batches, vbuffer);
    }

    { // Visualization with Maya : print a MEL script that generates particles
      // at the location of the refined vertices

        printf("particle ");
        float const * refinedVerts = vbuffer->BindCpuBuffer() + 3*nCoarseVerts;
        for (int i=0; i<nRefinedVerts; ++i) {
            float const * vert = refinedVerts + 3*i;
            printf("-p %f %f %f\n", vert[0], vert[1], vert[2]);
        }
        printf("-c 1;\n");
    }

    delete vbuffer;
    delete context;
}

//------------------------------------------------------------------------------
static Far::TopologyRefiner const *
createTopologyRefiner(int maxlevel) {

    // Populate a topology descriptor with our raw data

    typedef Far::TopologyRefinerFactoryBase::TopologyDescriptor Descriptor;

    Sdc::Type type = OpenSubdiv::Sdc::TYPE_CATMARK;

    Sdc::Options options;
    options.SetVVarBoundaryInterpolation(Sdc::Options::VVAR_BOUNDARY_EDGE_ONLY);

    Descriptor desc;
    desc.numVertices  = g_nverts;
    desc.numFaces     = g_nfaces;
    desc.vertsPerFace = g_vertsperface;
    desc.vertIndices  = g_vertIndices;

    // Instantiate a FarTopologyRefiner from the descriptor
    Far::TopologyRefiner * refiner =
        Far::TopologyRefinerFactory<Descriptor>::Create(type, options, desc);

    // Uniformly refine the topolgy up to 'maxlevel'
    refiner->RefineUniform( maxlevel );

    return refiner;
}

//------------------------------------------------------------------------------
