/*
 * modelTest.cpp
 *
 *  Created on: Dec 31, 2013
 *      Author: Dimitrios Kourkoulis
 */
#include <boost/test/unit_test.hpp>
#include "globals.h"
#include "../Model.h"
#include "../GameLog.h"
#include "../Configuration.h"
#include <boost/smart_ptr.hpp>

using namespace AvoidTheBug3D;
BOOST_AUTO_TEST_SUITE(ModelTestSuite)
BOOST_AUTO_TEST_CASE( model_test ) {

	boost::shared_ptr<GameLog> log = globals->log;

	boost::shared_ptr<Configuration> cfg = globals->cfg;

	boost::scoped_ptr<Model> model(new Model(
		"/Game/Data/UnspecifiedAnimal/UnspecifiedAnimalWithTexture.obj",
		false, true, cfg, log));

	BOOST_CHECK_EQUAL(model->getNumVertices() > 0, true);
	BOOST_CHECK_EQUAL(model->getNumFaces() > 0, true);

	model->getVertexData();
	model->getIndexData();
	model->getNormalsData();
	model->getTextureCoordsData();

	BOOST_CHECK_NE(0, model->getVertexDataComponentCount());
	BOOST_CHECK_NE(0, model->getIndexDataIndexCount());
	BOOST_CHECK_NE(0, model->getNormalsDataComponentCount());
	BOOST_CHECK_NE(0, model->getTextureCoordsDataComponentCount());

	cout << "Vertex data component count: "
			<< model->getVertexDataComponentCount() << endl << "Index count: "
			<< model->getIndexDataIndexCount() << endl
			<< "Normals data component count: "
			<< model->getNormalsDataComponentCount() << endl
			<< "Texture coordinates count: "
			<< model->getTextureCoordsDataComponentCount() << endl;

	model->outputVertexData();
	model->outputTextureCoordsData();
}
BOOST_AUTO_TEST_SUITE_END()
