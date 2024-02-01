#ifndef SHADER_HPP
#define SHADER_HPP

#include <stdio.h>
#include <stdlib.h>

#include <cstdio>

#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include<GL/glew.h>



GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

#endif
