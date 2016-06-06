//
// Created by Chatchai Wangwiwattana on 6/6/16.
//

#ifndef PUPILWARE_MDSTARBUSTNEO_HPP
#define PUPILWARE_MDSTARBUSTNEO_HPP

#include "MDStarbust.hpp"

namespace pw{

class MDStarbustNeo: public MDStarbust {

public:
    MDStarbustNeo();
    MDStarbustNeo( const MDStarbustNeo& other);
    virtual ~MDStarbustNeo();

protected:
    virtual float getCost(int step) const;

};


}


#endif //PUPILWARE_MDSTARBUSTNEO_HPP
