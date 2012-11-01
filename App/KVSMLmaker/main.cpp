#include<stdio.h>
#include<fstream>
#include<iostream>
using namespace std;

int main()
{
  char* buffer1=new char[256];
  char* buffer2=new char[256];
  string filename="Y_BsplineBlock3";
  for(int i=1;i<=2;i++)
    {
      sprintf(buffer1,"_%04d.kvsml",i);
      string str_kvsml =filename + buffer1;
      sprintf(buffer2,"_%04d_value.dat",i);
      string str_dat =filename + buffer2;
      ofstream outs(str_kvsml.c_str(),ofstream::out);
      outs<<"<KVSML>    <Object type=\"UnstructuredVolumeObject\">     <UnstructuredVolumeObject cell_type=\"tetrahedra\">            <Node nnodes=\"7789241\">                <Value veclen=\"1\">                    <DataArray type=\"float\" file=\"";
      outs<<str_dat<<"\" format=\"binary\"/>                </Value>                <Coord>                    <DataArray type=\"float\" file=\"Y_BsplineBlock3_coord.dat\" format=\"binary\"/>                </Coord>            </Node>            <Cell ncells=\"36864000\">                <Connection>                    <DataArray type=\"uint\" file=\"Y_BsplineBlock3_connect.dat\" format=\"binary\"/>                </Connection>	  </Cell>        </UnstructuredVolumeObject>    </Object></KVSML>";
    }
}
