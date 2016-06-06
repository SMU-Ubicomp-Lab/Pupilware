//
// Created by Chatchai Wangwiwattana on 6/6/16.
//

#include "MDStarbustNeo.hpp"

namespace pw{

    MDStarbustNeo::MDStarbustNeo(){
        threshold = 0;
    }

    MDStarbustNeo::MDStarbustNeo( const MDStarbustNeo& other){}
    MDStarbustNeo::~MDStarbustNeo(){}

    float MDStarbustNeo::getCost(int step) const{
        return ((primer/precision) * (MAX_WALKING_STEPS - step));
    }
}