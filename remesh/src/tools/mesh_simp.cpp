#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <Eigen/Dense>
#include "remesh/MeshAttrSimplifier.hpp"
#include "remesh/GeometryChecker.hpp"
#include "remesh/HoppeMeasurer.hpp"
#include "utils/term_msg.h"
#include "io/TglMeshReader.hpp"

using namespace std;

typedef EdgeCollapsibleTriMesh<Vector3<REAL>, uint32_t> THEMesh;
typedef GeometryChecker<THEMesh>                        TChecker;
typedef HoppeMeasuerer<THEMesh>                         TMeasure;
 
static string meshFile = "";
static string outFile = "";

static void process_args(int argc, char* argv[])
{
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    po::options_description desc("Allowed options & arguments");
    desc.add_options()
        ("help,h", "display help message")
        ("mesh,s", po::value<string>(&meshFile), ".obj file for the triangle mesh")
        ("out,o", po::value<string>(&outFile), "output obj file name");
    po::variables_map vm;
    store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if ( vm.count("help") )
    {
        cout << "Usage: " << fs::path(argv[0]).filename().string() << " [options] <args>" << endl;
        cout << "Simplify mesh with attributes" << endl;
        cout << endl;
        cout << desc;
        cout << endl;
        exit(0);
    }

    if ( meshFile.empty() || outFile.empty() )
    {
        PRINT_ERROR("No mesh file or output filename is specified\n");
        cout << "Usage: " << fs::path(argv[0]).filename().string() << " [options] <args>" << endl;
        cout << "Simplify mesh based on vibration modes" << endl;
        cout << endl;
        cout << desc;
        cout << endl;
        exit(0);
    }

    printf("=============================================================\n");
    printf("triangle mesh:          %s\n", meshFile.c_str());
    printf("output file:            %s\n", outFile.c_str());
    printf("=============================================================\n");
}

static void write_as_obj(const THEMesh* mesh, const char* filename)
{
    typedef typename THEMesh::TVertex   TVertex;
    typedef typename THEMesh::THalfEdge THalfEdge;
    typedef typename THEMesh::TFace     TFace;
    boost::unordered_map<TVertex*, int> vidMap;

    int nv, nf;
    TVertex* const*   vs = mesh->vertices(nv);
    TFace* const*     fs = mesh->faces(nf);

    ofstream fout(filename);
    if ( fout.fail() )
    {
        SHOULD_NEVER_HAPPEN(-1);
    }

    for(int i = 0;i < nv;++ i)
    {
        fout << "v " << vs[i]->pos_.x << ' '
                     << vs[i]->pos_.y << ' '
                     << vs[i]->pos_.z << endl;
        vidMap[vs[i]] = i+1;
    }
    for(int i = 0;i < nf;++ i)
    {
        THalfEdge* const he = fs[i]->he_;
        fout << "f " << vidMap[he->headVtx_];
        for(THalfEdge* hi = he->next_;hi != he;hi = hi->next_)
        {
            fout << ' ' << vidMap[hi->headVtx_];
        }
        fout << endl;
    }
    fout.close();
}

static void simplify()
{
    TriangleMesh<REAL> mesh;
    PRINT_MSG("read mesh ... ");
    MeshObjReader::read(meshFile.c_str(), mesh);
    printf(" [OK]\n");

    TChecker    checker;
    TMeasure    measure;
    MeshAttrSimplifier< TChecker, TMeasure > simp(&mesh, &checker, &measure);
    
    PRINT_MSG("Simplify mesh ... \n");
    Vector3<REAL>* dummy = new Vector3<REAL>[mesh.num_vertices()];
    memset(dummy, 0, sizeof(Vector3<REAL>)*mesh.num_vertices());
    simp.simplify(dummy);

    write_as_obj(simp.collapsible_mesh(), outFile.c_str());

    int nv, nf;
    simp.collapsible_mesh()->vertices(nv);
    simp.collapsible_mesh()->faces(nf);
    PRINT_MSG("Output mesh: %d vertices, %d triangles\n", nv, nf);
    delete []dummy;
}

int main(int argc, char* argv[])
{
    process_args(argc, argv);

    simplify();
    return 0;
}
