#include "../core/hash.h"
#include "openssl/md4.h"
#include "openssl/md5.h"

class md {

    class MessageDigest4: public hashing::HashFunction {

        public:

            data::Bytes hash(data::Bytes plaintext) override {
                unsigned char ibuf[plaintext.getLength()];
                for (int i = 0; i < plaintext.getLength(); i++) {
                    ibuf[i] = plaintext.get(i);
                }
                unsigned char obuf[32];
                MD4(ibuf, plaintext.getLength(), obuf);
                data::Bytes output(32);
                for (int i = 0; i < 32; i++) {
                    output.set(i, obuf[i]);
                }
                return output;
            }

    };

    class MessageDigest5: public hashing::HashFunction {

        public:

            data::Bytes hash(data::Bytes plaintext) override {
                unsigned char ibuf[plaintext.getLength()];
                for (int i = 0; i < plaintext.getLength(); i++) {
                    ibuf[i] = plaintext.get(i);
                }
                unsigned char obuf[32];
                MD5(ibuf, plaintext.getLength(), obuf);
                data::Bytes output(32);
                for (int i = 0; i < 32; i++) {
                    output.set(i, obuf[i]);
                }
                return output;
            }

    };

};