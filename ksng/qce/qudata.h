#include "qcore.h"

namespace qudata {

	enum DataState { Separated, Combined };

	class Qubits {

		private:

			sda::Array<qcore::QuantumState> qubits;
			qcore::QuantumState combinedState;
			DataState dataState;

		public:

			

	};

};