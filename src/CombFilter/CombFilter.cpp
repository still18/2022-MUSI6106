
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilter.h"
#include "CombFilterIf.h"



CCombFilterBase::CCombFilterBase(int fMaxDelayLengthInFrames, int iNumChannels)
{
}


Error_t CCombFilterBase::reset ()
{
    return Error_t::kNoError;
}

Error_t CCombFilterBase::setParam (CCombFilterIf::FilterParam_t eParam, float fParamValue)
{
    return Error_t::kNoError;
}

float CCombFilterBase::getParam (CCombFilterIf::FilterParam_t eParam) const
{
//    float iPara = 0;
//    switch (eParam)
//    {
//        case CCombFilterIf::kParamGain:
//            iPara =  1;
//            break;
//        case CCombFilterIf::kParamDelay:
//            iPara = 2;
//            break;
//        case CCombFilterIf::kNumFilterParams:
//            iPara = -1;
//            break;
//    }
//
//
//    return iPara;
    return 0;
}

