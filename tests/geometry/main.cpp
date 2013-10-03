#include <geometry/TriangleMesh.h>

int main() {

  TriangleMesh mesh;

  mesh.readObj("tests/geometry/hello000.obj");
  return 0;
}
