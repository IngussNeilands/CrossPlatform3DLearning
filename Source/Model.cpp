#include "Model.h"
#include <fstream>
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
#include "GameException.h"

using namespace std;
using namespace boost;
namespace AvoidTheBug3D
{
int getTokens(string input, char sep, string* tokens){
	int curPos = 0;
	int count = 0;

	int length = input.length();

	for (int idx = 0; idx < length; ++idx) {
		if (input[idx] == sep) {
			++count;
		}
	}
	++count;

	for (int idx = 0; idx < count; ++idx) {
		if (idx == count - 1) {
			tokens[idx] = input.substr(curPos);
		}
		else {

		size_t foundPos = input.find(sep, curPos);
		tokens[idx] = input.substr(curPos, foundPos - curPos);
		curPos = foundPos + 1;
		}
	}
	return count;
}


Model::Model() {

}

void Model::init(const string &filename,
             const boost::shared_ptr<Configuration> cfg,
             const boost::shared_ptr<GameLog> log)
{
    this->cfg = cfg;
    this->log = log;

    vertices = new vector<float *>();
    vertices->clear();
    facesVertexIndexes = new vector<int *>();
    facesVertexIndexes->clear();
    normals = new vector<float *>();
    normals->clear();
    facesNormalIndexes = new vector<int *>();
    facesNormalIndexes->clear();
    textureCoords = new vector<float *>();
    textureCoords->clear();
    textureCoordsIndexes = new vector<int *>();
    textureCoordsIndexes->clear();
    vertexData = NULL;
    vertexDataSize = 0;
    vertexDataComponentCount = 0;
    indexData = NULL;
    indexDataSize = 0;
    indexDataIndexCount = 0;
    normalsData = NULL;
    normalsDataSize = 0;
    normalsDataComponentCount = 0;
    textureCoordsData = NULL;
    textureCoordsDataSize = 0;
    textureCoordsDataComponentCount = 0;
    loadFromFile(filename);
}

Model::~Model(void)
{
    this->deleteInitialBuffers();

    if (vertexData != NULL)
    {
        delete[] vertexData;
    }

    if (indexData != NULL)
    {
        delete[] indexData;
    }

    if (normalsData != NULL)
    {
        delete[] normalsData;
    }

    if (textureCoordsData != NULL)
    {
        delete[] textureCoordsData;
    }
}

void Model::loadFromFile(string fileLocation)
{
    if (vertices->size() != 0)
    {
        throw GameException(
            "Illegal attempt to reload model. Please use another object.");
    }
    ifstream file((cfg->getHomeDirectory() + fileLocation).c_str());
    string line;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            if (line[0] == 'v' || line[0] == 'f')
            {
                string *tokens = new string[4]; // Max 4 such tokens in Wavefront files
				
				int numTokens = getTokens(line, ' ', tokens);
				
				int idx = 0;

                if (line[0] == 'v' && line[1] == 'n')
                {
                    // get vertex normal
                    float *vn = new float[3];
                    //BOOST_FOREACH (const string& t, tokens)
                    for (int tokenIdx = 0; tokenIdx < numTokens; ++tokenIdx)
                    {
						string t = tokens[tokenIdx];
                        if (idx > 0)   // The first token is the vertex normal indicator
                        {
                            vn[idx - 1] = (float) atof(t.c_str());
                        }
                        ++idx;
                    }
                    normals->push_back(vn);
                }
                else if (line[0] == 'v' && line[1] == 't')
                {
                    float *vt = new float[2];
                    //BOOST_FOREACH (const string& t, tokens)
					for (int tokenIdx = 0; tokenIdx < numTokens; ++tokenIdx)
					{
						string t = tokens[tokenIdx];
                        if (idx > 0)   // The first token is the vertex texture coordinate indicator
                        {
                            vt[idx - 1] = (float) atof(t.c_str());
                        }
                        ++idx;
                    }

                    vt[1] = 1.0f - vt[1]; // OpenGL's y direction for textures is the opposite of that
                    // of Blender's, so an inversion is needed
                    textureCoords->push_back(vt);
                }
                else if (line[0] == 'v')
                {
                    // get vertex
                    float *v = new float[3];
                    //BOOST_FOREACH (const string& t, tokens)
					for (int tokenIdx = 0; tokenIdx < numTokens; ++tokenIdx)
					{
						string t = tokens[tokenIdx];
                        if (idx > 0)   // The first token is the vertex indicator
                        {
                            v[idx - 1] = (float) atof(t.c_str());
                        }
                        ++idx;
                    }
                    vertices->push_back(v);
                }
                else
                {
                    // get vertex index
                    int *v = new int[3];
                    int *n = NULL;
                    int *textC = NULL;
                    //BOOST_FOREACH (const string& t, tokens)
					for (int tokenIdx = 0; tokenIdx < numTokens; ++tokenIdx)
					{
						string t = tokens[tokenIdx];

                        if (idx > 0)   // The first token is face indicator
                        {
                            if (t.find("//") != string::npos)   // normal index contained in the string
                            {
                                // and texture coordinate index is missing
                                if (n == NULL)
                                    n = new int[3];

                                v[idx - 1] = atoi(
                                                 t.substr(0, t.find("//")).c_str());
                                n[idx - 1] = atoi(
                                                 t.substr(t.find("//") + 2).c_str());
                            }
                            else if (t.find("/") != string::npos
                                     && t.find("//") == string::npos)   // normal and texture coordinate index are
                            {
                                // contained in the string
                                if (n == NULL)
                                    n = new int[3];
                                if (textC == NULL)
                                    textC = new int[3];
								
								string *components = new string[3]; // Max 3 such components in Wavefront files
								int numComponents = getTokens(t, '/', components);

                                int componentIdx = 0;

                                //BOOST_FOREACH(const string &component, components)
								for (int compIdx = 0; compIdx < numComponents; ++compIdx)
								{
									string component = components[compIdx];
                                    switch (componentIdx)
                                    {
                                    case 0:
                                        v[idx - 1] = atoi(component.c_str());
                                        break;
                                    case 1:
                                        textC[idx - 1] = atoi(
                                                             component.c_str());
                                        break;
                                    case 2:
                                        n[idx - 1] = atoi(component.c_str());
                                        break;
                                    }
                                    ++componentIdx;
                                }

								if (components != NULL)
								{
									delete[] components;
								}
                            }

                            else   // just the vertex index is contained in the string
                            {
                                v[idx - 1] = atoi(t.c_str());
                            }
                        }
                        ++idx;
                    }
                    facesVertexIndexes->push_back(v);
                    if (n != NULL)
                        facesNormalIndexes->push_back(n);
                    if (textC != NULL)
                        textureCoordsIndexes->push_back(textC);
                }

				if (tokens != NULL)
				{
					delete[] tokens;
				}
            }
        }
        file.close();

        if (textureCoords->size() > 0)
        {
            this->correctDataVectors();
        }

        // Generate the data and delete the initial buffers
        this->getVertexData();
        this->getIndexData();
        this->getNormalsData();
        this->getTextureCoordsData();
        this->deleteInitialBuffers();

    }
    else
        throw GameException(
            "Could not open file " + cfg->getHomeDirectory()
            + fileLocation);

}

float* Model::getVertexData()
{
    if (vertexData == NULL)
    {
        // 4 components per vertex
        vertexDataComponentCount = vertices->size() * 4;
        vertexDataSize = vertexDataComponentCount * sizeof(float);

        vertexData = new float[vertexDataComponentCount];

        int idx = 0;

        BOOST_FOREACH(const float* vertex, *vertices)
        {
            for (int coordIdx = 0; coordIdx != 3; ++coordIdx)
            {
                vertexData[idx] = vertex[coordIdx];
                ++idx;
            }
            vertexData[idx] = 1.0f;
            ++idx;
        }
    }

    return vertexData;
}

unsigned int * Model::getIndexData()
{
    if (indexData == NULL)
    {
        // 3 indices per face
        int numIndexes = facesVertexIndexes->size() * 3;
        indexDataSize = numIndexes * sizeof(int);

        indexData = new unsigned int[numIndexes];

        indexDataIndexCount = 0;

        BOOST_FOREACH(const int* face, *facesVertexIndexes)
        {

            for (int indexIdx = 0; indexIdx != 3; ++indexIdx)
            {
                indexData[indexDataIndexCount] = face[indexIdx] - 1; // -1 because Wavefront indexes
                // are not 0 based
                ++indexDataIndexCount;
            }

        }
    }
    return indexData;
}


int Model::getVertexDataComponentCount() const
{
    return vertexDataComponentCount;
}

int Model::getVertexDataSize() const
{
    return vertexDataSize;
}

int Model::getIndexDataSize() const
{
    return indexDataSize;
}

int Model::getIndexDataIndexCount() const
{
    return indexDataIndexCount;
}

float* Model::getNormalsData()
{

// Create an array of normal components which corresponds
// by index to the array of vertex components

    if (normalsData == NULL)
    {

        if (vertexDataComponentCount == 0)
        {
            throw GameException(
                "There are no vertices or vertex data has not yet been created.");
        }

        // 3 components per vertex (a single index for vertices, normals and texture coordinates
        // is passed to OpenGL, so normals data will be aligned to vertex data according to the
        // vertex index)
        normalsDataComponentCount = 3 * vertices->size();
        normalsDataSize = normalsDataComponentCount * sizeof(float);

        normalsData = new float[normalsDataComponentCount];

        int faceVertexArrayIndex = 0;

        BOOST_FOREACH(const int* faceVertexIndex, *facesVertexIndexes)
        {
            for (int vertexIndex = 0; vertexIndex != 3; ++vertexIndex)
            {

                for (int normalsDataComponent = 0; normalsDataComponent != 3;
                        ++normalsDataComponent)
                {
                    normalsData[3 * (faceVertexIndex[vertexIndex] - 1)
                                + normalsDataComponent] =
                                    normals->at(
                                        facesNormalIndexes->at(faceVertexArrayIndex)[vertexIndex]
                                        - 1)[normalsDataComponent];
                }
            }
            ++faceVertexArrayIndex;
        }
    }
    return normalsData;
}

int Model::getNormalsDataSize() const
{
    return normalsDataSize;
}

int Model::getNormalsDataComponentCount() const
{
    return normalsDataComponentCount;
}

float* Model::getTextureCoordsData()
{
// Create an array of texture coordinates components which corresponds
// by index to the array of vertex components
    if (textureCoordsData == NULL && textureCoords->size() > 0)
    {

        if (vertexDataComponentCount == 0)
        {
            throw GameException(
                "There are no vertices or vertex data has not yet been created.");
        }

        // 2 components per vertex (a single index for vertices, normals and texture coordinates
        // is passed to OpenGL, so texture coordinates data will be aligned to vertex data according
        // to the vertex index)
        textureCoordsDataComponentCount =  2 * vertices->size();
        textureCoordsDataSize = textureCoordsDataComponentCount * sizeof(float);

        textureCoordsData = new float[textureCoordsDataComponentCount];

        int faceVertexArrayIndex = 0;

        BOOST_FOREACH(const int* faceVertexIndex, *facesVertexIndexes)
        {
            for (int vertexIndex = 0; vertexIndex != 3; ++vertexIndex)
            {

                for (int textureCoordsComponent = 0;
                        textureCoordsComponent != 2; ++textureCoordsComponent)
                {
                    textureCoordsData[2 * (faceVertexIndex[vertexIndex] - 1)
                                      + textureCoordsComponent] =
                                          textureCoords->at(
                                              textureCoordsIndexes->at(faceVertexArrayIndex)[vertexIndex]
                                              - 1)[textureCoordsComponent];
                }
            }
            ++faceVertexArrayIndex;
        }
    }
    return textureCoordsData;
}

int Model::getTextureCoordsDataSize() const
{
    return textureCoordsDataSize;
}

int Model::getTextureCoordsDataComponentCount() const
{
    return textureCoordsDataComponentCount;
}


void Model::correctDataVectors()
{

    boost::scoped_ptr<boost::unordered_map<int, int> > vertexUVPairs(new boost::unordered_map<int, int>());

    int numIndexes = facesVertexIndexes->size();

    for (int idx = 0; idx < numIndexes; ++idx)
    {

        int *faceVertexIndex = facesVertexIndexes->at(idx);

        for (int vertexIndex = 0; vertexIndex != 3; ++vertexIndex)
        {

            unordered_map<int, int>::iterator vertexUVPair = vertexUVPairs->find(faceVertexIndex[vertexIndex]);
            if(vertexUVPair != vertexUVPairs->end())
            {
                if(vertexUVPair->second != textureCoordsIndexes->at(idx)[vertexIndex])
                {
                    // duplicate corresponding vertex data entry and point the vertex index to the new tuple
                    float *v = new float[3];
                    // -1 because at this stage the indexes are still as exported from Blender, meaning 1-based
                    // and not 0-based
                    memcpy(v, vertices->at(faceVertexIndex[vertexIndex] - 1), 3*sizeof(float));
                    vertices->push_back(v);

                    faceVertexIndex[vertexIndex] = vertices->size();

                    vertexUVPairs->insert(make_pair(faceVertexIndex[vertexIndex], textureCoordsIndexes->at(idx)[vertexIndex]));
                }
                // So we don't add a pair if the exact same pair already exists. We do if it does not (see below) or if
                // the vertex index number exists in a pair with a different texture coordinates index number (see above)
            }
            else
            {
                vertexUVPairs->insert(make_pair(faceVertexIndex[vertexIndex], textureCoordsIndexes->at(idx)[vertexIndex]));
            }


        }
    }

}

void Model::deleteInitialBuffers()
{
    if (vertices != NULL)
    {
        for (int i = 0; i != vertices->size(); ++i)
        {
            delete[] vertices->at(i);
        }
        vertices->clear();
        delete vertices;
        vertices = NULL;
    }

    if (facesVertexIndexes != NULL)
    {
        for (int i = 0; i != facesVertexIndexes->size(); ++i)
        {
            delete[] facesVertexIndexes->at(i);
        }
        facesVertexIndexes->clear();
        delete facesVertexIndexes;
        facesVertexIndexes = NULL;
    }

    if (normals != NULL)
    {
        for (int i = 0; i != normals->size(); ++i)
        {
            delete[] normals->at(i);
        }
        normals->clear();
        delete normals;
        normals = NULL;
    }

    if (facesNormalIndexes != NULL)
    {
        for (int i = 0; i != facesNormalIndexes->size(); ++i)
        {
            delete[] facesNormalIndexes->at(i);
        }
        facesNormalIndexes->clear();
        delete facesNormalIndexes;
        facesNormalIndexes = NULL;
    }

    if (textureCoords != NULL)
    {
        for (int i = 0; i != textureCoords->size(); ++i)
        {
            delete[] textureCoords->at(i);
        }
        textureCoords->clear();
        delete textureCoords;
        textureCoords = NULL;
    }

    if (textureCoordsIndexes != NULL)
    {
        for (int i = 0; i != textureCoordsIndexes->size(); ++i)
        {
            delete[] textureCoordsIndexes->at(i);
        }
        textureCoordsIndexes->clear();
        delete textureCoordsIndexes;
        textureCoordsIndexes = NULL;
    }

}

}

