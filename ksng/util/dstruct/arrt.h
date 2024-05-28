#pragma once

namespace arrt {

    template <typename T>
    class ArrayType {

        public:

            virtual T& get(int index) = 0;
            virtual void set(int index, T value) = 0;
            virtual int getLength() = 0;

            T& operator[](int index) { return get(index); }

    };

}