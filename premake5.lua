dofile "premake-utils.lua"

solution "Phi"
	configurations {"Debug", "Release"}

project "Phi"
	kind "ConsoleApp"
	language "C++"
	files {
		"Phi/**.h", 
		"Phi/**.cpp", 
		"Phi/Shaders/*.frag", 
		"Phi/Shaders/*.vert", 
		"Externals/glew/src/*.c", 
		"Externals/stb/*.h",
		"Externals/imgui/*.cpp",
		"Externals/imgui/*.h",
		"Externals/HappyHTTP/happyhttp.*"
	}
	includedirs {"Externals/glfw/include", "Externals/glew/include",  "Externals/glm", "Externals/", "Phi"}
	floatingpoint "Fast"
	flags       { "Symbols" }
	debugdir "Phi"
	pchsource "Phi/PreCompile.cpp"
	location ("build")

	os.mkdir("build")
	os.copydir("Phi/Shaders", "build/Shaders")
	defines { "GLEW_STATIC", "GLM_FORCE_RADIANS" }
  configuration { "Externals/imgui/*.cpp" }
    flags { "NoPCH" }
	configuration { "**.c"}
		flags { "NoPCH" }

	configuration "vs*"
		pchheader "PreCompile.h"
		defines     { "_CRT_SECURE_NO_WARNINGS" }

	configuration "vs2005"
		defines        {"_CRT_SECURE_NO_DEPRECATE" }
	
	configuration "windows"  
		os.copyfile("Externals/glfw/lib/glfw3.dll", "build/glfw3.dll")
		libdirs "Externals/glfw/lib"
		system "Windows"
		links {"opengl32", "glfw3dll"}
	
	configuration { "macosx", "gmake" }
		toolset "clang"
		buildoptions { "-mmacosx-version-min=10.4" }
		linkoptions  { "-mmacosx-version-min=10.4" }

	configuration "macosx"
		pchheader "../Phi/PreCompile.h"
		libdirs "Externals/glfw/lib/osx"
		links       { "glfw3", "OpenGL.framework", "CoreFoundation.framework", "CoreVideo.framework", "Cocoa.framework", "IOKit.framework", "AGL.framework"}
	configuration "linux"
		buildoptions { "-std=c++11" }
		linkoptions  { "-std=c++11" }

		if os.is64bit() then
			libdirs "Externals/glfw/lib/linux/x86_64"
		else
			libdirs "Externals/glfw/lib/linux/x86"
		end
		links       { "glfw3", "X11", "Xxf86vm", "Xrandr", "Xi", "GL", "pthread"}
		
	configuration "Debug"
		defines { "DEBUG" }
		optimize "Debug"

	configuration "Release"
	defines { "NDEBUG" }
	optimize "Full"
