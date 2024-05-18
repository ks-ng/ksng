#include "../nyxc/v2.h"
#include "../nyxc/training.h"

int tmax = 10000;

int main() {
	v2::NyxNGV2 agent(tmax, false, 256);
	agent.install(training::move2d::movementActuator.installable());
	agent.instruct(training::move2d::movementActions);
	agent.brief(training::move2d::initialState);
	agent.direct(training::move2d::directive1);
	cout << "initial: " << agent.status() << endl;
	cout << "training ..." << endl;
	for (int i = 0; i < tmax; i++) {
		agent.iterate();
	}
	agent.brief(training::move2d::initialState);
	cout << "training complete, engaging ..." << endl;
	agent.engage();
	while (agent.directiveDistance() > 2) {
		agent.iterate();
		cout << "  " << agent.status() << "\r";
		flush(cout);
	}
	cout << endl << "done." << endl;
	cout << "final: " << agent.status() << endl;
}