#include "ksng/util/data.h"
int main(){cout<<(data::Bytes({0,1,2,3})+data::Bytes({4,5,6,7})).hex()<<endl;}