#include "core.h"

namespace v1 {

	class NyxNGV1: public core::Agent {

		public:

			void iterate() override {
				int actionCount = getActionCount();
				double minimumDistance = core::distance(getDirective(), getState()); // initial distance
				double distance;
				int minimumDistanceIndex = 0;
				for (int i = 0; i < actionCount; i++) {
					distance = core::distance(getDirective(), delta(getState(), getAction(i)))
					if (distance < minimumDistance) {
						minimumDistance = distance;
						minimumDistanceIndex = i;
					}
				}
				takeAction(getAction(minimumDistanceIndex));
			}

	};

};