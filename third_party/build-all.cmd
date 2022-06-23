call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
cd bullet\projects\windows
msbuild BulletPhysics.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild BulletPhysics.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\dr-mp3\projects\windows
msbuild dr-mp3.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild dr-mp3.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\freetype2\projects\windows
msbuild freetype2.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild freetype2.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\GLEW\projects\windows
msbuild glew.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild glew.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\graph-collector\projects\windows
msbuild graph-collector.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild graph-collector.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\hlslparser\projects\windows
msbuild hlslparser.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild hlslparser.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\libjpeg\projects\windows
msbuild libjpeg.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild libjpeg.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\libpng\projects\windows
msbuild libpng.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild libpng.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\libzip\projects\windows
msbuild libzip.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild libzip.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\ogg\projects\windows
msbuild ogg.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild ogg.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\openal-soft\projects\windows
msbuild openal-soft.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild openal-soft.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\tinyexr\projects\windows
msbuild tinyexr.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild tinyexr.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\tinyxml2\projects\windows
msbuild tinyxml2.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild tinyxml2.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\vorbis\projects\windows
msbuild vorbis.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild vorbis.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\xz-utils\projects\windows
msbuild xz-utils.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild xz-utils.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..\zlib\projects\windows
msbuild zlib.sln /p:Configuration=Debug /t:Clean;Rebuild
msbuild zlib.sln /p:Configuration=Release /t:Clean;Rebuild
cd ..\..\..
