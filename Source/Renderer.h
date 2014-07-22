/*
 * Renderer.h
 *
 *  Created on: Jan 15, 2014
 *      Author: Dimitri Kourkoulis
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#ifndef SDLANDOPENGL
#define SDLANDOPENGL
//#define GLEW_STATIC
#define NO_SDL_GLEXT
#include "GL/glew.h" // It seems that, when using glew,
// we do not need to include gl.h,
// glext.h or glu.h (if we do include
// them, they need to be included after
// glew.
#include "SDL_opengl.h"
#include "SDL.h"
#endif //SDLANDOPENGL

#include <boost/smart_ptr.hpp>
#include "WorldObject.h"
#include <vector>
#include "Configuration.h"
#include "GameLog.h"
#include <boost/smart_ptr.hpp>

namespace AvoidTheBug3D {

//typedef unsigned int (*GL_CreateShader_Func)(unsigned int);

class Renderer {

protected:

	boost::shared_ptr<Configuration> cfg;
	boost::shared_ptr<GameLog> log;
	GLuint program;

	/**
	 * Load a shader's source code from a file into a string
	 * @param fileLocation The file's location, relative to the game path
	 * @return String containing the shader's source code
	 */
	string loadShaderFromFile(string fileLocation);

	/**
	 * Compile a shader's source code
	 * @param shaderSource String containing the shader's source code
	 * @param shaderType Type of shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER - the latter for OpenGL 3.3)
	 * @return OpenGL shader reference
	 */
	GLuint compileShader(string shaderSource, GLenum shaderType);

public:

	/**
	 * Constructor
	 * @param cfg The game's configuration object
	 * @param log The game's logging object
	 */
	Renderer(boost::shared_ptr<Configuration> cfg,
			boost::shared_ptr<GameLog> log);

	/**
	 * Initialise renderer (OpenGL, GLEW, etc)
	 * @param width The width of the window
	 * @param height The height of the window
	 */
	virtual void Init(int width, int height);

	/**
	 * Draw the scene
	 */
	virtual void DrawScene(
			boost::shared_ptr<vector<boost::shared_ptr<WorldObject> > > scene) = 0;

	/**
	 * Destructor
	 */
	virtual ~Renderer();

};

} /* namespace AvoidTheBug3D */

#endif /* RENDERER_H_ */
