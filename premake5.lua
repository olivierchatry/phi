--
-- Allows copying directories.
-- It uses the premake4 patterns (**=recursive match, *=file match)
-- NOTE: It won't copy empty directories!
-- Example: we have a file: src/test.h
--	os.copydir("src", "include") simple copy, makes include/test.h
--	os.copydir("src", "include", "*.h") makes include/test.h
--	os.copydir(".", "include", "src/*.h") makes include/src/test.h
--	os.copydir(".", "include", "**.h") makes include/src/test.h
--	os.copydir(".", "include", "**.h", true) will force it to include dir, makes include/test.h
--
-- @param src_dir
--    Source directory, which will be copied to dst_dir.
-- @param dst_dir
--    Destination directory.
-- @param filter
--    Optional, defaults to "**". Only filter matches will be copied. It can contain **(recursive) and *(filename).
-- @param single_dst_dir
--    Optional, defaults to false. Allows putting all files to dst_dir without subdirectories.
--    Only useful with recursive (**) filter.
-- @returns
--    True if successful, otherwise nil.
--
function os.copydir(src_dir, dst_dir, filter, single_dst_dir)
	if not os.isdir(src_dir) then error(src_dir .. " is not an existing directory!") end
	filter = filter or "**"
	src_dir = src_dir .. "/"
	print('copy "' .. src_dir .. filter .. '" to "' .. dst_dir .. '".')
	dst_dir = dst_dir .. "/"
	local dir = path.rebase(".",path.getabsolute("."), src_dir) -- root dir, relative from src_dir
 
	os.chdir( src_dir ) -- change current directory to src_dir
		local matches = os.matchfiles(filter)
	os.chdir( dir ) -- change current directory back to root
 
	local counter = 0
	for k, v in ipairs(matches) do
		local target = iif(single_dst_dir, path.getname(v), v)
		--make sure, that directory exists or os.copyfile() fails
		os.mkdir( path.getdirectory(dst_dir .. target))
		if os.copyfile( src_dir .. v, dst_dir .. target) then
			counter = counter + 1
		end
	end
 
	if counter == #matches then
		print( counter .. " files copied.")
		return true
	else
		print( "Error: " .. counter .. "/" .. #matches .. " files copied.")
		return nil
	end
end

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
		"Externals/stb_image/*.c", 
		"Externals/stb_image/*.h"
	}
	includedirs {"Externals/glfw/include", "Externals/glew/include",  "Externals/glm", "Externals/stb_image/", "Phi"}
	location ("build")
	floatingpoint "Fast"
	flags       { "Symbols" }
  debugdir "Phi"

  os.mkdir("build")
  os.copydir("Phi/Shaders", "build/Shaders")
  defines { "GLEW_STATIC", "GLM_FORCE_RADIANS" }

  configuration { "**.c"}
    flags { "NoPCH" }
    
	configuration "vs*"
		pchheader "PreCompile.h"
		pchsource "Phi/PreCompile.cpp"
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
		libdirs "Externals/glfw/lib/osx"
		links       { "glfw3", "OpenGL.framework", "CoreFoundation.framework", "Cocoa.framework", "IOKit.framework", "AGL.framework"}
	
	configuration "Debug"
		defines { "DEBUG" }
		optimize "Debug"
		
	configuration "Release"
		defines { "NDEBUG" }
		optimize "Full"
