@echo off

if "%1"=="" (
	call :generate %*
) else (
	call :%*
)

goto :EOF

rem --- Generate ---------------------------------------------------------------
:generate

echo Generating project files for Visual C++ 2010 ...

mkdir vc100

set PROJECTS=

for %%i in ( *.cpp ) do (
	call :add %%~ni
	mkdir vc100\%%~ni
	call :proj %%~ni > vc100\%%~ni\%%~ni.vcxproj
)
call :sln > vc100.sln

goto :EOF

rem --- Projects list ----------------------------------------------------------
:add

set PROJECTS=%PROJECTS%^ %*

goto :EOF

rem --- Project ----------------------------------------------------------------
:proj

echo ^<?xml version="1.0" encoding="utf-8"?^>
echo ^<Project DefaultTargets="Build" ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003"^>
echo  ^<ItemGroup Label="ProjectConfigurations"^>
echo    ^<ProjectConfiguration Include="Debug|Win32"^>
echo      ^<Configuration^>Debug^</Configuration^>
echo      ^<Platform^>Win32^</Platform^>
echo    ^</ProjectConfiguration^>
echo    ^<ProjectConfiguration Include="Debug|x64"^>
echo      ^<Configuration^>Debug^</Configuration^>
echo      ^<Platform^>x64^</Platform^>
echo    ^</ProjectConfiguration^>
echo    ^<ProjectConfiguration Include="Release|Win32"^>
echo      ^<Configuration^>Release^</Configuration^>
echo      ^<Platform^>Win32^</Platform^>
echo    ^</ProjectConfiguration^>
echo    ^<ProjectConfiguration Include="Release|x64"^>
echo      ^<Configuration^>Release^</Configuration^>
echo      ^<Platform^>x64^</Platform^>
echo    ^</ProjectConfiguration^>
echo  ^</ItemGroup^>
echo  ^<PropertyGroup Label="Globals"^>
echo    ^<ProjectGuid^>{A0C7723D-C7BD-4D03-A11B-D072490C4590}^</ProjectGuid^>
echo    ^<Keyword^>Win32Proj^</Keyword^>
echo    ^<RootNamespace^>%1^</RootNamespace^>
echo  ^</PropertyGroup^>
echo  ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" /^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration"^>
echo    ^<ConfigurationType^>Application^</ConfigurationType^>
echo    ^<UseDebugLibraries^>true^</UseDebugLibraries^>
echo    ^<CharacterSet^>Unicode^</CharacterSet^>
echo  ^</PropertyGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration"^>
echo    ^<ConfigurationType^>Application^</ConfigurationType^>
echo    ^<UseDebugLibraries^>true^</UseDebugLibraries^>
echo    ^<CharacterSet^>Unicode^</CharacterSet^>
echo  ^</PropertyGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration"^>
echo    ^<ConfigurationType^>Application^</ConfigurationType^>
echo    ^<UseDebugLibraries^>false^</UseDebugLibraries^>
echo    ^<WholeProgramOptimization^>true^</WholeProgramOptimization^>
echo    ^<CharacterSet^>Unicode^</CharacterSet^>
echo  ^</PropertyGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration"^>
echo    ^<ConfigurationType^>Application^</ConfigurationType^>
echo    ^<UseDebugLibraries^>false^</UseDebugLibraries^>
echo    ^<WholeProgramOptimization^>true^</WholeProgramOptimization^>
echo    ^<CharacterSet^>Unicode^</CharacterSet^>
echo  ^</PropertyGroup^>
echo  ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" /^>
echo  ^<ImportGroup Label="ExtensionSettings"^>
echo  ^</ImportGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'"^>
echo    ^<LinkIncremental^>true^</LinkIncremental^>
echo  ^</PropertyGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'"^>
echo    ^<LinkIncremental^>true^</LinkIncremental^>
echo  ^</PropertyGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'"^>
echo    ^<LinkIncremental^>false^</LinkIncremental^>
echo  ^</PropertyGroup^>
echo  ^<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'"^>
echo    ^<LinkIncremental^>false^</LinkIncremental^>
echo  ^</PropertyGroup^>
echo  ^<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'"^>
echo    ^<ClCompile^>
echo      ^<PrecompiledHeader^>^</PrecompiledHeader^>
echo      ^<WarningLevel^>Level3^</WarningLevel^>
echo      ^<Optimization^>Disabled^</Optimization^>
echo      ^<PreprocessorDefinitions^>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)^</PreprocessorDefinitions^>
echo      ^<AdditionalIncludeDirectories^>../../../include^</AdditionalIncludeDirectories^>
echo    ^</ClCompile^>
echo    ^<Link^>
echo      ^<SubSystem^>Console^</SubSystem^>
echo      ^<GenerateDebugInformation^>true^</GenerateDebugInformation^>
echo    ^</Link^>
echo  ^</ItemDefinitionGroup^>
echo  ^<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'"^>
echo    ^<ClCompile^>
echo      ^<PrecompiledHeader^>^</PrecompiledHeader^>
echo      ^<WarningLevel^>Level3^</WarningLevel^>
echo      ^<Optimization^>Disabled^</Optimization^>
echo      ^<PreprocessorDefinitions^>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)^</PreprocessorDefinitions^>
echo      ^<AdditionalIncludeDirectories^>../../../include^</AdditionalIncludeDirectories^>
echo    ^</ClCompile^>
echo    ^<Link^>
echo      ^<SubSystem^>Console^</SubSystem^>
echo      ^<GenerateDebugInformation^>true^</GenerateDebugInformation^>
echo    ^</Link^>
echo  ^</ItemDefinitionGroup^>
echo  ^<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'"^>
echo    ^<ClCompile^>
echo      ^<WarningLevel^>Level3^</WarningLevel^>
echo      ^<PrecompiledHeader^>^</PrecompiledHeader^>
echo      ^<Optimization^>MaxSpeed^</Optimization^>
echo      ^<FunctionLevelLinking^>true^</FunctionLevelLinking^>
echo      ^<IntrinsicFunctions^>true^</IntrinsicFunctions^>
echo      ^<PreprocessorDefinitions^>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)^</PreprocessorDefinitions^>
echo      ^<AdditionalIncludeDirectories^>../../../include^</AdditionalIncludeDirectories^>
echo    ^</ClCompile^>
echo    ^<Link^>
echo      ^<SubSystem^>Console^</SubSystem^>
echo      ^<GenerateDebugInformation^>true^</GenerateDebugInformation^>
echo      ^<EnableCOMDATFolding^>true^</EnableCOMDATFolding^>
echo      ^<OptimizeReferences^>true^</OptimizeReferences^>
echo    ^</Link^>
echo  ^</ItemDefinitionGroup^>
echo  ^<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'"^>
echo    ^<ClCompile^>
echo      ^<WarningLevel^>Level3^</WarningLevel^>
echo      ^<PrecompiledHeader^>^</PrecompiledHeader^>
echo      ^<Optimization^>MaxSpeed^</Optimization^>
echo      ^<FunctionLevelLinking^>true^</FunctionLevelLinking^>
echo      ^<IntrinsicFunctions^>true^</IntrinsicFunctions^>
echo      ^<PreprocessorDefinitions^>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)^</PreprocessorDefinitions^>
echo      ^<AdditionalIncludeDirectories^>../../../include^</AdditionalIncludeDirectories^>
echo    ^</ClCompile^>
echo    ^<Link^>
echo      ^<SubSystem^>Console^</SubSystem^>
echo      ^<GenerateDebugInformation^>true^</GenerateDebugInformation^>
echo      ^<EnableCOMDATFolding^>true^</EnableCOMDATFolding^>
echo      ^<OptimizeReferences^>true^</OptimizeReferences^>
echo    ^</Link^>
echo  ^</ItemDefinitionGroup^>
echo  ^<ItemGroup^>
echo    ^<ClCompile Include="..\..\%1.cpp" /^>
echo  ^</ItemGroup^>
echo  ^<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" /^>
echo  ^<ImportGroup Label="ExtensionTargets"^>
echo  ^</ImportGroup^>
echo ^</Project^>


goto :EOF

rem --- Workspace --------------------------------------------------------------
:sln

setlocal enabledelayedexpansion

echo Microsoft Visual Studio Solution File, Format Version 11.00
echo # Visual Studio 2010
echo.

set "Counter=0"
set "list="
for %%i in ( %PROJECTS% ) do (
	set /A "Counter+=1"
	call :GUID guid !Counter!
	if not "!list!"=="" set "list=!list!;"
	set "list=!list!!guid!"
	echo Project^("{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}"^) = "%%i", ".\vc100\%%i\%%i.vcxproj", "{!guid!}"
	echo EndProject
)

echo Global
echo 	GlobalSection(SolutionConfigurationPlatforms) = preSolution
echo 		Debug^|Win32 = Debug^|Win32
echo 		Debug^|x64 = Debug^|x64
echo 		Release^|Win32 = Release^|Win32
echo 		Release^|x64 = Release^|x64
echo 	EndGlobalSection
echo 	GlobalSection(ProjectConfigurationPlatforms) = postSolution

for %%g in (%list%) do (
	echo		{%%g}.Debug^|Win32.ActiveCfg = Debug^|Win32
	echo		{%%g}.Debug^|Win32.Build.0 = Debug^|Win32
	echo		{%%g}.Debug^|x64.ActiveCfg = Debug^|x64
	echo		{%%g}.Debug^|x64.Build.0 = Debug^|x64
	echo		{%%g}.Release^|Win32.ActiveCfg = Release^|Win32
	echo		{%%g}.Release^|Win32.Build.0 = Release^|Win32
	echo		{%%g}.Release^|x64.ActiveCfg = Release^|x64
	echo		{%%g}.Release^|x64.Build.0 = Release^|x64
)

echo 	EndGlobalSection
echo 	GlobalSection(SolutionProperties) = preSolution
echo 		HideSolutionNode = FALSE
echo 	EndGlobalSection
echo EndGlobal

endlocal

goto :EOF

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:GUID <xReturn> <xStep>
:: Generate a GUID without delayed expansion.
setlocal
set "xStep=%~2"
set "xGUID="
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
set "xGUID=%xGUID%-"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
set "xGUID=%xGUID%-"
set "xGUID=%xGUID%4"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
set "xGUID=%xGUID%-"
set "xGUID=%xGUID%A"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
set "xGUID=%xGUID%-"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
call :AppendHex xGUID "%xGUID%" "%xStep%"
endlocal & if not "%~1"=="" set "%~1=%xGUID%"
goto :eof
:: by David Ruhmann

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:AppendHex <xReturn> <xInput> <xStep>
:: Append a hexidecimal number to the end of the input.
:: 1. Generate Random Number = 0-15
:: 2. Convert Number to Hexidecimal
:: 3. Append to Input
setlocal
set "xStep=%~3"
set /a xValue=%random%+%xStep%
set /a "xValue=%xValue% %% 16"
if "%xValue%"=="10" set "xValue=A"
if "%xValue%"=="11" set "xValue=B"
if "%xValue%"=="12" set "xValue=C"
if "%xValue%"=="13" set "xValue=D"
if "%xValue%"=="14" set "xValue=E"
if "%xValue%"=="15" set "xValue=F"
endlocal & if not "%~1"=="" set "%~1=%~2%xValue%"
goto :eof
:: by David Ruhmann


pause
