/*
* PlayerView.h
*
*  Created on: Dec 26, 2013
*      Author: Dimitri Kourkoulis
*/

#ifndef PLAYERVIEW_H_
#define PLAYERVIEW_H_

#include <boost/smart_ptr.hpp>
#include "Configuration.h"
#include "GameLog.h"
#include "WorldObject.h"
#include "Renderer.h"

namespace AvoidTheBug3D {

	class PlayerView {

	private:
		boost::shared_ptr<Configuration> cfg;
		boost::shared_ptr<GameLog> log;

		boost::shared_ptr<WorldObject> goat;
		boost::shared_ptr<vector<boost::shared_ptr<WorldObject> > > scene;
		boost::shared_ptr<Renderer> renderer;

		float rotation;

	public:

		/**
		* Constructor
		* @param cfg The game's configuration object
		* @param log The game's logging object
		*/
		PlayerView(const boost::shared_ptr<Configuration> cfg,
			const boost::shared_ptr<GameLog> log);

		/**
		* Destructor
		*/
		~PlayerView();

		/**
		* Render the view
		*/
		void render();
	};

} /* namespace AvoidTheBug3D */

#endif /* PLAYERVIEW_H_ */
