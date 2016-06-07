//
//  mdStarbust.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef DummyAlgo_hpp
#define DummyAlgo_hpp

#include "PWAlgorithm.hpp"

namespace pw {

    class DummyAlgo : public PWAlgorithm {
    
    public:
        DummyAlgo();
        virtual ~DummyAlgo();
        
        virtual void init();
        virtual PWPupilSize process( const PupilMeta &pupilMeta );
        virtual void exit();
        
    private:
        int th;

        std::shared_ptr<CVWindow> thWin;
        
    };
}

#endif /* mdStarbust_hpp */
