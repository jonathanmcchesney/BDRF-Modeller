#include "IBL.h"

IBL::IBL() {
}
// This class is the main meat of the project
// This method is used to set up the Image Based Lighting component of the project
void IBL::setUp(Shader &equirectangularToCubemapShader, Shader &irradianceShader, Shader &pbrShader, Shader &backgroundShader,  Shader &shader, bool textured, glm::mat4 *projection, Shader &brdfShader, Shader &prefilterShader) {
	// The physically based rendering shader is used, attributes are set depending on whether the scene is textured or not
	pbrShader.UseCurrentShader();
	if (textured == true)
	{
		glUniform1i(glGetUniformLocation(pbrShader.Program, "prefilterMap"), 0);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "brdfLUT"), 1);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "albedoMap"), 2);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "normalMap"), 3);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "metallicMap"), 4);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "roughnessMap"), 5);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "aoMap"), 6);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "irradianceMap"), 7);
	}
	else
	{
		glUniform1i(glGetUniformLocation(pbrShader.Program, "prefilterMap"), 0);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "brdfLUT"), 1);
		glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 0.5f, 0.0f, 5.0f);
		glUniform1f(glGetUniformLocation(pbrShader.Program, "ao"), 1.0f);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "irradianceMap"), 4);

	}

	// Background shader is used, env map attribute is set (0)
	backgroundShader.UseCurrentShader();
	glUniform1i(glGetUniformLocation(backgroundShader.Program, "environmentMap"), 0);

	// Generate and render the framebuffers/ renderbuffers & bind accordingly - 512 x 512 resolution - efficient for nice results
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	// use of stbi class header - sourced online - for HDR background use
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;

	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, 5);
	auto random_integer = uni(rng);
	auto random_integer_2 = uni(rng);
	float *hdrData = 0;

	// Randomly select an HDR background to be used in IBL
	switch (random_integer) {
	case 0:
		hdrData = stbi_loadf("res/images/pbr/Alexs_Apt_2k.hdr", &width, &height, &nrComponents, 0);
		break;
	case 1:
		hdrData = stbi_loadf("res/images/pbr/Walk_Of_Fame/Mans_Outside_2k.hdr", &width, &height, &nrComponents, 0);
		break;
	case 2:
		hdrData = stbi_loadf("res/images/pbr/Factory_Catwalk/Factory_Catwalk_2k.hdr", &width, &height, &nrComponents, 0);
		break;
	case 3:
		hdrData = stbi_loadf("res/images/pbr/Circus_Backstage/Circus_Backstage_3k.hdr", &width, &height, &nrComponents, 0);
		break;
	case 4:
		hdrData = stbi_loadf("res/images/pbr/Theatre_Seating/Theatre-Side_2k.hdr", &width, &height, &nrComponents, 0);
		break;
	case 5:
		hdrData = stbi_loadf("res/images/pbr/Frozen_Waterfall/Frozen_Waterfall_Ref.hdr", &width, &height, &nrComponents, 0);
		break;
	default:
		break;
	}

	// if data is not empty - if a HDR background is selected, handle accordignly - CLAMP to edge is important in removing artefacts & reference Min etc filter accordingly
	unsigned int hdrTexture;
	if (hdrData)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, hdrData); // note how the data value is set to be float

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(hdrData); // set the stbi image as data - hdr Image
	}
	else
	{
		// use the terminal to log out statements if an exception occurs - in this case if hdrData is empty
		std::cout << "Failed to in load a HDR image." << std::endl;
	}

	// now we are generating the environment cubve map, setting the resolution to 512 x 512 accordingly, the faces are mapped and clapping and references
	// to the mipmapping are also used here - most of the fancy stuff for generating the env cube map occurs in the shader
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// We need to do these following steps to capture a perspective - view thats looking at each of the cubemaps faces
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// This was the difficult portion of this user story - changing an equirectangular map (sphere version of a cubemap) into a cubemap.
	// The reason for this is that typically a sphere holds a lot of data horizontally (less vertically) and we want to use this extra
	// detail in the creation of the environment lighting.
	equirectangularToCubemapShader.UseCurrentShader();
	glUniform1i(glGetUniformLocation(equirectangularToCubemapShader.Program, "equirectangularMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glUniformMatrix4fv(glGetUniformLocation(equirectangularToCubemapShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

	glViewport(0, 0, 512, 512); // set viewpoint parameters to 512 x 512 as previously set by env map
	// using the framebuffer - (this part is where the magic happens) - covert to cubemap accordingly, using the capture view points that point towards each face
	// attach as a colour attachment and finally clear the colour and depth buffer bit
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(equirectangularToCubemapShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // MAKE SURE TO SWITCH BACK TO THE DEFAULT BUFFER

	// remove visible dot artifacts - 
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Irradiance cubemap - same as before except with a resolution of 32 x 32 - noticably more blurry
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32); // setting storage as appropriate

	// solving the integral using irradiance cubemap - integral consists of both a diffuse and specular component each solved sepeartly (the specular is solved
	// through the use of the split sum - breaking the problem down further). THe irradiance shader is set and envMap attached as an attribute and binded.
	irradianceShader.UseCurrentShader();
	glUniform1i(glGetUniformLocation(irradianceShader.Program, "environmentMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glUniformMatrix4fv(glGetUniformLocation(irradianceShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

	glViewport(0, 0, 32, 32); // reset viewport dimensions respectively to 32 x 32 - same process as before but now for the irradiance cubemap - the blurry
	// map used for creating nice visual residue - i.e in the tv scene the blue emited from the screen is reflected off the back of the spheres, the lack
	// of defined detail means its a nice diffuse lighting effect (model).
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glUniformMatrix4fv(glGetUniformLocation(irradianceShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.renderCube(); // render cube using the cube class
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Prefilter cubemap - indirect specular component - need to compute this before running - i.e. before run time for efficiency.
	// this is for dealing with various roughness levels in the environments at different mipmap levels - handling reflection roughness essentially
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	// we are sampling so make sure to set the min filter as appropriate
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // mipmaps - yay

	// Quasi monte-carlo equation - tough one to learn but is used for a higher rate of convergence when dealing with a low
	// discrepency sequence for importance sampling - what portions of the population to sample
	prefilterShader.UseCurrentShader();
	glUniform1i(glGetUniformLocation(prefilterShader.Program, "environmentMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	glUniformMatrix4fv(glGetUniformLocation(prefilterShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 7; // change this to what value you want - for sample levels - 7 is a nice base to start at
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		mipWidth = 128 * std::pow(0.5, mip); // setting the mip width for each mip level - how rough/blurry
		mipHeight = 128 * std::pow(0.5, mip); // same for the height
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1); // actual roughness level is set here - taking the current mip level divided by the total - 1 (**begins at 0)
		glUniform1f(glGetUniformLocation(prefilterShader.Program, "roughness"), roughness);
		// Similar process as before - use the glm::lookat capture views - each face of the cubemap
		for (unsigned int i = 0; i < 6; ++i)
		{
			glUniformMatrix4fv(glGetUniformLocation(prefilterShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear the bits and render cube
			cube.renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer

	// 2D LUT - Bidirectional Reflection Distribution Function. - the fun part of the project - second part of the specular split sum approximation equation
	// the meat of this is within the fragment and vertex shader. Nothing here is drastically different from the other maps being used - textue is binded, 
	// resolution is set, framebuffer is used as appropriate etc. LUT = BDRF integration map (havent seen integration in years) of stored convoluted results.
	glGenTextures(1, &brdfLUTTexture);

	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);

	glViewport(0, 0, 512, 512);
	brdfShader.UseCurrentShader();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quad.RenderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Projections are handled accordingly for all the shaders.
	shader.UseCurrentShader();
	glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
	pbrShader.UseCurrentShader();
	glUniformMatrix4fv(glGetUniformLocation(pbrShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
	backgroundShader.UseCurrentShader();
	glUniformMatrix4fv(glGetUniformLocation(backgroundShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(*projection));
}

// Method is used to show the irradiance map & prefilter map (uncomment), just to give a visual aid as to the effect
// that the irradiance map has on the environment
void IBL::showMap(Shader &backgroundShader, bool showIrradiance, glm::mat4 *view, Shader &bdrfShader) {
	backgroundShader.UseCurrentShader();
	glUniformMatrix4fv(glGetUniformLocation(backgroundShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(*view));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	if (showIrradiance == true) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap); 

	}
	cube.renderCube();
}

// This (inefficient) method allows for the current applied textures (albedo, normal, metalness, roughness and ambient occlusion to be changed + texture name set.
// There are 20 textures in total each with their own roughness, metalness and reflectivity - sampled from their maps.
string IBL::ChangeTexture(int currentTexture) {
	// A switch statment is used here with an int param takin in for the current texture
	switch (currentTexture) {
	case 0:
		albedo = TextureLoading::LoadTexture("res/images/albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Rusted Metal";
		break;
	case 1:
		albedo = TextureLoading::LoadTexture("res/images/albedo2.png", false);
		normal = TextureLoading::LoadTexture("res/images/normal2.png", false);
		metallic = TextureLoading::LoadTexture("res/images/metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao2.png", false);
		texture = "Rusted Marbel";
		break;
	case 2:
		albedo = TextureLoading::LoadTexture("res/images/tex/bark1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/bark1-normal3.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/bark1-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/bark1-rough.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/bark1-ao.png", false);
		texture = "Treebark";
		break;
	case 3:
		albedo = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_basecolor-boosted.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/bark1-ao.png", false);
		texture = "Gold";
		break;
	case 4:
		albedo = TextureLoading::LoadTexture("res/images/tex/corkboard3b-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/corkboard3b-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/corkboard3b-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/corkboard3b-roughnness.png", false);
		ao = TextureLoading::LoadTexture("res/images/corkboard3b-ao.png", false);
		texture = "Cork";
		break;
	case 5:
		albedo = TextureLoading::LoadTexture("res/images/tex/cratered-rock-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/cratered-rock-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/cratered-rock-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/cratered-rock-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/cratered-rock-ao.png", false);
		texture = "Rock Crater";
		break;
	case 6:
		albedo = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-albedo3.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-normal2.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-roughness3.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Smooth Granite";
		break;
	case 7:
		albedo = TextureLoading::LoadTexture("res/images/tex/synth-rubber-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/synth-rubber-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/synth-rubber-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/synth-rubber-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Rubber";
		break;
	case 8:
		albedo = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2_Ambient_Occlusion.png", false);
		texture = "Stone";
		break;
	case 9:
		albedo = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-norrmal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-ao.png", false);
		texture = "Painted Cement";
		break;
	case 10:
		albedo = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-ao.png", false);
		texture = "Bamboo Wood";
		break;
	case 11:
		albedo = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_basecolor.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_ambientocclusion.png", false);
		texture = "Block";
		break;
	case 12:
		albedo = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Base_Color.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Ambient_Occlusion.png", false);
		texture = "Stone Floor";
		break;
	case 13:
		albedo = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-alb.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-rough.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		texture = "Copper Rock";
		break;
	case 14:
		albedo = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_basecolor-boosted.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		texture = "Copper";
		break;
	case 15:
		albedo = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		texture = "Worn Iron";
		break;
	case 16:
		albedo = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_basecolor.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_Height.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_AO.png", false);
		texture = "Mahogany Wood";
		break;
	case 17:
		albedo = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-albedo3.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-metalic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-roughness2.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-ao.png", false);
		texture = "Fabric";
		break;
	case 18:
		albedo = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-preview.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-ao.png", false);
		texture = "Grey Moss";
		break;
	case 19:
		albedo = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-ao.png", false);
		texture = "Concrete";
		break;
	case 20:
		albedo = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_basecolor.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Rusted Iron";
		break;
	default:
		texture = "default";
		break;
	}
	return texture;
}

// This method changes the value of the pbrShader as well as the attributes set for a textured/ untextured scene
Shader IBL::ChangeShader(Shader &tex, Shader &unTex, Shader &pbrShader, Shader &simpleTex, Shader &simpleUntex, bool textured, bool specular, int colourPreset) {
	if (textured == true)
	{
		if (specular == true)
			pbrShader = tex;
		else
			pbrShader = simpleTex;
		glUniform1i(glGetUniformLocation(pbrShader.Program, "prefilterMap"), 0);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "brdfLUT"), 1);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "albedoMap"), 2);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "normalMap"), 3);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "metallicMap"), 4);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "roughnessMap"), 5);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "aoMap"), 6);
		glUniform1i(glGetUniformLocation(pbrShader.Program, "irradianceMap"), 7);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, albedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normal);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, metallic);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, roughness);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, ao);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	}
	else
	{
		if (specular == true)
			pbrShader = unTex;
		else
			pbrShader = simpleUntex;
		glUniform1i(glGetUniformLocation(pbrShader.Program, "irradianceMap"), 0); // irradiance map
		glUniform1i(glGetUniformLocation(pbrShader.Program, "prefilterMap"), 1); // prefilter map
		glUniform1i(glGetUniformLocation(pbrShader.Program, "brdfLUT"), 2); // bdrf map
		if (colourPreset == 0) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 0.5, 0.00, 0.50); // set albedo - base diffuse colour
		}
		if (colourPreset == 1) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 0.00, 1.0, 0.00);
		}
		if (colourPreset == 2) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 0.00, 0.00, 1.0);
		}
		if (colourPreset == 3) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 0.5, 0.5, 0.00);
		}
		if (colourPreset == 4) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 1.0, 0.0, 0.0);
		}
		if (colourPreset == 5) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), 0.00, 0.5, 0.5);
		}
		if (colourPreset == 6) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), (float)sin(glfwGetTime()) * 0.5f + 0.5f, (float)cos(glfwGetTime()) * 0.5f + 0.5f, (float)cos(glfwGetTime()) * 0.5f + 0.5f);
		}
		if (colourPreset == 7) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), (float)cos(glfwGetTime()) * 0.5f + 0.5f, (float)cos(glfwGetTime()) * 0.5f + 0.5f, (float)sin(glfwGetTime()) * 0.5f + 0.5f);
		}
		if (colourPreset == 8) {
			glUniform3f(glGetUniformLocation(pbrShader.Program, "albedo"), (float)sin(glfwGetTime()) * 0.5f + 0.5f, (float)cos(glfwGetTime()) * 0.5f + 0.5f, (float)sin(glfwGetTime()) * 0.5f + 0.5f);
		}
		glUniform1f(glGetUniformLocation(pbrShader.Program, "ao"), 1.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
	}

		return pbrShader; // return the newly set pbrShader
}

/*
string IBL::ChangeTexture(int currentTexture) {
		switch (currentTexture) {
		case 0:
		albedo = albedo0;
		normal = normal0;
		metallic = metallic0;
		roughness = roughness0;
		ao = ao0;
		texture = "Rusted Metal";
		break;
		case 1:
		albedo = TextureLoading::LoadTexture("res/images/albedo2.png", false);
		normal = TextureLoading::LoadTexture("res/images/normal2.png", false);
		metallic = TextureLoading::LoadTexture("res/images/metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao2.png", false);
		texture = "Rusted Marbel";
		break;
		case 2:
		albedo = TextureLoading::LoadTexture("res/images/tex/bark1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/bark1-normal3.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/bark1-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/bark1-rough.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/bark1-ao.png", false);
		texture = "Treebark";
		break;
		case 3:
		albedo = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_basecolor-boosted.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/bark1-ao.png", false);
		texture = "Gold";
		break;
		case 4:
		albedo = TextureLoading::LoadTexture("res/images/tex/corkboard3b-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/corkboard3b-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/corkboard3b-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/corkboard3b-roughnness.png", false);
		ao = TextureLoading::LoadTexture("res/images/corkboard3b-ao.png", false);
		texture = "Cork";
		break;
		case 5:
		albedo = TextureLoading::LoadTexture("res/images/tex/cratered-rock-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/cratered-rock-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/cratered-rock-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/cratered-rock-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/cratered-rock-ao.png", false);
		texture = "Rock Crater";
		break;
		case 6:
		albedo = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-albedo3.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-normal2.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-roughness3.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Smooth Granite";
		break;
		case 7:
		albedo = TextureLoading::LoadTexture("res/images/tex/synth-rubber-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/synth-rubber-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/synth-rubber-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/synth-rubber-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Rubber";
		break;
		case 8:
		albedo = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2_Ambient_Occlusion.png", false);
		texture = "Stone";
		break;
		case 9:
		albedo = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-norrmal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-ao.png", false);
		texture = "Painted Cement";
		break;
		case 10:
		albedo = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-ao.png", false);
		texture = "Bamboo Wood";
		break;
		case 11:
		albedo = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_basecolor.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_ambientocclusion.png", false);
		texture = "Block";
		break;
		case 12:
		albedo = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Base_Color.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Ambient_Occlusion.png", false);
		texture = "Stone Floor";
		break;
		case 13:
		albedo = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-alb.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-rough.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		texture = "Copper Rock";
		break;
		case 14:
		albedo = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_basecolor-boosted.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		texture = "Copper";
		break;
		case 15:
		albedo = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		texture = "Worn Iron";
		break;
		case 16:
		albedo = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_basecolor.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_Height.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_AO.png", false);
		texture = "Mahogany Wood";
		break;
		case 17:
		albedo = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-albedo3.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-metalic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-roughness2.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-ao.png", false);
		texture = "Fabric";
		break;
		case 18:
		albedo = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-preview.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-metal.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-ao.png", false);
		texture = "Grey Moss";
		break;
		case 19:
		albedo = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-albedo.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-metalness.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-ao.png", false);
		texture = "Concrete";
		break;
		case 20:
		albedo = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_basecolor.png", false);
		normal = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_normal.png", false);
		metallic = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_metallic.png", false);
		roughness = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_roughness.png", false);
		ao = TextureLoading::LoadTexture("res/images/ao.png", false);
		texture = "Rusted Iron";
		break;
		default:
		texture = "default";
		break;
		}
		return texture;
}*/

/*
void IBL::LoadTextures() {
		albedo0 = TextureLoading::LoadTexture("res/images/albedo.png", false);
		normal0 = TextureLoading::LoadTexture("res/images/normal.png", false);
		metallic0 = TextureLoading::LoadTexture("res/images/metallic.png", false);
		roughness0 = TextureLoading::LoadTexture("res/images/roughness.png", false);
		ao0 = TextureLoading::LoadTexture("res/images/ao.png", false);

		albedo1 = TextureLoading::LoadTexture("res/images/albedo2.png", false);
		normal1 = TextureLoading::LoadTexture("res/images/normal2.png", false);
		metallic1 = TextureLoading::LoadTexture("res/images/metallic.png", false);
		roughness1 = TextureLoading::LoadTexture("res/images/roughness.png", false);
		ao1 = TextureLoading::LoadTexture("res/images/ao2.png", false);

		albedo2 = TextureLoading::LoadTexture("res/images/tex/bark1-albedo.png", false);
		normal2 = TextureLoading::LoadTexture("res/images/tex/bark1-normal3.png", false);
		metallic2 = TextureLoading::LoadTexture("res/images/tex/bark1-metalness.png", false);
		roughness2 = TextureLoading::LoadTexture("res/images/tex/bark1-rough.png", false);
		ao2 = TextureLoading::LoadTexture("res/images/tex/bark1-ao.png", false);
		
		albedo3 = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_basecolor-boosted.png", false);
		normal3 = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_normal.png", false);
		metallic3 = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_metallic.png", false);
		roughness3 = TextureLoading::LoadTexture("res/images/tex/gold-scuffed-Unreal-Engine/gold-scuffed_roughness.png", false);
		ao3 = TextureLoading::LoadTexture("res/images/tex/bark1-ao.png", false);
		
		albedo4 = TextureLoading::LoadTexture("res/images/tex/corkboard3b-albedo.png", false);
		normal4 = TextureLoading::LoadTexture("res/images/tex/corkboard3b-normal.png", false);
		metallic4 = TextureLoading::LoadTexture("res/images/tex/corkboard3b-metalness.png", false);
		roughness4 = TextureLoading::LoadTexture("res/images/tex/corkboard3b-roughnness.png", false);
		ao4 = TextureLoading::LoadTexture("res/images/corkboard3b-ao.png", false);
		
		albedo5 = TextureLoading::LoadTexture("res/images/tex/cratered-rock-albedo.png", false);
		normal5 = TextureLoading::LoadTexture("res/images/tex/cratered-rock-normal.png", false);
		metallic5 = TextureLoading::LoadTexture("res/images/tex/cratered-rock-metalness.png", false);
		roughness5 = TextureLoading::LoadTexture("res/images/tex/cratered-rock-roughness.png", false);
		ao5 = TextureLoading::LoadTexture("res/images/tex/cratered-rock-ao.png", false);
		
		albedo6 = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-albedo3.png", false);
		normal6 = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-normal2.png", false);
		metallic6 = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-metalness.png", false);
		roughness6 = TextureLoading::LoadTexture("res/images/tex/granitesmooth1-roughness3.png", false);
		ao6 = TextureLoading::LoadTexture("res/images/ao.png", false);
		
		albedo7 = TextureLoading::LoadTexture("res/images/tex/synth-rubber-albedo.png", false);
		normal7 = TextureLoading::LoadTexture("res/images/tex/synth-rubber-normal.png", false);
		metallic7 = TextureLoading::LoadTexture("res/images/tex/synth-rubber-metalness.png", false);
		roughness7 = TextureLoading::LoadTexture("res/images/tex/synth-rubber-roughness.png", false);
		ao7 = TextureLoading::LoadTexture("res/images/ao.png", false);
		
		albedo8 = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-albedo.png", false);
		normal8 = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-normal.png", false);
		metallic8 = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-metalness.png", false);
		roughness8 = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2-roughness.png", false);
		ao8 = TextureLoading::LoadTexture("res/images/tex/Pocked-stone2_Ambient_Occlusion.png", false);
		
		albedo9 = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-albedo.png", false);
		normal9 = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-norrmal.png", false);
		metallic9 = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-metalness.png", false);
		roughness9 = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-roughness.png", false);
		ao9 = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-ao.png", false);
		
		albedo10 = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-albedo.png", false);
		normal10 = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-normal.png", false);
		metallic10 = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-metal.png", false);
		roughness10 = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-roughness.png", false);
		ao10 = TextureLoading::LoadTexture("res/images/tex/bamboo-wood-semigloss-Unreal-Engine/bamboo-wood-semigloss-ao.png", false);
		
		albedo11 = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_basecolor.png", false);
		normal11 = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_normal.png", false);
		metallic11 = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_metallic.png", false);
		roughness11 = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_roughness.png", false);
		ao11 = TextureLoading::LoadTexture("res/images/tex/blocksrough-Unreal-Engine/blocksrough_ambientocclusion.png", false);
		
		albedo12 = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Base_Color.png", false);
		normal12 = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Normal.png", false);
		metallic12 = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Metallic.png", false);
		roughness12 = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Roughness.png", false);
		ao12 = TextureLoading::LoadTexture("res/images/tex/cavefloor2-Unreal-Engine/cavefloor2_Ambient_Occlusion.png", false);
		
		albedo13 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-alb.png", false);
		normal13 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-normal.png", false);
		metallic13 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-metal.png", false);
		roughness13 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-rough.png", false);
		ao13 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		
		albedo14 = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_basecolor-boosted.png", false);
		normal14 = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_normal.png", false);
		metallic14 = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_metallic.png", false);
		roughness14 = TextureLoading::LoadTexture("res/images/tex/Copper-scuffed_Unreal-Engine/Copper-scuffed_roughness.png", false);
		ao14 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		
		albedo15 = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-albedo.png", false);
		normal15 = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-normal.png", false);
		metallic15 = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-metal.png", false);
		roughness15 = TextureLoading::LoadTexture("res/images/tex/greasy-metal-pan1-Unreal-Engine/greasy-metal-pan1-roughness.png", false);
		ao15 = TextureLoading::LoadTexture("res/images/tex/copper-rock1-Unreal-Engine/copper-rock1-ao.png", false);
		
		albedo16 = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_basecolor.png", false);
		normal16 = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_normal.png", false);
		metallic16 = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_Height.png", false);
		roughness16 = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_roughness.png", false);
		ao16 = TextureLoading::LoadTexture("res/images/tex/mahogfloor-Unreal-Engine/mahogfloor_AO.png", false);
		
		albedo17 = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-albedo3.png", false);
		normal17 = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-normal.png", false);
		metallic17 = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-metalic.png", false);
		roughness17 = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-roughness2.png", false);
		ao17 = TextureLoading::LoadTexture("res/images/tex/old-textured-fabric-Unreal-Engine/old-textured-fabric-ao.png", false);
		
		albedo18 = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-albedo.png", false);
		normal18 = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-preview.png", false);
		metallic18 = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-metal.png", false);
		roughness18 = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-roughness.png", false);
		ao18 = TextureLoading::LoadTexture("res/images/tex/mossy-ground1-Unreal-Engine/mossy-ground1-ao.png", false);
		
		albedo19 = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-albedo.png", false);
		normal19 = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-normal.png", false);
		metallic19 = TextureLoading::LoadTexture("res/images/tex/wornpaintedcement-metalness.png", false);
		roughness19 = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-roughness.png", false);
		ao19 = TextureLoading::LoadTexture("res/images/tex/pockedconcrete1-Unreal-Engine/pockedconcrete1-ao.png", false);
		
		albedo20 = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_basecolor.png", false);
		normal20 = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_normal.png", false);
		metallic20 = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_metallic.png", false);
		roughness20 = TextureLoading::LoadTexture("res/images/tex/rustediron-streaks2-Unreal-Engine/rustediron-streaks_roughness.png", false);
		ao20 = TextureLoading::LoadTexture("res/images/ao.png", false);
		
}*/