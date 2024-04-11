#include "core.h"

namespace v1 {

	class NyxNGV1: public nyxcore::Agent {

		public:

			string name() override { return "NyxNGV1"; }

			void iterate() override {
				int actionCount = getActionCount();
				double minimumDistance = nyxcore::distance(getDirective(), getState()); // initial distance
				double distance;
				int minimumDistanceIndex = 0;
				for (int i = 0; i < actionCount; i++) {
					distance = nyxcore::distance(getDirective(), delta(getState(), getAction(i)));
					if (distance < minimumDistance) {
						minimumDistance = distance;
						minimumDistanceIndex = i;
					}
				}
				takeAction(getAction(minimumDistanceIndex));
			}

	};

};