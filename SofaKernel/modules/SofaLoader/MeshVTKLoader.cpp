/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, development version     *
*                (c) 2006-2016 INRIA, USTL, UJF, CNRS, MGH                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                               SOFA :: Modules                               *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include <sofa/core/ObjectFactory.h>
#include <SofaLoader/MeshVTKLoader.h>
#include <sofa/core/visual/VisualParams.h>

#include <iostream>
#include <cstdio>
#include <sstream>

//XML VTK Loader
#define checkError(A) if (!A) { return false; }
#define checkErrorPtr(A) if (!A) { return NULL; }
#define checkErrorMsg(A, B) if (!A) { msg_error("MeshVTKLoader") << B << "\n" ; return false; }

namespace sofa
{

namespace component
{

namespace loader
{

using namespace sofa::defaulttype;
using sofa::core::objectmodel::BaseData ;
using sofa::core::objectmodel::BaseObject ;
using sofa::defaulttype::Vector3 ;
using sofa::defaulttype::Vec ;
using std::istringstream;
using std::istream;
using std::ofstream;
using std::string;
using helper::vector;

//////////////////// PRIVATE CLASS FOR INTERNAL USE BY MeshVTKLoader ////////
enum VTKDatasetFormat { IMAGE_DATA, STRUCTURED_POINTS,
                        STRUCTURED_GRID, RECTILINEAR_GRID,
                        POLYDATA, UNSTRUCTURED_GRID };

class BaseVTKReader : public BaseObject
{
public:
    class BaseVTKDataIO : public BaseObject
    {
    public:
        string name;
        int dataSize;
        int nestedDataSize;
        BaseVTKDataIO() : dataSize(0), nestedDataSize(1) {}
        virtual ~BaseVTKDataIO() {}
        virtual void resize(int n) = 0;
        virtual bool read(istream& f, int n, int binary) = 0;
        virtual bool read(const string& s, int n, int binary) = 0;
        virtual bool read(const string& s, int binary) = 0;
        virtual bool write(ofstream& f, int n, int groups, int binary) = 0;
        virtual const void* getData() = 0;
        virtual void swap() = 0;

        virtual BaseData* createSofaData() = 0;
    };

    template<class T>
    class VTKDataIO : public BaseVTKDataIO
    {
    public:
        T* data;
        VTKDataIO() : data(NULL) {}
        ~VTKDataIO() { if (data) delete[] data; }
        virtual const void* getData()
        {
            return data;
        }

        virtual void resize(int n)
        {
            if (dataSize != n)
            {
                if (data) delete[] data;
                data = new T[n];
            }

            dataSize = n;
        }
        static T swapT(T t, int nestedDataSize)
        {
            T revT;
            char* revB = (char*) &revT;
            const char* tmpB = (char*) &t;

            if (nestedDataSize < 2)
            {
                for (unsigned int c=0; c<sizeof(T); ++c)
                    revB[c] = tmpB[sizeof(T)-1-c];
            }
            else
            {
                int singleSize = sizeof(T)/nestedDataSize;
                for (int i=0; i<nestedDataSize; ++i)
                {
                    for (unsigned int c=0; c<sizeof(T); ++c)
                        revB[c+i*singleSize] = tmpB[(sizeof(T)-1-c) + i*singleSize];
                }

            }
            return revT;

        }
        void swap()
        {
            for (int i=0; i<dataSize; ++i)
                data[i] = swapT(data[i], nestedDataSize);
        }
        virtual bool read(const string& s, int n, int binary)
        {
            istringstream iss(s);
            return read(iss, n, binary);
        }

        virtual bool read(const string& s, int binary)
        {
            int n=0;
            //compute size itself
            if (binary == 0)
            {

                string::size_type begin = 0;
                string::size_type end = s.find(' ', begin);
                n=1;

                while (end != string::npos)
                {
                    n++;
                    begin = end + 1;
                    end = s.find(' ', begin);
                }
            }
            else
            {
                n = sizeof(s.c_str())/sizeof(T);
            }
            istringstream iss(s);

            return read(iss, n, binary);
        }

        virtual bool read(istream& in, int n, int binary)
        {
            resize(n);
            if (binary)
            {
                in.read((char*)data, n *sizeof(T));
                if (in.eof() || in.bad())
                {
                    resize(0);
                    return false;
                }
                if (binary == 2) // swap bytes
                {
                    for (int i=0; i<n; ++i)
                    {
                        data[i] = swapT(data[i], nestedDataSize);
                    }
                }
            }
            else
            {
                int i = 0;
                string line;
                while(i < dataSize && !in.eof() && !in.bad())
                {
                    std::getline(in, line);
                    istringstream ln(line);
                    while (i < n && ln >> data[i])
                        ++i;
                }
                if (i < n)
                {
                    resize(0);
                    return false;
                }
            }
            return true;
        }

        virtual bool write(ofstream& out, int n, int groups, int binary)
        {
            if (n > dataSize && !data) return false;
            if (binary)
            {
                out.write((char*)data, n * sizeof(T));
            }
            else
            {
                if (groups <= 0 || groups > n) groups = n;
                for (int i = 0; i < n; ++i)
                {
                    if ((i % groups) > 0)
                        out << ' ';
                    out << data[i];
                    if ((i % groups) == groups-1)
                        out << '\n';
                }
            }
            if (out.bad())
                return false;
            return true;
        }

        virtual BaseData* createSofaData()
        {
            Data<helper::vector<T> >* sdata = new Data<helper::vector<T> >(name.c_str(), true, false);
            sdata->setName(name);
            helper::vector<T>& sofaData = *sdata->beginEdit();

            for (int i=0 ; i<dataSize ; i++)
                sofaData.push_back(data[i]);
            sdata->endEdit();

            return sdata;
        }
    };

    BaseVTKDataIO* newVTKDataIO(const string& typestr)
    {
        if  (!strcasecmp(typestr.c_str(), "char") || !strcasecmp(typestr.c_str(), "Int8"))
            return new VTKDataIO<char>;
        else if (!strcasecmp(typestr.c_str(), "unsigned_char") || !strcasecmp(typestr.c_str(), "UInt8"))
            return new VTKDataIO<unsigned char>;
        else if (!strcasecmp(typestr.c_str(), "short") || !strcasecmp(typestr.c_str(), "Int16"))
            return new VTKDataIO<short>;
        else if (!strcasecmp(typestr.c_str(), "unsigned_short") || !strcasecmp(typestr.c_str(), "UInt16"))
            return new VTKDataIO<unsigned short>;
        else if (!strcasecmp(typestr.c_str(), "int") || !strcasecmp(typestr.c_str(), "Int32"))
            return new VTKDataIO<int>;
        else if (!strcasecmp(typestr.c_str(), "unsigned_int") || !strcasecmp(typestr.c_str(), "UInt32"))
            return new VTKDataIO<unsigned int>;
        //else if (!strcasecmp(typestr.c_str(), "long") || !strcasecmp(typestr.c_str(), "Int64"))
        //	return new VTKDataIO<long long>;
        //else if (!strcasecmp(typestr.c_str(), "unsigned_long") || !strcasecmp(typestr.c_str(), "UInt64"))
        // 	return new VTKDataIO<unsigned long long>;
        else if (!strcasecmp(typestr.c_str(), "float") || !strcasecmp(typestr.c_str(), "Float32"))
            return new VTKDataIO<float>;
        else if (!strcasecmp(typestr.c_str(), "double") || !strcasecmp(typestr.c_str(), "Float64"))
            return new VTKDataIO<double>;
        else return NULL;
    }

    BaseVTKDataIO* newVTKDataIO(const string& typestr, int num)
    {
        BaseVTKDataIO* result = NULL;

        if (num == 1)
            result = newVTKDataIO(typestr);
        else
        {
            if (!strcasecmp(typestr.c_str(), "char") || !strcasecmp(typestr.c_str(), "Int8") ||
                !strcasecmp(typestr.c_str(), "short") || !strcasecmp(typestr.c_str(), "Int32") ||
                !strcasecmp(typestr.c_str(), "int") || !strcasecmp(typestr.c_str(), "Int32"))
            {
                switch (num)
                {
                case 2:
                    result = new VTKDataIO<Vec<2, int> >;
                    break;
                case 3:
                    result = new VTKDataIO<Vec<3, int> >;
                    break;
                default:
                    return NULL;
                }
            }

            if (!strcasecmp(typestr.c_str(), "unsigned char") || !strcasecmp(typestr.c_str(), "UInt8") ||
                !strcasecmp(typestr.c_str(), "unsigned short") || !strcasecmp(typestr.c_str(), "UInt32") ||
                !strcasecmp(typestr.c_str(), "unsigned int") || !strcasecmp(typestr.c_str(), "UInt32"))
            {
                switch (num)
                {
                case 2:
                    result = new VTKDataIO<Vec<2, unsigned int> >;
                    break;
                case 3:
                    result = new VTKDataIO<Vec<3, unsigned int> >;
                    break;
                default:
                    return NULL;
                }
            }
            if (!strcasecmp(typestr.c_str(), "float") || !strcasecmp(typestr.c_str(), "Float32"))
            {
                switch (num)
                {
                case 2:
                    result = new VTKDataIO<Vec<2, float> >;
                    break;
                case 3:
                    result = new VTKDataIO<Vec<3, float> >;
                    break;
                default:
                    return NULL;
                }
            }
            if (!strcasecmp(typestr.c_str(), "double") || !strcasecmp(typestr.c_str(), "Float64"))
            {
                switch (num)
                {
                case 2:
                    result = new VTKDataIO<Vec<2, double> >;
                    break;
                case 3:
                    result = new VTKDataIO<Vec<3, double> >;
                    break;
                default:
                    return NULL;
                }
            }
        }
        result->nestedDataSize = num;
        return result;
    }

public:
    BaseVTKDataIO* inputPoints;
    BaseVTKDataIO* inputPolygons;
    BaseVTKDataIO* inputCells;
    BaseVTKDataIO* inputCellOffsets;
    BaseVTKDataIO* inputCellTypes;
    helper::vector<BaseVTKDataIO*> inputPointDataVector;
    helper::vector<BaseVTKDataIO*> inputCellDataVector;
    bool isLittleEndian;

    int numberOfPoints, numberOfCells, numberOfLines;


    BaseVTKReader():inputPoints (NULL), inputPolygons(NULL), inputCells(NULL), inputCellOffsets(NULL), inputCellTypes(NULL),
        numberOfPoints(0),numberOfCells(0)
    {}

    bool readVTK(const char* filename)
    {
        bool state = readFile(filename);
        return state;
    }

    virtual bool readFile(const char* filename) = 0;
};

class LegacyVTKReader : public BaseVTKReader
{
public:
    bool readFile(const char* filename);
};

class XMLVTKReader : public BaseVTKReader
{
public:
    bool readFile(const char* filename);
protected:
    bool loadUnstructuredGrid(TiXmlHandle datasetFormatHandle);
    bool loadPolydata(TiXmlHandle datasetFormatHandle);
    bool loadRectilinearGrid(TiXmlHandle datasetFormatHandle);
    bool loadStructuredGrid(TiXmlHandle datasetFormatHandle);
    bool loadStructuredPoints(TiXmlHandle datasetFormatHandle);
    bool loadImageData(TiXmlHandle datasetFormatHandle);
    BaseVTKDataIO* loadDataArray(TiXmlElement* dataArrayElement, int size, string type);
    BaseVTKDataIO* loadDataArray(TiXmlElement* dataArrayElement, int size);
    BaseVTKDataIO* loadDataArray(TiXmlElement* dataArrayElement);
};

////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// MeshVTKLoader IMPLEMENTATION //////////////////////////////////
MeshVTKLoader::MeshVTKLoader() : MeshLoader()
    , reader(NULL)
{
}

MeshVTKLoader::VTKFileType MeshVTKLoader::detectFileType(const char* filename)
{
    std::ifstream inVTKFile(filename, std::ifstream::in | std::ifstream::binary);

    if( !inVTKFile.is_open() )
        return MeshVTKLoader::NONE;

    string line;
    std::getline(inVTKFile, line);

    if (line.find("<?xml") != string::npos)
    {
        std::getline(inVTKFile, line);

        if (line.find("<VTKFile") != string::npos)
            return MeshVTKLoader::XML;
        else
            return MeshVTKLoader::NONE;
    }
    else if (line.find("<VTKFile") != string::npos) //... not xml-compliant
        return MeshVTKLoader::XML;
    else if (line.find("# vtk DataFile") != string::npos)
        return MeshVTKLoader::LEGACY;
    else //default behavior if the first line is not correct ?
        return MeshVTKLoader::NONE;
}

bool MeshVTKLoader::load()
{
    sout << "Loading VTK file: " << m_filename << sendl;

    bool fileRead = false;

    // -- Loading file
    const char* filename = m_filename.getFullPath().c_str();

    // Detect file type (legacy or vtk)
    MeshVTKLoader::VTKFileType type = detectFileType(filename);
    switch (type)
    {
    case XML:
        reader = new XMLVTKReader();
        break;
    case LEGACY:
        reader = new LegacyVTKReader();
        break;
    case NONE:
    default:
        serr << "Header not recognized" << sendl;
        reader = NULL;
        break;
    }

    if (!reader)
        return false;

    // -- Reading file
    if(!canLoad())
        return false;

    fileRead = reader->readVTK (filename);
    this->setInputsMesh();
    this->setInputsData();

    delete reader;

    return fileRead;
}

bool MeshVTKLoader::setInputsMesh()
{
    vector<Vector3>& my_positions = *(d_positions.beginEdit());
    if (reader->inputPoints)
    {
        BaseVTKReader::VTKDataIO<double>* vtkpd =  dynamic_cast<BaseVTKReader::VTKDataIO<double>* > (reader->inputPoints);
        BaseVTKReader::VTKDataIO<float>* vtkpf =  dynamic_cast<BaseVTKReader::VTKDataIO<float>* > (reader->inputPoints);
        if (vtkpd)
        {
            const double* inPoints = (vtkpd->data);
            if (inPoints)
                for (int i=0; i < vtkpd->dataSize; i+=3)
                    my_positions.push_back(Vector3 ((double)inPoints[i+0], (double)inPoints[i+1], (double)inPoints[i+2]));
            else return false;
        }
        else if (vtkpf)
        {
            const float* inPoints = (vtkpf->data);
            if (inPoints)
                for (int i=0; i < vtkpf->dataSize; i+=3)
                    my_positions.push_back(Vector3 ((float)inPoints[i+0], (float)inPoints[i+1], (float)inPoints[i+2]));
            else return false;
        }
        else
        {
            serr << "Type of coordinate (X,Y,Z) not supported" << sendl;
            return false;
        }
    }
    else
        return false;

    d_positions.endEdit();

    helper::vector<Edge >& my_edges = *(d_edges.beginEdit());
    helper::vector<Triangle >& my_triangles = *(d_triangles.beginEdit());
    helper::vector<Quad >& my_quads = *(d_quads.beginEdit());
    helper::vector<Tetrahedron >& my_tetrahedra = *(d_tetrahedra.beginEdit());
    helper::vector<Hexahedron >& my_hexahedra = *(d_hexahedra.beginEdit());

    if (reader->inputPolygons)
    {
        const int* inFP = (const int*) reader->inputPolygons->getData();
        int poly = 0;
        for (int i=0; i < reader->inputPolygons->dataSize;)
        {
            int nv = inFP[i]; ++i;
            bool valid = true;
            if (reader->inputPoints)
            {
                for (int j=0; j<nv; ++j)
                    if ((unsigned)inFP[i+j] >= (unsigned)(reader->inputPoints->dataSize/3))
                    {
                        serr << "ERROR: invalid point " << inFP[i+j] << " in polygon " << poly << sendl;
                        valid = false;
                    }
            }
            if (valid)
            {
                if (nv == 4)
                {
                    addQuad(&my_quads, inFP[i+0],inFP[i+1],inFP[i+2],inFP[i+3]);
                }
                else if (nv >= 3)
                {
                    int f[3];
                    f[0] = inFP[i+0];
                    f[1] = inFP[i+1];
                    for (int j=2; j<nv; j++)
                    {
                        f[2] = inFP[i+j];
                        addTriangle(&my_triangles, f[0], f[1], f[2]);
                        f[1] = f[2];
                    }
                }
                i += nv;
            }
            ++poly;
        }
    }
    else if (reader->inputCells && reader->inputCellTypes)
    {
        const int* inFP = (const int*) reader->inputCells->getData();
        //offsets are not used if we have parsed with the legacy method
        const int* offsets = (reader->inputCellOffsets == NULL) ? NULL : (const int*) reader->inputCellOffsets->getData();

        const int* dataT = (int*)(reader->inputCellTypes->getData());

        helper::vector<int> numSubPolyLines;

        int nbf = reader->numberOfCells;
        int i = 0;
        for (int c = 0; c < nbf; ++c)
        {
            int t = dataT[c];// - 48; //ASCII
            int nv;
            if (offsets)
            {
                i = (c == 0) ? 0 : offsets[c-1];
                nv = inFP[i];
            }
            else
            {
                nv = inFP[i]; ++i;
            }

            switch (t)
            {
            case 0: // EMPTY_CELL
                break;
            case 1: // VERTEX
                break;
            case 2: // POLY_VERTEX
                break;
            case 3: // LINE
                addEdge(&my_edges, inFP[i+0], inFP[i+1]);
                break;
            case 4: // POLY_LINE
                numSubPolyLines.push_back(nv);
                for (int v = 0; v < nv-1; ++v) {
                    addEdge(&my_edges, inFP[i+v+0], inFP[i+v+1]);
                    //std::cout << " c = " << c << " i = " << i <<  " v = " << v << "  edge: " << inFP[i+v+0] << " " << inFP[i+v+1] << std::endl;
                }
                break;
            case 5: // TRIANGLE
                addTriangle(&my_triangles,inFP[i+0], inFP[i+1], inFP[i+2]);
                break;
            case 6: // TRIANGLE_STRIP
                for (int j=0; j<nv-2; j++)
                    if (j&1)
                        addTriangle(&my_triangles, inFP[i+j+0],inFP[i+j+1],inFP[i+j+2]);
                    else
                        addTriangle(&my_triangles, inFP[i+j+0],inFP[i+j+2],inFP[i+j+1]);
                break;
            case 7: // POLYGON
                for (int j=2; j<nv; j++)
                    addTriangle(&my_triangles, inFP[i+0],inFP[i+j-1],inFP[i+j]);
                break;
            case 8: // PIXEL
                addQuad(&my_quads, inFP[i+0], inFP[i+1], inFP[i+3], inFP[i+2]);
                break;
            case 9: // QUAD
                addQuad(&my_quads, inFP[i+0], inFP[i+1], inFP[i+2], inFP[i+3]);
                break;
            case 10: // TETRA
                addTetrahedron(&my_tetrahedra, inFP[i+0], inFP[i+1], inFP[i+2], inFP[i+3]);
                break;
            case 11: // VOXEL
                addHexahedron(&my_hexahedra, inFP[i+0], inFP[i+1], inFP[i+3], inFP[i+2],
                        inFP[i+4], inFP[i+5], inFP[i+7], inFP[i+6]);
                break;
            case 12: // HEXAHEDRON
                addHexahedron(&my_hexahedra, inFP[i+0], inFP[i+1], inFP[i+2], inFP[i+3],
                        inFP[i+4], inFP[i+5], inFP[i+6], inFP[i+7]);
                break;
            // more types are defined in vtkCellType.h in libvtk
            default:
                serr << "ERROR: unsupported cell type " << t << sendl;
            }

            if (!offsets)
                i += nv;
        }

        if (numSubPolyLines.size() > 0) {
            size_t sz = reader->inputCellDataVector.size();
            reader->inputCellDataVector.resize(sz+1);
            reader->inputCellDataVector[sz] = reader->newVTKDataIO("int");

            BaseVTKReader::VTKDataIO<int>* cellData = dynamic_cast<BaseVTKReader::VTKDataIO<int>* > (reader->inputCellDataVector[sz]);

            if (cellData == NULL) return false;

            cellData->resize((int)numSubPolyLines.size());

            for (size_t ii = 0;  ii < numSubPolyLines.size(); ii++)
                cellData->data[ii] = numSubPolyLines[ii];

            cellData->name = "PolyLineSubEdges";
        }

    }
    if (reader->inputPoints) delete reader->inputPoints;
    if (reader->inputPolygons) delete reader->inputPolygons;
    if (reader->inputCells) delete reader->inputCells;
    if (reader->inputCellTypes) delete reader->inputCellTypes;

    d_edges.endEdit();
    d_triangles.endEdit();
    d_quads.endEdit();
    d_tetrahedra.endEdit();
    d_hexahedra.endEdit();

    return true;
}

bool MeshVTKLoader::setInputsData()
{
    ///Point Data
    for (size_t i=0 ; i<reader->inputPointDataVector.size() ; i++)
    {
        const char* dataname = reader->inputPointDataVector[i]->name.c_str();

        BaseData* basedata = reader->inputPointDataVector[i]->createSofaData();
        this->addData(basedata, dataname);
    }

    ///Cell Data
    for (size_t i=0 ; i<reader->inputCellDataVector.size() ; i++)
    {
        const char* dataname = reader->inputCellDataVector[i]->name.c_str();
        BaseData* basedata = reader->inputCellDataVector[i]->createSofaData();
        this->addData(basedata, dataname);
    }

    return true;
}


//Legacy VTK Loader
bool LegacyVTKReader::readFile(const char* filename)
{
    std::ifstream inVTKFile(filename, std::ifstream::in | std::ifstream::binary);
    if( !inVTKFile.is_open() )
    {
        return false;
    }
    string line;

    // Part 1
    std::getline(inVTKFile, line);
    if (string(line,0,23) != "# vtk DataFile Version ")
    {
        serr << "Error: Unrecognized header in file '" << filename << "'." << sendl;
        inVTKFile.close();
        return false;
    }
    string version(line,23);

    // Part 2
    string header;
    std::getline(inVTKFile, header);

    // Part 3
    std::getline(inVTKFile, line);

    int binary;
    if (line == "BINARY" || line == "BINARY\r" )
    {
        binary = 1;
    }
    else if ( line == "ASCII" || line == "ASCII\r")
    {
        binary = 0;
    }
    else
    {
        serr << "Error: Unrecognized format in file '" << filename << "'." << sendl;
        inVTKFile.close();
        return false;
    }

    if (binary && strlen(filename)>9 && !strcmp(filename+strlen(filename)-9,".vtk_swap"))
        binary = 2; // bytes will be swapped


    // Part 4
    do
        std::getline(inVTKFile, line);
    while (line == "");
    if (line != "DATASET POLYDATA" && line != "DATASET UNSTRUCTURED_GRID"
        && line != "DATASET POLYDATA\r" && line != "DATASET UNSTRUCTURED_GRID\r" )
    {
        serr << "Error: Unsupported data type in file '" << filename << "'." << sendl;
        inVTKFile.close();
        return false;
    }

    sout << (binary == 0 ? "Text" : (binary == 1) ? "Binary" : "Swapped Binary") << " VTK File (version " << version << "): " << header << sendl;
    VTKDataIO<int>* inputPolygonsInt = NULL;
    VTKDataIO<int>* inputCellsInt = NULL;
    VTKDataIO<int>* inputCellTypesInt = NULL;
    inputCellOffsets = NULL;

    while(!inVTKFile.eof())
    {
        std::getline(inVTKFile, line);
        if (line.empty()) continue;
        istringstream ln(line);
        string kw;
        ln >> kw;
        if (kw == "POINTS")
        {
            int n;
            string typestr;
            ln >> n >> typestr;
            sout << "Found " << n << " " << typestr << " points" << sendl;
            inputPoints = newVTKDataIO(typestr);
            if (inputPoints == NULL) return false;
            if (!inputPoints->read(inVTKFile, 3*n, binary)) return false;
            //nbp = n;
        }
        else if (kw == "POLYGONS")
        {
            int n, ni;
            ln >> n >> ni;
            sout << "Found " << n << " polygons ( " << (ni - 3*n) << " triangles )" << sendl;
            inputPolygons = new VTKDataIO<int>;
            inputPolygonsInt = dynamic_cast<VTKDataIO<int>* > (inputPolygons);
            if (!inputPolygons->read(inVTKFile, ni, binary)) return false;
        }
        else if (kw == "CELLS")
        {
            int n, ni;
            ln >> n >> ni;
            sout << "Found " << n << " cells" << sendl;
            inputCells = new VTKDataIO<int>;
            inputCellsInt = dynamic_cast<VTKDataIO<int>* > (inputCells);
            if (!inputCells->read(inVTKFile, ni, binary)) return false;
            numberOfCells = n;
        }
         else if (kw == "LINES")
        {
            int n, ni;
            ln >> n >> ni;
            sout << "Found " << n << " lines" << sendl;
            inputCells = new VTKDataIO<int>;
            inputCellsInt = dynamic_cast<VTKDataIO<int>* > (inputCellsInt);
            if (!inputCells->read(inVTKFile, ni, binary)) return false;
            numberOfCells = n;

            inputCellTypes = new VTKDataIO<int>;
            inputCellTypesInt = dynamic_cast<VTKDataIO<int>* > (inputCellTypes);
            inputCellTypesInt->resize(n);
            for (int i = 0; i < n; i++)
                inputCellTypesInt->data[i] = 4;
        }
        else if (kw == "CELL_TYPES")
        {
            int n;
            ln >> n;
            inputCellTypes = new VTKDataIO<int>;
            inputCellTypesInt = dynamic_cast<VTKDataIO<int>* > (inputCellTypes);
            if (!inputCellTypes->read(inVTKFile, n, binary)) return false;
        }
        else if (kw == "CELL_DATA") {
            int n;
            ln >> n;

            std::getline(inVTKFile, line);
            if (line.empty()) continue;
            /// line defines the type and name such as SCALAR dataset
            istringstream lnData(line);
            string dataStructure, dataName, dataType;
            lnData >> dataStructure;

            sout << "Data structure: " << dataStructure << sendl;

            if (dataStructure == "SCALARS") {
                size_t sz = inputCellDataVector.size();

                inputCellDataVector.resize(sz+1);
                lnData >> dataName;
                lnData >> dataType;

                inputCellDataVector[sz] = newVTKDataIO(dataType);
                if (inputCellDataVector[sz] == NULL) return false;
                /// one more getline to read LOOKUP_TABLE line, not used here
                std::getline(inVTKFile, line);

                if (!inputCellDataVector[sz]->read(inVTKFile,n, binary)) return false;
                inputCellDataVector[sz]->name = dataName;
                sout << "Read cell data: " << inputCellDataVector[sz]->dataSize << sendl;
            }
            else if (dataStructure == "FIELD") {
                std::getline(inVTKFile,line);
                if (line.empty()) continue;
                /// line defines the type and name such as SCALAR dataset
                istringstream lnData(line);
                string dataStructure;
                lnData >> dataStructure;

//                if (dataStructure == "Topology") {
                    int perCell, cells;
                    lnData >> perCell >> cells;
                    sout << "Reading topology for lines: "<< perCell << " " << cells << sendl;

                    size_t sz = inputCellDataVector.size();

                    inputCellDataVector.resize(sz+1);
                    inputCellDataVector[sz] = newVTKDataIO("int");

                    if (!inputCellDataVector[sz]->read(inVTKFile,perCell*cells, binary))
                        return false;

                    inputCellDataVector[sz]->name = "Topology";
//                }
            }
            else  /// TODO
                std::cerr << "WARNING: reading vector data not implemented" << std::endl;
        }
        else if (!kw.empty())
            std::cerr << "WARNING: Unknown keyword " << kw << std::endl;

        sout << "LNG: " << inputCellDataVector.size() << sendl;

        if (inputPoints && inputPolygons) break; // already found the mesh description, skip the rest
        if (inputPoints && inputCells && inputCellTypes && inputCellDataVector.size() > 0) break; // already found the mesh description, skip the rest
    }

    if (binary)
    {
        // detect swapped data
        bool swapped = false;
        if (inputPolygons)
        {
            if ((unsigned)inputPolygonsInt->data[0] > (unsigned)inputPolygonsInt->swapT(inputPolygonsInt->data[0], 1))
                swapped = true;
        }
        else if (inputCells && inputCellTypes)
        {
            if ((unsigned)inputCellTypesInt->data[0] > (unsigned)inputCellTypesInt->swapT(inputCellTypesInt->data[0],1 ))
                swapped = true;
        }
        if (swapped)
        {
            sout << "Binary data is byte-swapped." << sendl;
            if (inputPoints) inputPoints->swap();
            if (inputPolygons) inputPolygons->swap();
            if (inputCells) inputCells->swap();
            if (inputCellTypes) inputCellTypes->swap();
        }
    }

    return true;
}

bool XMLVTKReader::readFile(const char* filename)
{
    TiXmlDocument vtkDoc(filename);
    //quick check
    checkErrorMsg(vtkDoc.LoadFile(), "Unknown error while loading VTK Xml doc");

    TiXmlHandle hVTKDoc(&vtkDoc);
    TiXmlElement* pElem;
    TiXmlHandle hVTKDocRoot(0);

    //block VTKFile
    pElem = hVTKDoc.FirstChildElement().ToElement();
    checkErrorMsg(pElem, "VTKFile Node not found");

    hVTKDocRoot = TiXmlHandle(pElem);

    //Endianness
    const char* endiannessStrTemp = pElem->Attribute("byte_order");
    isLittleEndian = (string(endiannessStrTemp).compare("LittleEndian") == 0) ;

    //read VTK data format type
    const char* datasetFormatStrTemp = pElem->Attribute("type");
    checkErrorMsg(datasetFormatStrTemp, "Dataset format not defined")
    string datasetFormatStr = string(datasetFormatStrTemp);
    VTKDatasetFormat datasetFormat;

    if (datasetFormatStr.compare("UnstructuredGrid") == 0)
        datasetFormat = UNSTRUCTURED_GRID;
    else if (datasetFormatStr.compare("PolyData") == 0)
        datasetFormat = POLYDATA;
    else if (datasetFormatStr.compare("RectilinearGrid") == 0)
        datasetFormat = RECTILINEAR_GRID;
    else if (datasetFormatStr.compare("StructuredGrid") == 0)
        datasetFormat = STRUCTURED_GRID;
    else if (datasetFormatStr.compare("StructuredPoints") == 0)
        datasetFormat = STRUCTURED_POINTS;
    else if (datasetFormatStr.compare("ImageData") == 0)
        datasetFormat = IMAGE_DATA;
    else checkErrorMsg(false, "Dataset format " << datasetFormatStr << " not recognized");

    TiXmlHandle datasetFormatHandle = TiXmlHandle(hVTKDocRoot.FirstChild( datasetFormatStr.c_str() ).ToElement());

    bool stateLoading = false;
    switch (datasetFormat)
    {
    case UNSTRUCTURED_GRID :
        stateLoading = loadUnstructuredGrid(datasetFormatHandle);
        break;
    case POLYDATA :
        stateLoading = loadPolydata(datasetFormatHandle);
        break;
    case RECTILINEAR_GRID :
        stateLoading = loadRectilinearGrid(datasetFormatHandle);
        break;
    case STRUCTURED_GRID :
        stateLoading = loadStructuredGrid(datasetFormatHandle);
        break;
    case STRUCTURED_POINTS :
        stateLoading = loadStructuredPoints(datasetFormatHandle);
        break;
    case IMAGE_DATA :
        stateLoading = loadImageData(datasetFormatHandle);
        break;
    default:
        checkErrorMsg(false, "Dataset format not implemented");
        break;
    }
    checkErrorMsg(stateLoading, "Error while parsing XML");

    return true;
}

BaseVTKReader::BaseVTKDataIO* XMLVTKReader::loadDataArray(TiXmlElement* dataArrayElement)
{
    return loadDataArray(dataArrayElement,0);
}

BaseVTKReader::BaseVTKDataIO* XMLVTKReader::loadDataArray(TiXmlElement* dataArrayElement, int size)
{
    return loadDataArray(dataArrayElement, size, "");
}

BaseVTKReader::BaseVTKDataIO* XMLVTKReader::loadDataArray(TiXmlElement* dataArrayElement, int size, string type)
{
    //Type
    const char* typeStrTemp;
    if (type.empty())
    {
        typeStrTemp = dataArrayElement->Attribute("type");
        checkErrorPtr(typeStrTemp);
    }
    else
        typeStrTemp = type.c_str();

    //Format
    const char* formatStrTemp = dataArrayElement->Attribute("format");

    if (formatStrTemp==NULL) formatStrTemp = dataArrayElement->Attribute("Format");

    checkErrorPtr(formatStrTemp);

    int binary = 0;
    if (string(formatStrTemp).compare("ascii") == 0)
        binary = 0;
    else if (isLittleEndian)
        binary = 1;
    else
        binary = 2;

    //NumberOfComponents
    int numberOfComponents;
    if (dataArrayElement->QueryIntAttribute("NumberOfComponents", &numberOfComponents) != TIXML_SUCCESS)
        numberOfComponents = 1;

    //Values
    const char* listValuesStrTemp = dataArrayElement->GetText();

    bool state = false;

    if (!listValuesStrTemp) return NULL;
    if (string(listValuesStrTemp).size() < 1) return NULL;

    BaseVTKDataIO* d = BaseVTKReader::newVTKDataIO(string(typeStrTemp));

    if (!d) return NULL;

    if (size > 0)
        state = (d->read(string(listValuesStrTemp), numberOfComponents*size, binary));
    else
        state = (d->read(string(listValuesStrTemp), binary));
    checkErrorPtr(state);

    return d;
}

bool XMLVTKReader::loadUnstructuredGrid(TiXmlHandle datasetFormatHandle)
{
    TiXmlElement* pieceElem = datasetFormatHandle.FirstChild( "Piece" ).ToElement();

    checkError(pieceElem);
    for( ; pieceElem; pieceElem=pieceElem->NextSiblingElement())
    {
        pieceElem->QueryIntAttribute("NumberOfPoints", &numberOfPoints);
        pieceElem->QueryIntAttribute("NumberOfCells", &numberOfCells);

        TiXmlNode* dataArrayNode;
        TiXmlElement* dataArrayElement;
        TiXmlNode* node = pieceElem->FirstChild();

        for ( ; node ; node = node->NextSibling())
        {
            string currentNodeName = string(node->Value());

            if (currentNodeName.compare("Points") == 0)
            {
                /* Points */
                dataArrayNode = node->FirstChild("DataArray");
                checkError(dataArrayNode);
                dataArrayElement = dataArrayNode->ToElement();
                checkError(dataArrayElement);
                //Force the points coordinates to be stocked as double
                inputPoints = loadDataArray(dataArrayElement, numberOfPoints, "Float64");
                checkError(inputPoints);
            }

            if (currentNodeName.compare("Cells") == 0)
            {
                /* Cells */
                dataArrayNode = node->FirstChild("DataArray");
                for ( ; dataArrayNode; dataArrayNode = dataArrayNode->NextSibling( "DataArray"))
                {
                    dataArrayElement = dataArrayNode->ToElement();
                    checkError(dataArrayElement);
                    string currentDataArrayName = string(dataArrayElement->Attribute("Name"));
                    ///DA - connectivity
                    if (currentDataArrayName.compare("connectivity") == 0)
                    {
                        //number of elements in values is not known ; have to guess it
                        inputCells = loadDataArray(dataArrayElement);
                        checkError(inputCells);
                    }
                    ///DA - offsets
                    if (currentDataArrayName.compare("offsets") == 0)
                    {
                        inputCellOffsets = loadDataArray(dataArrayElement, numberOfCells-1);
                        checkError(inputCellOffsets);
                    }
                    ///DA - types
                    if (currentDataArrayName.compare("types") == 0)
                    {
                        inputCellTypes = loadDataArray(dataArrayElement, numberOfCells, "Int32");
                        checkError(inputCellTypes);
                    }
                }
            }

            if (currentNodeName.compare("PointData") == 0)
            {
                dataArrayNode = node->FirstChild("DataArray");
                for ( ; dataArrayNode; dataArrayNode = dataArrayNode->NextSibling( "DataArray"))
                {
                    dataArrayElement = dataArrayNode->ToElement();
                    checkError(dataArrayElement);

                    string currentDataArrayName = string(dataArrayElement->Attribute("Name"));

                    BaseVTKDataIO* pointdata = loadDataArray(dataArrayElement, numberOfPoints);
                    checkError(pointdata);
                    pointdata->name = currentDataArrayName;
                    inputPointDataVector.push_back(pointdata);
                }
            }
            if (currentNodeName.compare("CellData") == 0)
            {
                dataArrayNode = node->FirstChild("DataArray");
                for ( ; dataArrayNode; dataArrayNode = dataArrayNode->NextSibling( "DataArray"))
                {
                    dataArrayElement = dataArrayNode->ToElement();
                    checkError(dataArrayElement);
                    string currentDataArrayName = string(dataArrayElement->Attribute("Name"));
                    BaseVTKDataIO* celldata = loadDataArray(dataArrayElement, numberOfCells);
                    checkError(celldata);
                    celldata->name = currentDataArrayName;
                    inputCellDataVector.push_back(celldata);
                }
            }
        }
    }

    return true;
}

bool XMLVTKReader::loadPolydata(TiXmlHandle datasetFormatHandle)
{
    SOFA_UNUSED(datasetFormatHandle);
    serr << "Polydata dataset not implemented yet" << sendl;
    return false;
}

bool XMLVTKReader::loadRectilinearGrid(TiXmlHandle datasetFormatHandle)
{
    SOFA_UNUSED(datasetFormatHandle);
    serr << "RectilinearGrid dataset not implemented yet" << sendl;
    return false;
}

bool XMLVTKReader::loadStructuredGrid(TiXmlHandle datasetFormatHandle)
{
    SOFA_UNUSED(datasetFormatHandle);
    serr << "StructuredGrid dataset not implemented yet" << sendl;
    return false;
}

bool XMLVTKReader::loadStructuredPoints(TiXmlHandle datasetFormatHandle)
{
    SOFA_UNUSED(datasetFormatHandle);
    serr << "StructuredPoints dataset not implemented yet" << sendl;
    return false;
}

bool XMLVTKReader::loadImageData(TiXmlHandle datasetFormatHandle)
{
    SOFA_UNUSED(datasetFormatHandle);
    serr << "ImageData dataset not implemented yet" << sendl;
    return false;
}


//////////////////////////////////////////// REGISTERING TO FACTORY /////////////////////////////////////////
/// Registering the component
/// see: https://www.sofa-framework.org/community/doc/programming-with-sofa/components-api/the-objectfactory/
/// 1-SOFA_DECL_CLASS(componentName) : Set the class name of the component
/// 2-RegisterObject("description") + .add<> : Register the component
SOFA_DECL_CLASS(MeshVTKLoader)

int MeshVTKLoaderClass = core::RegisterObject("Mesh loader for the VTK file format.")
        .add< MeshVTKLoader >()
        ;


} // namespace loader

} // namespace component

} // namespace sofa

