
newaction {
    trigger = "android-lib",
    description = "Android Static Library Project",
    
    onWorkspace = function(wks)
        
	end,

    onProject = function(prj)
        premake.generate(prj, path.join("jni", "Application.mk"), generate_application_mk)
        premake.generate(prj, path.join("jni", "Android.mk"), generate_android_mk)
	end
}

premake.api.register
{
	name = "androidabis",
	scope = "project",
	kind = "list:string",
	allowed = {
		"armeabi",
		"armeabi-v7a",
		"arm64-v8a",
		"x86",
		"x86_64"
	}
}

premake.api.register 
{
	name = "androidminsdkversion",
	scope = "project",
	kind = "string"
}

function generate_application_mk(prj)
    local androidABIs = ""

    for _, abi in ipairs(prj.androidabis) do
        androidABIs = (androidABIs .. " " .. abi)
    end

    premake.x("APP_MODULES := " .. prj.targetname)
    
    if prj.androidminsdkversion then
        premake.x("APP_PLATFORM := " .. prj.androidminsdkversion)
    end

    premake.x("APP_ABI := " .. androidABIs)
    premake.x("APP_STL := c++_static")
    premake.x("APP_CPPFLAGS += -std=gnu++17 -frtti -fexceptions -fcoroutines-ts")
    premake.x("NDK_TOOLCHAIN_VERSION := 4.8")
    premake.x("")
end

function generate_android_mk(prj)
    local jniPath = path.join(prj.location, "jni")

    premake.x("LOCAL_PATH := $(call my-dir)")
    premake.x("")

    local outdir = path.join("$(LOCAL_PATH)", path.getrelative(path.join(prj.location, "jni"), prj.targetdir))
    local artdir = path.join("$(LOCAL_PATH)", path.getrelative(path.join(prj.location, "jni"), prj.objdir))
    local gdbdir = "$(TARGET_OUT)/setup.gdb"

    premake.x("TARGET_OUT := " .. outdir)
    premake.x("TARGET_OBJS := " .. artdir)
    premake.x("TARGET_GDB_SETUP := " .. gdbdir)
    premake.x("")

    premake.x("include $(CLEAR_VARS)")
    premake.x("")

    premake.x("LOCAL_MODULE := " .. prj.targetname)
    premake.x("LOCAL_ARM_MODE := arm")
    premake.x("")

    for cfg in premake.project.eachconfig(prj) do
        
        local configName = string.lower(cfg.name)
        premake.push("ifeq ($(APP_OPTIM)," .. configName .. ")")

        local localCFlags = "LOCAL_CFLAGS :="

        if cfg.optimize == "On" then
            localCFlags = localCFlags .. " -O3"
        end

        if cfg.defines then
            for _, def in ipairs(cfg.defines) do
                localCFlags = (localCFlags .. " -D" .. def)
            end
        end

        premake.x(localCFlags)

        premake.pop()
        premake.x("endif")
    end

    premake.x("")

    local includeDirs = "LOCAL_C_INCLUDES :="

    for _, dir in ipairs(prj.includedirs) do
        local includeDir = path.getrelative(jniPath, dir)
        includeDir = path.join("$(LOCAL_PATH)", includeDir)
        includeDirs = (includeDirs .. " " .. includeDir)
    end

    for _, dir in ipairs(prj.sysincludedirs) do
        local includeDir = path.getrelative(jniPath, dir)
        includeDir = path.join("$(LOCAL_PATH)", includeDir)
        includeDirs = (includeDirs .. " " .. includeDir)
    end

    premake.x(includeDirs)
    premake.x("")

    if prj.files then
        premake.push("LOCAL_SRC_FILES  := \\")
        
        local count = #prj.files
        
        local i = 0
        for _, file in ipairs(prj.files) do
            local sourceFile = path.getrelative(jniPath, file)

            if i < (count - 1) then
                sourceFile = (sourceFile .. " \\")
            end
            
            i = i + 1

            premake.x(sourceFile)
        end

        premake.pop()
        premake.x("")
    end

    premake.x("include $(BUILD_STATIC_LIBRARY)")
    premake.x("")
end
