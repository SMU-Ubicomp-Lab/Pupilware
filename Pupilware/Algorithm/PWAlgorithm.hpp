//
//  AlgorithmBase.hpp
//  Pupilware
//
//  Created by Chatchai Wangwiwattana on 5/25/16.
//  Copyright © 2016 Chatchai Wangwiwattana. All rights reserved.
//

#ifndef AlgorithmBase_hpp
#define AlgorithmBase_hpp

#include <opencv2/opencv.hpp>
#include "PupilMeta.hpp"

#include "../etc/CWUIHelper.hpp"
#include "../etc/CWCVUtility.hpp"

#include "PWTypes.hpp"


namespace pw{

    class PWAlgorithm {
        
    public:
        PWAlgorithm();

        virtual ~PWAlgorithm();

        virtual void init();
        
        virtual PWPupilSize process( const PupilMeta &pupilMeta ) =0;
        
        virtual void exit();
        
    };
}


#endif /* AlgorithmBase_hpp */
