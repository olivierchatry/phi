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
#   include <glm/gtx/matrix_major_storage.hpp>
#   include <glm/gtx/matrix_interpolation.hpp>
#	include <glm/gtx/transform.hpp>
#	include <glm/gtc/quaternion.hpp>
#	include <glm/gtx/quaternion.hpp>
#	include <glm/gtc/matrix_transform.hpp>
#	include <glm/gtc/type_ptr.hpp>
#   include <vector>
#   include <fstream>
#   include <string>
#	include <limits>
#	include <algorithm>
#   include <map>
#endif

#include <stb/stb_image.h>