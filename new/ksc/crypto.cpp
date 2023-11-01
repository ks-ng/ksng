#pragma once

// Encryption - securing data

class Encryption {

	public:

		template <const int len>
		class Key {

			private:

				unsigned char keyData[len];

			public:

				const int length = len;

				Key(unsigned char data[len]) {
					for (int i = 0; i < length; i++) {
						keyData[i] = data[i];
					}
				}

		}

		template 
		virtual int* encrypt(unsigned char plaintext[]

}
