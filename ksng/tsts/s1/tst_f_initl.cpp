#include <iostream>

using namespace std;

int s(initializer_list<int> v) {
    int r = 0;
    for (const auto& x : v) {
        r += x;
    }
    return r;
}

int main() {
    cout << s({1, 2, 3, 4, 5});
}