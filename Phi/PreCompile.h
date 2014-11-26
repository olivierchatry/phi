#include "targetver.h"

#include <stdio.h>

#if defined(_WIN32)
#   include <tchar.h>
#endif


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __cplusplus
#	include <glm/glm.hpp>
#	include <glm/ext.hpp>
#   include <glm/gtc/random.hpp>
#   include <vector>
#   include <fstream>
#   include <string>
#	include <limits>
#	include <algorithm>
#endif

#include <stb_image.h>