//
// Created by Chatchai Wangwiwattana on 6/6/16.
//

#ifndef PUPILWARE_MDSTARBUSTNEO_HPP
#define PUPILWARE_MDSTARBUSTNEO_HPP

#include "MDStarbust.hpp"

namespace pw{

class MDStarbustNeo: public MDStarbust {

public:
    MDStarbustNeo( const std::string& name );
    MDStarbustNeo( const MDStarbustNeo& other);
    virtual ~MDStarbustNeo();

    virtual void init() override ;

protected:
    virtual float getCost(int step) const override ;

private:
    int sigma;

};


}


#endif //PUPILWARE_MDSTARBUSTNEO_HPP
