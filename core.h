#ifndef _CORE_H_
#define _CORE_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#ifndef GLM_ENABLE_EXPERIMENTAL
	#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <map>
#include <ctype.h>
#include <algorithm>
#include <Math.h>
#include <tuple>
#include <omp.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <direct.h>

//Miscs
#define EPSILON 1e-4f
#define POINT_SIZE 1.0f

//simulation rates
#define DEFAULT_SIMRATE 300.0f
#define DEFAULT_RENDRATE 100.0f
#define DEFAULT_LOD_LEVEL 10
#define DEFAULT_LOWEST_VERTEX_NUM 1000

#endif