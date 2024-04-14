#include "../nyxc/v2.h"
#include "../nyxc/training.h"

int tmax = 5000;

int main() {
	v2::NyxNGV2 agent;
	int i = 0;
	bool completed;
	while (true) {
		cout << "  loading new agent ..." << "\r";
		flush(cout);
		i = 0;
		agent = v2::NyxNGV2(tmax, false, 256);
		agent.install(training::move2d::movementActuator.installable());
		agent.instruct(training::move2d::movementActions);
		agent.brief(training::move2d::initialState);
		agent.direct(training::move2d::directive1);
		cout << "  training new agent ... " << "\r";
		for (int i = 0; i < tmax; i++) {
			agent.iterate();
		}
		agent.brief(training::move2d::initialState);
		agent.engage();
		completed = false;
		for (int j = 0; j < 50; j++) {
			agent.iterate();
			i++;
			cout << "  " << agent.status() << "\r";
			flush(cout);
			if (agent.directiveDistance() <= 2) { completed = true; break; }
		}
		if (completed) {
			cout << "NyxNGV2 completed objective in " << i;
		} else {
			cout << "NyxNGV2 could not complete objective in 100";
		}
		cout << " steps                                                                                                              " << endl;
	}
}