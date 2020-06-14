workspace "MagicaFluid"
	architecture "x64"
	startproject "MagicaFluid"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}
	
	location "build"

outputdir = "%{cfg.buildcfg}"

project "MagicaFluid"
	location "build"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir (outputdir)
	objdir ("bin-int/" .. outputdir)
	
	includedirs
	{	
		"Dependencise/glad/include",
		"Dependencise/imgui-docking",
		"Dependencise/imgui-docking/examples",
		"Dependencise/glm",
		"Dependencise/stb",
		"MagicaFluid/",
		"MagicaFluid/Fluid",
		"MagicaFluid/Magica"
	}

	files
	{
		"Dependencise/imgui-docking/examples/imgui_impl_glfw.cpp",
		"Dependencise/imgui-docking/examples/imgui_impl_opengl3.cpp",
		"Dependencise/imgui-docking/imgui.cpp",
		"Dependencise/imgui-docking/imgui_demo.cpp",
		"Dependencise/imgui-docking/imgui_draw.cpp",
		"Dependencise/imgui-docking/imgui_widgets.cpp",
		"Dependencise/glad/src/**.c",
		"Dependencise/stb/stb_image.h",
		"MagicaFluid/**.h",
		"MagicaFluid/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"
		staticruntime "off"
		
		includedirs
		{	
			"Dependencise/glfw/WIN64/include"
		}
		
		defines
		{
			"IMGUI_IMPL_OPENGL_LOADER_GLAD"
		}
		
		links
		{
			"opengl32.lib",
			"Dependencise/glfw/WIN64/lib-vc2019/glfw3.lib"
		}
		
	filter "system:macosx"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		platforms {"x64"}
 		defaultplatform "x64"
		
		includedirs
		{	
			"Dependencise/glfw/MACOS/include"
		}
		
		defines
		{
			"IMGUI_IMPL_OPENGL_LOADER_GLAD"
		}
		
		links
		{
			"Dependencise/glfw/MACOS/lib-macos/libglfw3.a",
			"Cocoa.framework",
			"IOKit.framework",
			"OpenGL.framework"
		}
		
		xcodebuildsettings
		{
			["ALWAYS_SERCH_USER_PAHT"] = "YES"
		}
		
	filter "configurations:Debug"
			defines "_DEBUG"
			runtime "Debug"
			symbols "on"

	filter "configurations:Release"
			defines "_RELEASE"
			runtime "Release"
			optimize "on"
