
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "Util.h"

#include "CombFilter.h"
#include "CombFilterIf.h"


CCombFilterBase::CCombFilterBase()
{
    m_fGain = 1.0;
    m_iDelayInSamples = 0;
    m_iNumChannels = 1;
    m_iMaxDelayInSamples = 1;
    m_ppRingBuff = 0;
}

CCombFilterBase::CCombFilterBase(int fMaxDelayLengthInSamples, int iNumChannels)
{
    m_ppRingBuff = new CRingBuffer<float>*[iNumChannels];
    for (int i = 0; i < iNumChannels; i++) {
        m_ppRingBuff[i] = new CRingBuffer<float>(fMaxDelayLengthInSamples);
    }
    
    m_iMaxDelayInSamples = fMaxDelayLengthInSamples;
    m_iNumChannels = iNumChannels;
    
    m_fGain = 1.0;
    m_iMaxDelayInSamples = 0;
}

CCombFilterBase::~CCombFilterBase ()
{
    for (int i = 0; i < m_iNumChannels; i++) {
        delete[] m_ppRingBuff[i];
    }
    delete[] m_ppRingBuff;
    m_ppRingBuff = 0;
}

Error_t CCombFilterBase::setParam (CCombFilterIf::FilterParam_t eParam, float fParamValue)
{
    switch (eParam)
    {
        case CCombFilterIf::kParamGain:
            m_fGain = fParamValue;
            break;
        case CCombFilterIf::kParamDelay:
            m_iDelayInSamples = fParamValue;
            break;
        case CCombFilterIf::kNumFilterParams:
            return Error_t::kFunctionInvalidArgsError;
            break;
    }
    return Error_t::kNoError;
}

float CCombFilterBase::getParam (CCombFilterIf::FilterParam_t eParam) const
{
    float retVal = 0.0;
    switch (eParam)
    {
        case CCombFilterIf::kParamGain:
            retVal = m_fGain;
            break;
        case CCombFilterIf::kParamDelay:
            retVal = m_iDelayInSamples;
            break;
        case CCombFilterIf::kNumFilterParams:
            retVal = 0;//? throw error?
            //return Error_t::kFunctionInvalidArgsError;
            break;
    }
    return retVal;
}

//IIR Process
Error_t CCombFilterIIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    if (m_fGain < -1) {
        m_fGain = -1;
    } else if (m_fGain > 1) {
        m_fGain = 1;
    }
    for (int a = 0; a < m_iNumChannels; a++) {
        for (int b = 0; b < iNumberOfFrames; b++) {
            ppfOutputBuffer[a][b] = ppfInputBuffer[a][b] + m_fGain * (m_ppRingBuff[a]->getPostInc());
            //use output retrival for IIR
            m_ppRingBuff[a]->putPostInc(ppfOutputBuffer[a][b]);
        }
    }
    return Error_t::kNoError;
}

//FIR Process
Error_t CCombFilterFIR::process(float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames)
{
    for (int a = 0; a < m_iNumChannels; a++) {
        for (int b = 0; b < iNumberOfFrames; b++) {
            ppfOutputBuffer[a][b] = ppfInputBuffer[a][b] + m_fGain * (m_ppRingBuff[a]->getPostInc());
            //use input retrival for FIR
            m_ppRingBuff[a]->putPostInc(ppfInputBuffer[a][b]);
        }
    }
    return Error_t::kNoError;
}

CCombFilterFIR::CCombFilterFIR() : CCombFilterBase(){}

CCombFilterIIR::CCombFilterIIR() : CCombFilterBase(){}

CCombFilterFIR::CCombFilterFIR(int fMaxDelayLengthInSamples, int iNumChannels) : CCombFilterBase(fMaxDelayLengthInSamples, iNumChannels)
{}

CCombFilterIIR::CCombFilterIIR(int fMaxDelayLengthInSamples, int iNumChannels) : CCombFilterBase(fMaxDelayLengthInSamples, iNumChannels)
{}

//got vtable errors if I didn't include these
CCombFilterIIR::~CCombFilterIIR()
{
    for (int i = 0; i < m_iNumChannels; i++) {
        delete[] m_ppRingBuff[i];
    }
    delete[] m_ppRingBuff;
    m_ppRingBuff = 0;
}

CCombFilterFIR::~CCombFilterFIR()
{
    for (int i = 0; i < m_iNumChannels; i++) {
        delete[] m_ppRingBuff[i];
    }
    delete[] m_ppRingBuff;
    m_ppRingBuff = 0;
}
