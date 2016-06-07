//
// Brief: This class is stored Pupilware result for the entire videos.
//        It is intended to budle with PWAlgorithm
//
// Created by Chatchai Wangwiwattana on 6/7/16.
//

#ifndef PUPILWARE_PWDATAMODEL_HPP
#define PUPILWARE_PWDATAMODEL_HPP

#include "PWTypes.hpp"
#include <vector>

namespace pw{

    class PWDataModel {

    public:

        void                        addPupilSize( const PWPupilSize& ps );
        void                        setPupilSizeAt( size_t index, const PWPupilSize& ps );

        PWPupilSize                 getPupilSizeAt( size_t index ) const;

        const std::vector <float>&  getLeftPupilSizes() const;
        const std::vector <float>&  getRightPupilSizes() const;

        void                        resize( size_t newSize );


    private:

        const unsigned int          MAXIMUM_BUFFER_SIZE = 16384; // 2^14

        std::vector<float>          leftPupilSizes;
        std::vector<float>          rightPupilSizes;
    };

}

#endif //PUPILWARE_PWDATAMODEL_HPP
