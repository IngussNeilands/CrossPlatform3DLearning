/*
 * RendererOpenGL33.cpp
 *
 *  Created on: Jan 19, 2014
 *      Author: Dimitri Kourkoulis
 */

#include "RendererOpenGL33.h"
#include <fstream>


namespace AvoidTheBug3D {

RendererOpenGL33::RendererOpenGL33( boost::shared_ptr<Configuration> cfg,
		 boost::shared_ptr<GameLog> log) : Renderer(cfg, log) {
	vertexShader = loadShaderFromFile("/Game/Data/Shaders/testShader.vert");
	fragmentShader = loadShaderFromFile("/Game/Data/Shaders/testShader.frag");

}

void RendererOpenGL33::Init(int width, int height) {
}

void RendererOpenGL33::DrawScene(boost::shared_ptr<vector<WorldObject> > scene) {
}

RendererOpenGL33::~RendererOpenGL33() {
	// TODO Auto-generated destructor stub
}

string& RendererOpenGL33::loadShaderFromFile(string fileLocation) {
//	ifstream file((cfg->getHomeDirectory() + fileLocation).c_str());
//		string line;
//		if (file.is_open()) {
//			while (getline(file, line)) {

	string empty = "";
	return empty;

}

} /* namespace AvoidTheBug3D */

