
newaction {
    trigger = "android-app",
    description = "Android Application Project",
    
    onWorkspace = function(wks)
        
	end,

    onProject = function(prj)
        --premake.generate(prj, path.join("jni", "build.gradle"), generate_application)
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

function generate_application(prj)
    
end
