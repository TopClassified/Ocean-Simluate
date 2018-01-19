
#include<vector>

#include<GL/SOIL.h>

#include<GL/Shader.h>

#include<glm/glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

#include<GL/glew.h>
#include<GLFW/glfw3.h>

class SkyBox
{
public:
	SkyBox(GLuint skyboxTexture);
	~SkyBox();
	void Render(glm::mat4 View, glm::mat4 Projection, glm::mat4 Reflection, int Reflect);
	GLuint GetSkyText()
	{
		return skyboxTexture;
	}
private:
	std::vector<const GLchar*> faces;
	Shader *SkyRender;
	GLuint skyboxVAO, skyboxVBO;
	GLuint skyboxTexture;
};

