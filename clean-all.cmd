@echo off
echo removing bin\, bmi\, lib\, obj\ and .vs\ folders...
cd third_party\bullet
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\dr-mp3
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\freetype2
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\GLEW
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\hlslparser
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\libjpeg
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\libpng
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\libzip
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\ogg
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\openal-soft
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\tinyexr
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\tinyxml2
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\vorbis
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\xz-utils
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\zlib
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\..\core
if exist bmi rmdir /s /q bmi
if exist lib rmdir /s /q lib
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..\test
if exist bin rmdir /s /q bin
if exist obj rmdir /s /q obj
if exist projects\windows\.vs rmdir /s /q projects\windows\.vs
cd ..
echo done.