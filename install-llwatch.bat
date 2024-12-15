@echo off

set prog=llwatch
set devenv=F:\opt\VisualStudio\2022\Preview\Common7\IDE\devenv.exe 

echo "msbuild=%msbuild%"
if not exist "%msbuild%" (
echo Fall back msbuild not found at "%msbuild%"
set msbuild=F:\opt\VisualStudio\2022\Preview\MSBuild\Current\Bin\MSBuild.exe
)
echo "Msbuild=%msbuild%"

cd %prog%-ms

@echo Clean %proj% 
rmdir /s x64 2> nul

@echo.
@echo Build release target
@rem %devenv%  %prog%.sln /Build  "Release|x64"
echo "%msbuild%" %prog%.sln -p:Configuration="Release";Platform=x64 -verbosity:minimal  -detailedSummary:True
"%msbuild%" %prog%.sln -p:Configuration="Release";Platform=x64 -verbosity:minimal  -detailedSummary:True 
cd ..

@echo.
@echo ---- Build done 
if not exist "%prog%-ms\x64\Release\%prog%.exe" (
   echo Failed to build %prog%-ms\x64\Release\%prog%.exe
   goto _end
)
@echo.
@echo Copy Release to d:\opt\bin
dir %prog%-ms\x64\Release\%prog%.exe
copy %prog%-ms\x64\Release\%prog%.exe d:\opt\bin\%prog%.exe

@echo.
@echo Compare md5 hash
cmp -h %prog%-ms\x64\Release\%prog%.exe d:\opt\bin\%prog%.exe
ld -a -ph %prog%-ms\x64\Release\%prog%.exe d:\opt\bin\%prog%.exe

:_end
