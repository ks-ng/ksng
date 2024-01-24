#include <cmath>
#include <gmp.h> // for really really big ints
#include "xln.h"

using namespace std;
using namespace xln;

namespace prime {

	class PrimalityTest {

		public:

			virtual void test(XLN n) = 0;
			bool isProbablePrime

	};

}