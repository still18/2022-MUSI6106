
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilter.h"
#include "CombFilterIf.h"


static const char*  kCMyProjectBuildDate = __DATE__;


CCombFilterIf::CCombFilterIf () :
    m_bIsInitialized(false),//return endprocess if not inited in function calls
    m_pCCombFilter(0),
    m_fSampleRate(0)
{
    // this should never hurt
    this->reset ();
}


CCombFilterIf::~CCombFilterIf ()
{
    this->reset ();
}

const int  CCombFilterIf::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CCombFilterIf::getBuildDate ()
{
    return kCMyProjectBuildDate;
}

Error_t CCombFilterIf::create (CCombFilterIf*& pCCombFilter)
{
    //allocate memory for pcComc
    pCCombFilter = new CCombFilterIf();
    if (!pCCombFilter)
    {
        return Error_t::kMemError;
    }

    return Error_t::kNoError;
}

Error_t CCombFilterIf::destroy (CCombFilterIf*& pCCombFilter)
{
    delete pCCombFilter;
    pCCombFilter = 0;
    return Error_t::kNoError;
}

Error_t CCombFilterIf::init (CombFilterType_t eFilterType, float fMaxDelayLengthInS, float fSampleRateInHz, int iNumChannels)
{
    //depending on filter type, make m_comb.. IIR/FIR
    m_bIsInitialized = true;
    m_fSampleRate = fSampleRateInHz;
    m_tFliterType = eFilterType;
    
    float maxDelayInSamples;
    maxDelayInSamples = fMaxDelayLengthInS * fSampleRateInHz;
    
    switch (eFilterType)
    {
        case (kCombFIR):
            //call FIR constructor
            m_pCCombFilter = new CCombFilterFIR(maxDelayInSamples, iNumChannels);
            break;
        case (kCombIIR):
            //call IIR constructor
            m_pCCombFilter = new CCombFilterIIR(maxDelayInSamples, iNumChannels);
            break;
        default:
            m_pCCombFilter = new CCombFilterFIR(maxDelayInSamples, iNumChannels);
            break;
    }
    
    return Error_t::kNoError;
}

Error_t CCombFilterIf::reset ()
{
    init(m_tFliterType, 1.0, m_fSampleRate, 1);
    return Error_t::kNoError;
}

Error_t CCombFilterIf::process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    return m_pCCombFilter->process(ppfInputBuffer, ppfOutputBuffer, iNumberOfFrames);
}

Error_t CCombFilterIf::setParam (FilterParam_t eParam, float fParamValue)
{
    return m_pCCombFilter->setParam(eParam, fParamValue);
}

float CCombFilterIf::getParam (FilterParam_t eParam) const
{
    return m_pCCombFilter->getParam(eParam);
}
