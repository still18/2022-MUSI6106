#if !defined(__CombFilter_hdr__)
#define __CombFilter_hdr__

#include "ErrorDef.h"
#include "CombFilterIf.h"


/*! \brief base class for the comb filter (FIR & IIR)
*/
class CCombFilterBase
{
public:
    
    //include essentials, don't need others

    CCombFilterBase (int fMaxDelayLengthInFrames, int iNumChannels);
    virtual ~CCombFilterBase ();
    
    /*! resets the internal variables (requires new call of init)
    \return Error_t
    */
    Error_t reset ();

    /*! sets a comb filter parameter
    \param eParam what parameter (see ::FilterParam_t)
    \param fParamValue value of the parameter
    \return Error_t
    */
    Error_t setParam (CCombFilterIf::FilterParam_t eParam, float fParamValue);//declare from IF
    
    /*! return the value of the specified parameter
    \param eParam
    \return float
    */
    float   getParam (CCombFilterIf::FilterParam_t eParam) const;
    
    /*! processes one block of audio
    \param ppfInputBuffer input buffer [numChannels][iNumberOfFrames]
    \param ppfOutputBuffer output buffer [numChannels][iNumberOfFrames]
    \param iNumberOfFrames buffer length (per channel)
    \return Error_t
    */
    virtual Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames) = 0;

protected:
    //ringbuffer, num channels, etc

private:
    CCombFilterBase (const CCombFilterBase& that);
    //virtual ~CCombFilterBase();
};


//Sub-classes

class CCombFilterFIR : public CCombFilterBase
{
    //construct, deconstruct, process
    //define in cpp
    CCombFilterFIR (int fMaxDelayLengthInFrames, int iNumChannels);
    ~CCombFilterFIR();
    
    Error_t process (float **ppfInputBuffer, float ** ppfOutputBuffer, int iNumberOfFrames);
};

class CCombFilterIIR : public CCombFilterBase
{
    //construct, deconstruct, process
    //define in cpp
    CCombFilterIIR (int fMaxDelayLengthInFrames, int iNumChannels);
    ~CCombFilterIIR();
    
    Error_t process (float **ppfInputBuffer, float ** ppfOutputBuffer, int iNumberOfFrames);
};

#endif // #if !defined(__CombFilterIf_hdr__)



