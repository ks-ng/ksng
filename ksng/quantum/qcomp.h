#pragma once
#include "qcore.h"

namespace qcomp {

    sda::SecureDataArray<qcore::QuantumVector> extractQuantumData(qcore::QuantumVector vec) {
        // Extract the quantum data from the given vector

        // 1. check if the vector is valid
        int i = 1;
        while (i < vec.getLength()) { i *= 2; }
        if (i != vec.getLength()) {
            stringstream ss;
            ss << "cannot extract qubinary quantum data from vector (size is " << vec.getLength() << ", not a power of 2)";
            notif::fatal(ss.str());
            return sda::SDA<qcore::QuantumVector>(0);
        }

        // 2. set up the extracted qubits
        sda::SDA<qcore::QV> results(i);
        for (int j = 0; j < i; j++) { results.set(j, qcore::QV(2)); }

        // 3. extract quantum data
        

        return results;
    }

};