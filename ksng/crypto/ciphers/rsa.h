#pragma once
#include "../core/cipher.h"
#include "../../util/xln.h" // FIX THIS HEADER !

namespace rsa {

    int DEFAULT_KEY_SIZE = 3072;

    class RivestShamirAdleman: public cipher::AsymmetricCipher {

        public:
        
            cipher::Keypair generateKeys() override {
                xln::XLN p, q, n, tot, e, d; 
                p = xln::getPrime(DEFAULT_KEY_SIZE);
                q = xln::getPrime(DEFAULT_KEY_SIZE);
                n = p * q;
                tot = xln::getLCM(p - 1, q - 1);
                e = xln::XLN(65537)
                d = xln::getModularInverse(e, tot)
                data::Bytestream pu, pr;
                pu << e.toBytes() << n.toBytes();
                pr << d.toBytes();
                return Keypair(
                    key::Key(pu.bytes()),
                    key::Key(pr.bytes())
                );
            }
        
    };

    using RSA = RivestShamirAdleman;

}: