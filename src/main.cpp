#include "smallrender.h"
#include<stdexcept>

int main(int argc, char *argv[]){
  if (argc < 2)
    throw std::runtime_error("Usage: program <model_path>");

  std::string mtl;
  if( argc < 3)
    mtl = "./";
  else
    mtl = argv[2];
  SmallRenderer sr(500, 500);
  std::string path = argv[1];
  sr.init(path, mtl);
  sr.run();
}
