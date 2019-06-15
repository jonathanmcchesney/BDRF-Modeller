# ELE4014 Advanced Visualisation - Jonathan McChesney 

## Project Description:
In this project I have attempted to create a BRDF/Specular/Lambertian lighting model demonstrator*.
*Which is implemented in GLSL Shaders.

I wanted to explore more realistic lighting models, having used Lambertian Reflectance, Phong Lighting &
the Blinn Phong Model in the past. I wanted to explore further the mathematics involved in creating these various
models and dive into the world of PBR (Physically Based Rendering). My end goal was to create a project that would
more accurately simulate the flow of light, using BRDF (Bidirectional Reflectance Distribution Function), Freshnel
reflectivity, conservation of energy, IBL (Image Based Lighting), Specular components (Specular IBL) and appropriate approximation
of Metalness & Microsurface Scattering (roughness):- based on the metallic workflow. Other aspects I took time to 
research and integrate were the split sum approximation (Epic Games), using pre-filtered HDR environment maps and 
importance sampling.

Furthermore I wanted to extend this and allow the user to alter the metalness & roughness of the material to show
how light reacts in different materials. By further texturing models I was able to create a more aesthetically 
pleasing and physically plausible approximation of lighting, in different environments.

### PBR Conditions:
-Must use a physically based Bidirectional Reflectance Distribution Function.
-Must remain energy conserving.
-Must be based on a microfacet level surface model.

Reflectance Equation: Lo(p,ωo)=∫Ω(kdcπ+ksDFG4(ωo⋅n)(ωi⋅n))Li(p,ωi)n⋅ωidωi

### How To Use:
	When you load up the executable a random HDR environment map is chosen (from a total of 5), this changes the light
	of the relevant scenes dramatically (especially when SpecularIBL is activated). There are 5 rows and column of purple spheres
	roughness is increased from left to right and metalness is increased from bottom to top. Using Enter you can toggle whether 
	SpecularIBL is applied to the scene (giving a nice reflective aesthetic using mip map levels from prefiltered maps) or whether
	the specular component is approximated by the light sources (bright areas on the sphere). Using "O" & "P "you can alter the colour
	of the spheres to static colours or dynamically changing colours. Using "Q" "E" "R" & "T" you can change the colour of the light 
	hitting the spheres from the light bulbs (loaded .obj models showing the point light locations). Using "J" & "K" you can also change 
	the  point light locations. Using "WASD" to move the camera and the mouse to look around you can move around the scene and see
	how light reacts in different situations. Using the "space bar" you can toggle whether or not the spheres are simply coloured or
	have a texture applied. With textures active use numbers "1" & "2" to cycle between the various textues (20 in total). Finally use
	"Y" & "U" to switch scenes, use U to switch to the next scene.

	In this scene we have a lone sphere, again we can use the various commands to change its colour, speculaIBL, texture etc.
	Unique to this scene (and the next) is that using the "Up" and "Down" arrows you can change the metalness of the sphere, it becomes
	darker and more an approximation of a metal surface as metalness increases. Also using "Left" and "Right" to alter the roughness of
	the material, making it less smooth as the roughness increases. You can play about with these parameters to see how their 
	properties affect the light. Using "G" you can also apply rotation to the cube to see how light reacts in motion. Toggle textures on
	and then see how each material performs while the sphere is moving and specularIBL is toggled On/Off. My favourite material is
	the smooth granite, it gives an aesthetically pleasing reflection when specular IBL is turned on, especially in the TV room backdrop.
	
	In the last scene instead of spheres is a loaded wavefront .obj model of a dinosaur. Again all the same parameters affect this scene,
	try toggling light colours, textures, metalness & roughness as well as specularIBL. Notice the translucency applied when untextured
	and specular IBL is active. When textured the textures translate nicely to this moe complex shape and so react well under different
	light scenes.

	The texures used to demonstrate PBR in this project were sourced from http://freepbr.com/, each material shows drastically different
	results under the same lighting, with some materials more/less reflective than others. This shows how the roughness & metalness
	properties of the materials (stored in their maps) all react differently. (See Bamboo Wood & Smooth Granite.)

	These are the following textures included:
		- Rusted Metal
		- Rusted Marbel
		- Treebark
		- Gold
		- Cork
		- Rock Crater
		- Smooth Granite 
		- Rubber
		- Stone
		- Painted Cement
		- Bamboo Wood 
		- Brick
		- Stone Floor
		- Copper Rock
		- Copper
		- Worn Iron
		- Mahogany Wood
		- Fabric
		- Grey Moss
		- Concrete
		- Rusted Iron
	
The project consists of three scenes:
	1) Various spheres orgainsed in rows and columms. Each row demonstrating the change in metalness 
	   property of the sphere (top being very metallic, bottom being less metallic (dielectric)). Each
	   column demonstrates the roughness property of the sphere (left being less rough, right being very
	   rough). The base colour of the spheres can be altered as can the applied light colour. The texture
	   applied to the sphere can also be toggled. (total of 20 different materials- each with their
	   own properties (metalness & roughness). Using Space you can toggle textured/untextured spheres, and 
	   using ENTER you can toggle specular IBL - showing off the specular component of the reflectance 
 	   equation.

	2) Single sphere, with the abilty to manually adjust the metalness & roughness of the sphere. Again
	    light colour and base colour of the sphere can be changed. As can the applied material be changed,
           	    the sphere can also be rotated accordingly using the G key. SPACE and ENTER can be used to toggle
                    Textured/Untextured & Specular IBL which results in a nice reflective aesthetic. Different Lighting
	     presets can also be cycled through using J/K, the light coloour can also be changed using Q, E, R & T.

	3) Loaded wavefront .obj file of a dinosaur. This scene shows how the light reacts with not only 
                    different material properties but with more complex shapes. Again all relevant commands can be issued
 	    here to toggle Textures, Light and Specular IBL.

### Features:
	- Common Features:
	I tried to Include as many features in this project as I could, from the ability to:
		- change light scenes, 
		- light colours, 
		- roughness & metalness, 
		- textures (20 avaliable use numbers 1 & 2 to switch between them when Texture is Applied (Space)),
		- toggle Specular IBL, 
		- different HDR background maps (6 avaliable randomly on startup), 
		- toggle visible convoluted Irradiance map, 
		- switch scenes,
		- sphere rotations, 
		- camera movement, 
		- on screen help text (hidden or visible).

### Controls:

- W / A / S / D:- Movement
- Mouse:- Moves Camera
- Q / E / R:- Change Light Colour
- T:- Reset Light Colour to White
- O / P:- Change Sphere Basecolour
- F:- Toggle Convoluted Irradiance Map
- SPACE:- Toggle Textured/ Untextured Mode
- ENTER:- Toogle Specular IBL
- Y / U:- Switch Between Scenes
- Numbers 1 / 2:- Change Materials
- UP / DOWN:- Change Metalness
- LEFT / RIGHT:- Change Roughness
- H:- Hide/Unhide Text 
- J/K :- Cycle Through Lighting Presets

### Files:
	.h & .cpp files:
		- Camera.h
		- Constants.h		
		- Cube.cpp, Cube.h
		- IBL.cpp, IBL.h
		- main.cpp
		- Mesh.h
		- Model.h
		- Object.cpp, Object.h
		- Quad.cpp, Quad.h
		- Scenes.cpp, Scenes.h
		- Shader.h
		- Sphere.cpp, Sphere.h
		- Text.cpp, Text.h
		- Texture.cpp, Texture.h

	shader files:
		- backgroundPBR.vertex & backgroundPBR.fragment
		- brdf.vertex, brdg.fragment
		- cubemapAdvanced.vertex
		- cubemapConversion.fragment
		- cubemapConvolution.fragment
		- pbrFinal.vertex
		- pbrFinalTextured.fragment, pbrFinalUntextured.fragment
		- pbrPrefilter.fragment
		- pbrTextured.fragment, pbrUntextured.fragment
		- text.vertex & text.fragment

	other files:
		- stb_image.cpp & stb_*.h (sourced online for HDR environment map integration header)
		- glm (Maths Lib), Freetype (Text Lib), SOIL2 (Texture Loading), Assimp (Asset Importer)
		- relevant models, HDR images and assets in the res folder

	unimplemented files:
		PerlinNoise.cpp & PerlinNoise.h (sourced online)
		imgui.cpp, imconfig.h, imgui.h & imgui_* (changed mind on using this 3rd party GUI)

### Sources:
CSC3049 & ELE4014 - Lectures
https://www.opengl.org/
www.opengl-tutorial.org/
Practical Algorithms for 3D Computer Graphics.
LearnOpenGL.com & Offline Transcript.
http://www.codinglabs.net/article_physically_based_rendering.aspx
http://mrl.nyu.edu/~perlin/demox/Planet.html
http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter26.html

### Robustness & Stability:
	I focused on creating both a robust and stable project that would run on a plethora of machines, this
	allowed me to insure that the project would not crash when running.

### Simplicity & Code Design:
	Another area of focus was on creating a simple solution to a complex problem using correct coding
	standards, which while easier said than done allowed me to create more high quality code that is
	easily maintained and extended.
	
### Usability:
	Usabilty again was an area I made sure was focused on, to run the project simply run the executable within
	the folder containing:
		- assimp.exe & Assimp32.dll
		- freetype6.dll & glew32.dll
		- res (Resource Folder)
