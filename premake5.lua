-- premake5.lua
workspace "Chip8"
    configurations { "Debug", "Release" }
    platforms { "x64" }
    
    -- Set the architecture
    filter { "platforms:x64" }
        architecture "x86_64"
    
    -- Reset filters
    filter {}

project "Chip8"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    objdir "obj/%{cfg.buildcfg}"
    
    files {
        "src/**.h",
        "src/**.hpp",
        "src/**.cpp",
        "headers/**.h",     
        "headers/**.hpp"    
    }
    
    -- Add headers directory to include path
    includedirs {
        "headers",         
        "C:/SDL2/include"   
    }
    
    -- SDL2 path configuration
    SDL2_PATH = "C:/SDL2"   
    
    filter "system:windows"
        defines { "WINDOWS" }
        links { "SDL2", "SDL2main", "SDL2_mixer"}
        libdirs { SDL2_PATH .. "/lib/x64" }
        
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
    -- Post-build command to copy SDL2.dll to the executable directory
    filter { "system:windows", "configurations:Debug" }
        postbuildcommands {
            "{COPY} " .. SDL2_PATH .. "/lib/x64/SDL2.dll %{cfg.targetdir}"
        }
        
    filter { "system:windows", "configurations:Release" }
        postbuildcommands {
            "{COPY} " .. SDL2_PATH .. "/lib/x64/SDL2.dll %{cfg.targetdir}"
        }
        
    filter { "system:windows", "configurations:Debug" }
        postbuildcommands {
            "{COPY} " .. SDL2_PATH .. "/lib/x64/SDL2_mixer.dll %{cfg.targetdir}"
        }
        
    filter { "system:windows", "configurations:Release" }
        postbuildcommands {
            "{COPY} " .. SDL2_PATH .. "/lib/x64/SDL2_mixer.dll %{cfg.targetdir}"
        }
        