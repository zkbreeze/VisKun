#pragma once

namespace kvs
{

class StructuredVolumeObject;
class UnstructuredVolumeObject;

class CubeToTetrahedra
{
public:
    static UnstructuredVolumeObject* Convert(const StructuredVolumeObject& structure);

}; // CubeToTetrahedra

} // kvs
