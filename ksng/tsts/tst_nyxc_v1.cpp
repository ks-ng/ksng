#include "../nyxc/v1.h"

class MovementActuator: public nyxcore::Actuator {

	public:

		nyxcore::Entity delta(nyxcore::Entity state, nyxcore::Entity action) override {
			return nyxcore::Entity({state.get(0) + action.get(0), state.get(1) + action.get(1)});
		}

};


MovementActuator movementActuator;
sda::SDA<nyxcore::Entity> movementActions({nyxcore::Entity({1, 0}), nyxcore::Entity({0, 1}), nyxcore::Entity({-1, 0}), nyxcore::Entity({0, -1})});
nyxcore::Entity initialState({0, 0});
nyxcore::Entity directive({100, 200});

int main() {
	v1::NyxNGV1 agent;
	agent.install(movementActuator.installable());
	agent.instruct(movementActions);
	agent.brief(initialState);
	agent.direct(directive);
	cout << "initial: " << agent.status() << endl;
	for (int i = 0; i < 300; i++) {
		agent.iterate();
	}
	cout << "final: " << agent.status() << endl;
	cout << endl;
}