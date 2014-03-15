#include "Model.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "GameException.h"

using namespace std;
using namespace boost;
namespace AvoidTheBug3D {

Model::Model(string filename, const boost::shared_ptr<Configuration> &cfg,
		const boost::shared_ptr<GameLog> &log) {
	this->cfg = cfg;
	this->log = log;

	vertices = new vector<float *>();
	vertices->clear();
	faces = new vector<int *>();
	faces->clear();
	vertexData = NULL;
	vertexDataComponentCount = 0;
	multiColour = false;
	loadFromFile(filename);
}

Model::~Model(void) {
	if (vertices != NULL) {
		for (int i = 0; i != vertices->size(); ++i) {
			delete[] vertices->at(i);
		}
		vertices->clear();
		delete vertices;
	}

	if (faces != NULL) {
		for (int i = 0; i != faces->size(); ++i) {
			delete[] faces->at(i);
		}
		faces->clear();
		delete faces;
	}

	if (vertexData != NULL) {
		delete[] vertexData;
	}
}

void Model::loadFromFile(string fileLocation) {
	if (vertices->size() != 0) {
		throw new GameException(
				"Illegal attempt to reload model. Please use another object.");
	}
	ifstream file((cfg->getHomeDirectory() + fileLocation).c_str());
	string line;
	if (file.is_open()) {
		while (getline(file, line)) {
			if (line[0] == 'v' || line[0] == 'f') {
				char_separator<char> sep(" ");
				tokenizer<char_separator<char> > tokens(line, sep);
				int idx = 0;

				if (line[0] == 'v') {
					float *v = new float[3];
					BOOST_FOREACH (const string& t, tokens) {
						if (idx > 0) { // The first token is the vertice indicator
							v[idx - 1] = atof(t.c_str());
						}
						++idx;
					}
					vertices->push_back(v);
				} else {
					int *v = new int[3];
					BOOST_FOREACH (const string& t, tokens) {
						if (idx > 0) { // The first token is face indicator
							v[idx - 1] = atoi(t.c_str());
						}
						++idx;
					}
					faces->push_back(v);
				}
			}
		}
		file.close();
	} else
		throw GameException(
				"Could not open file " + cfg->getHomeDirectory()
						+ fileLocation);

}

void Model::outputVertices() {
	int vectorCount = vertices->size();
	for (int idx = 0; idx != vectorCount; ++idx) {
		cout << "Vertex:" << vertices->at(idx)[0] << " - "
				<< vertices->at(idx)[1] << " - " << vertices->at(idx)[2]
				<< endl;
	}
}

int Model::getNumVertices() {
	return (int) this->vertices->size();
}

int Model::getNumFaces() {
	return (int) this->faces->size();
}

vector<float*>* Model::getVertices() {
	return vertices;
}

void Model::outputFaces() {
	int vectorCount = faces->size();
	for (int idx = 0; idx != vectorCount; ++idx) {
		cout << "Face:" << faces->at(idx)[0] << " - " << faces->at(idx)[1]
				<< " - " << faces->at(idx)[2] << endl;
	}
}

vector<int*>* Model::getFaces() {
	return faces;
}

float* Model::getVertexData() {
	if (vertexData == NULL) {
		int numFaces = faces->size();
		int numVertexComponents = numFaces * 12; // faces * num vertices per face * 4 (3 components + 1)
		vertexData = new float[numVertexComponents * (multiColour ? 2 : 1)]; // components multiplied
																			 // by 2 if random colours
																			 // will be added
		vertexDataComponentCount = 0;
		int colourIndex = 0;
		// Face
		for (int faceIdx = 0; faceIdx != numFaces; ++faceIdx) {
			// Vertex
			for (int verticeIdx = 0; verticeIdx != 3; ++verticeIdx) {
				// Coordinate
				for (int coordIdx = 0; coordIdx != 3; ++coordIdx) {

					vertexData[vertexDataComponentCount] = vertices->at(
							faces->at(faceIdx)[verticeIdx] - 1)[coordIdx];
					++vertexDataComponentCount;

					// w component
					if (coordIdx == 2) {
						vertexData[vertexDataComponentCount] = 1.0f;
						++vertexDataComponentCount;

						if (multiColour) {
							// Add random colours for each face to the data.

							// Colour rotation
							if (verticeIdx == 0) {
								if (colourIndex == 3) {
									colourIndex = 0;
								} else {
									++colourIndex;
								}
							}

							float colour[] = {0.0f, 0.0f, 0.0f, 0.0f};

							switch (colourIndex) {

								// Red
								case 0:
								colour[0] = 1.0f;
								break;

								// Yellow
								case 1:
									colour[0] = 1.0f;
									colour[1] = 1.0f;
								break;

								// Blue
								case 2:
									colour[2] = 1.0f;
								break;

								// White
								case 3:
									colour[0] = 1.0f;
									colour[1] = 1.0f;
									colour[2] = 1.0f;
								break;

								// Green (should not appear)
								default:
									colour[1] = 1.0f;
								break;
							}
							memcpy(&vertexData[vertexDataComponentCount - 1 + numVertexComponents], &colour, 4);
						} // if multiColour
					} // w component
				} // Coordinate
			} // Vertex
		} // Face
	} // if vertexData == NULL

	return vertexData;
}

void Model::outputVertexData() {
	cout << endl << "Number of faces: " << faces->size() << endl;
	cout << "Total number of components: " << vertexDataComponentCount << endl;
	for (int cnt = 0; cnt != vertexDataComponentCount; ++cnt) {
		if (cnt % 4 == 0)
			cout << endl;
		cout << vertexData[cnt] << " ";
	}
}

int Model::getVertexDataComponentCount() {
	return vertexDataComponentCount;
}

void Model::setMultiColour(bool multiColour) {
	this->multiColour = multiColour;
}

}

