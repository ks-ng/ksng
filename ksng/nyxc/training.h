#pragma once
#include "core.h"

namespace training {

	namespace move2d {

		class MovementActuator: public nyxcore::Actuator {

			public:

				nyxcore::Entity delta(nyxcore::Entity state, nyxcore::Entity action) override {
					return nyxcore::Entity({state.get(0) + action.get(0), state.get(1) + action.get(1)});
				}

		};


		MovementActuator movementActuator;
		sda::SDA<nyxcore::Entity> movementActions({nyxcore::Entity({1, 0}), nyxcore::Entity({0, 1}), nyxcore::Entity({-1, 0}), nyxcore::Entity({0, -1})});
		nyxcore::Entity initialState({0, 0});
		nyxcore::Entity directive1({10, 20});
		nyxcore::Entity directive2({-10, 0});
		nyxcore::Entity directive3({30, -10});

	};

};