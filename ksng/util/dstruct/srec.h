#include "../notif.h"
#include <array>

namespace srec {

	template <typename... Types>
	class SecureRecord {

		private:

			std::tuple<Types...> data_;
			bool locked;

		public:

			SecureRecord(Types... args) : data_{args...} {}

			void reveal() { locked = false; }
			void hide() { locked = true; }
			void securityCheck() { 
				if (locked) { 
					notif::security("invalid attempt to access to secure record", ALERT);
				} 
			}

			template <std::size_t Index>
			auto get() const {
				securityCheck();
				return std::get<Index>(data_);
			}
		
	};

	template <typename... Types>
	using SR = SecureRecord<Types...>;

};