#include "../../util/data.h"
#include "../core/hash.h"
#include "openssl/sha.h"

namespace sha {

    class SHA_224: public hashing::HashFunction {

        public:

            data::Bytes hash(data::Bytes plaintext) override {
                unsigned char ibuf[plaintext.getLength()];
                for (int i = 0; i < plaintext.getLength(); i++) {
                    ibuf[i] = plaintext.get(i);
                }
                unsigned char obuf[32];
                SHA224(ibuf, plaintext.getLength(), obuf);
                data::Bytes output(32);
                for (int i = 0; i < 32; i++) {
                    output.set(i, obuf[i]);
                }
                return output;
            }

    };

    class SHA_256: public hashing::HashFunction {

        public:

            data::Bytes hash(data::Bytes plaintext) override {
                unsigned char ibuf[plaintext.getLength()];
                for (int i = 0; i < plaintext.getLength(); i++) {
                    ibuf[i] = plaintext.get(i);
                }
                unsigned char obuf[32];
                SHA256(ibuf, plaintext.getLength(), obuf);
                data::Bytes output(32);
                for (int i = 0; i < 32; i++) {
                    output.set(i, obuf[i]);
                }
                return output;
            }

    };

    class SHA_384: public hashing::HashFunction {

        public:

            data::Bytes hash(data::Bytes plaintext) override {
                unsigned char ibuf[plaintext.getLength()];
                for (int i = 0; i < plaintext.getLength(); i++) {
                    ibuf[i] = plaintext.get(i);
                }
                unsigned char obuf[32];
                SHA384(ibuf, plaintext.getLength(), obuf);
                data::Bytes output(32);
                for (int i = 0; i < 32; i++) {
                    output.set(i, obuf[i]);
                }
                return output;
            }

    };

    class SHA_512: public hashing::HashFunction {

        public:

            data::Bytes hash(data::Bytes plaintext) override {
                unsigned char ibuf[plaintext.getLength()];
                for (int i = 0; i < plaintext.getLength(); i++) {
                    ibuf[i] = plaintext.get(i);
                }
                unsigned char obuf[32];
                SHA512(ibuf, plaintext.getLength(), obuf);
                data::Bytes output(32);
                for (int i = 0; i < 32; i++) {
                    output.set(i, obuf[i]);
                }
                return output;
            }

    };

};