---------------------------------------------
------------ GLOBAL VARIABLES ---------------
---------------------------------------------

-- @Note: Basic configs:
GPU_API = {
    GL = "GAME_GL"
}

cpp_version = "C++17"
gpu_api     = GPU_API.GL

-- @Note: Dir locations:
loc_this  = "./" -- @Note: You can change this if you want to move generate.lua.
loc_bin   = loc_this .. ".bin/%{prj.name}/%{cfg.buildcfg}"
loc_int   = loc_this .. ".bin-int/%{prj.name}/%{cfg.buildcfg}"
loc_core  = loc_this .. "core/" 
loc_3rd   = loc_this .. "3rd/"
loc_imgui = loc_3rd  .. "imgui/"
loc_box2d = loc_3rd  .. "box2d/"

-- @Note: Include dirs:

-- imgui:
inc_imgui = {
    loc_imgui .. "src/",
    loc_imgui .. "src/backends/",   
}

-- 3rd:
inc_3rd = {
    inc_imgui,
    loc_3rd .. "miniaudio",
    loc_3rd .. "tiny_obj_loader",
    loc_3rd .. "khr",
    loc_3rd .. "stb",
}

-- core:
inc_core = {
    inc_3rd,
    loc_core,
    loc_core .. "**",
}

-- any game:

inc_game_base = {
    inc_3rd,
    inc_core,
}

-- @Note: Files

-- core:
fls_core = {
    loc_3rd .. "miniaudio/**.h", 
    loc_3rd .. "tiny_obj_loader/**.h", 
    loc_3rd .. "khr/**.h", 
    loc_3rd .. "stb/**.h", 
    loc_core .. "**.h",
    loc_core .. "**.cpp",  
}

-- @Note: Links

-- any game:
lnk_game_base = {
    "imgui",
    "core",
    "box2d",
}

lnk_game_win = {
    "User32.lib",
    "OpenGL32.lib",
    lnk_game_base,
}

---------------------------------------------
------------ PROJECT FUNCTIONS --------------
---------------------------------------------
function config_base()
    filter {}
        defines {
            gpu_api,
            "_CRT_SECURE_NO_WARNINGS",
            "_SILENCE_CXX23_ALIGNED_STORAGE_DEPRECATION_WARNING"
        }
    
    filter "configurations:debug"
        runtime  "Debug"
        symbols  "On"
        optimize "Off"
        defines  "GAME_DEBUG"

    filter "configurations:release"
        runtime  "Release"
        symbols  "On"
        optimize "On"
        defines  "GAME_RELEASE"

    filter "configurations:dist"
        runtime  "Release"
        symbols  "Off"
        optimize "On"
        defines  "GAME_DIST"

    filter "system:windows"
        defines "GAME_WIN"
    
    filter {}
end

function prj_base(name)
    project(name)
    language "C++"
    cppdialect (cpp_version)
    targetdir (loc_bin)
    objdir (loc_int)
        
end

function prj_lib(name)
    prj_base(name)
    kind "StaticLib"
end

function prj_app(name, loc)
    prj_base(name)
    debugdir (loc)
    filter "configurations:dist"
        kind "WindowedApp"
        entrypoint "mainCRTStartup"
    filter "configurations:debug"
        kind "ConsoleApp"
    filter "configurations:release"
        kind "ConsoleApp"
    filter {}
end

function prj_game(name, loc)

    local loc_prj = loc_this .. name
    if loc ~= nil then
        loc_prj = loc_this .. loc
    end

    prj_app(name, loc_prj)

    --- @Note: PCH config
    pchheader "game_pch.h"
    pchsource (loc_prj .. "/code" .. "/game_pch.cpp")
    forceincludes "game_pch.h"
    ---
    config_base()

    local loc = loc_this .. name 

    includedirs { 
        inc_game_base, 
        loc_box2d .. "include",
        loc_prj .. "/code/**",
    }

    files { 
        loc_prj .. "/code/**.h", 
        loc_prj .. "/code/**.cpp", 
        loc_prj .. "/assets/**.glsl", 
    }

    filter "system:windows"
        links(lnk_game_win)
end

---------------------------------------------
-------------- CONFIGURATION ----------------
---------------------------------------------

---------------------------------------------
---------------- WORKSPACE ------------------
---------------------------------------------

workspace "game"
    location ".project-files"
    architecture "x64"
    configurations { "debug", "release", "dist" }
    startproject "01_quads"

group "3rd"

---------------------------------------------
-------------- PROJECT IMGUI ----------------
---------------------------------------------

prj_lib "imgui"
    config_base()
    includedirs (inc_imgui)
    files { 
        loc_imgui .. "src/**.h", 
        loc_imgui .. "src/**.cpp", 
    }
    
    removefiles (loc_imgui .. "src/backends/**")
    
    filter "system:windows"
        files 
        { 
            loc_imgui .. "src/backends/imgui_impl_opengl3_loader.h", 
            loc_imgui .. "src/backends/imgui_impl_opengl3.h", 
            loc_imgui .. "src/backends/imgui_impl_opengl3.cpp", 
            loc_imgui .. "src/backends/imgui_impl_win32.h", 
            loc_imgui .. "src/backends/imgui_impl_win32.cpp" 
        }
    filter{}

---------------------------------------------
-------------- PROJECT BOX2D ----------------
---------------------------------------------

prj_lib "box2d"
    language "C"
    cdialect "C17"    
    config_base()
    includedirs {
        loc_box2d .. "include",
        loc_box2d .. "src"
    }
    files {
        loc_box2d .. "include/**.h",
        loc_box2d .. "src/**.h",
        loc_box2d .. "src/**.c"
    }
    buildoptions {
        "/experimental:c11atomics",
    }

---------------------------------------------
-------------- PROJECT CORE -----------------
---------------------------------------------
group ""

prj_lib "core"
    -- @Note: PCH config
    pchheader "core_pch.h"
    pchsource (loc_core .. "core_pch.cpp")
    forceincludes "core_pch.h"
    --
    config_base()
    includedirs(inc_core)
    files(fls_core)

---------------------------------------------
-------------------- GAME -------------------
---------------------------------------------

prj_game ("game", "game") 