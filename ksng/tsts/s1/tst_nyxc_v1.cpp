#include "../nyxc/v1.h"
#include "../nyxc/training.h"

int main() {
	v1::NyxNGV1 agent;
	agent.install(training::move2d::movementActuator.installable());
	agent.instruct(training::move2d::movementActions);
	agent.brief(training::move2d::initialState);
	agent.direct(training::move2d::directive1);
	cout << "initial: " << agent.status() << endl;
	for (int i = 0; i < 50; i++) {
		agent.iterate();
	}
	cout << "final: " << agent.status() << endl;
	cout << endl;
}