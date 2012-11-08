#include "CubeToTetrahedra.h"
#include <kvs/StructuredVolumeObject>
#include <kvs/UnstructuredVolumeObject>
#include <kvs/ValueArray>

namespace
{
    using namespace kvs;
    UInt32 CalcNodeId(size_t x, size_t y, size_t z, unsigned int i, const StructuredVolumeObject& volume)
    {
        return (x + (i + x) % 2) + (y + (i % 4 / 2 + y) % 2) * volume.nnodesPerLine() + (z + (i % 8 / 4 + z) % 2) * volume.nnodesPerSlice();
    }
}

namespace kvs
{

UnstructuredVolumeObject* CubeToTetrahedra::Convert(const StructuredVolumeObject& in_volume)
{
    if (in_volume.gridType() != VolumeObjectBase::Uniform)
    {
        throw "Grid type of input volume is not uniform grid.";
    }

    UnstructuredVolumeObject* out_volume = new UnstructuredVolumeObject;

    Vector3ui res = in_volume.resolution();
    size_t ncells = (res.x() - 1) * (res.y() - 1) * (res.z() - 1) * 5;
    size_t nconnections = ncells * 4;

    UnstructuredVolumeObject::Connections connections(nconnections);
    VolumeObjectBase::Coords coords(in_volume.nnodes() * 3);

    size_t coord_index = 0;
    for (size_t z = 0; z < res.z(); ++z)
    {
        for (size_t y = 0; y < res.y(); ++y)
        {
            for (size_t x = 0; x < res.x(); ++x)
            {
                coords[coord_index + 0] = static_cast<float>(x);
                coords[coord_index + 1] = static_cast<float>(y);
                coords[coord_index + 2] = static_cast<float>(z);
                coord_index += 3;
            }
        }
    }

    size_t connection_index = 0;
    for (size_t z = 0; z < res.z() - 1; ++z)
    {
        for (size_t y = 0; y < res.y() - 1; ++y)
        {
            for (size_t x = 0; x < res.x() - 1; ++x)
            {
                // Devide one cubic cell to five tetrahedral cell.
                UInt32 node_id[8];
                for (unsigned int node = 0; node < 8; ++node)
                {
                    node_id[node] = ::CalcNodeId(x, y, z, node, in_volume);
                }

                const int n_sub_cells = 5;
                const int n_nodes_of_tet_cell = 4;
                const int sub_cell_connection[n_sub_cells][n_nodes_of_tet_cell]
                    = { {1, 2, 4, 0},
                        {4, 7, 1, 5},
                        {2, 1, 7, 3},
                        {7, 4, 2, 6},
                        {1, 4, 2, 7} };

                for (int sub_cell = 0; sub_cell < n_sub_cells; ++sub_cell)
                {
                    for (int node = 0; node < n_nodes_of_tet_cell; ++node)
                    {
                        connections[connection_index] = node_id[sub_cell_connection[sub_cell][node]];
                        ++connection_index;
                    }
                }
            }
        }
    }
    out_volume->setNCells(ncells);
    out_volume->setMinMaxValues(in_volume.minValue(), in_volume.maxValue());
    out_volume->setVeclen(in_volume.veclen());
    out_volume->setCoords(coords);
    out_volume->setValues(in_volume.values());
    out_volume->setNNodes(in_volume.nnodes());
    out_volume->setConnections(connections);
    out_volume->setCellType(VolumeObjectBase::Tetrahedra);
    out_volume->updateMinMaxCoords();
    out_volume->updateMinMaxValues();

    return out_volume;
}

} // kvs
