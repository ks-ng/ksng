#include <string>
#include "../util/dstruct/sda.h"

using namespace std;

namespace nyxngc_super {

	class Entity {

		private:

			int entitySize;
			sda::SDA<double> values;
			sda::SDA<string> names;

		public:

			explicit Entity(int size): values(sda::SDA<double>(size)), names(sda::SDA<string>(size)) {}

			bool has(string )

	};

};