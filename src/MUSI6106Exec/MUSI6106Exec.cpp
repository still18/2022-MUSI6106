
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    //Paths to be read from input line
    std::string sInputFilePath,                 //!< file paths
                sOutputFilePath;

    //Standard variables
    static const int kBlockSize = 1024;
    clock_t time = 0;

    //Audio data pointers
    float **ppfAudioInputData =  0;
    float **ppfAudioOutputData = 0;

    //Files
    CAudioFileIf *pInputFile =  0;
    CAudioFileIf *pOutputFile = 0;
    
    //Type vars
    CAudioFileIf::FileSpec_t stFileSpec;
    CCombFilterIf::CombFilterType_t combType;
    
    //Comb value vars
    float delay;
    float maxDelay = 1;
    float gain;
    CCombFilterIf *pCombFilter;
    
    showClInfo();
    
    //float                           *pAudioData;
    
    
    
    ////////////////////////////////////////////////////////////////
    //Parse command line arguments
    if (argc < 5)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        
        //Filter type
        if (strcmp(argv[2], "fir")) {
            combType = CCombFilterIf::kCombFIR;
        } else if (strcmp(argv[2], "iir")) {
            combType = CCombFilterIf::kCombIIR;
        } else {
            cout << "INCORRECT FILTER TYPE: please enter \"fir\" or \"iir\"";
        }
        
        //Delay length in seconds
        delay = atof(argv[3]);
        
        //Gain
        gain = atof(argv[4]);
        
    }
    
    
    
    
    
    //////////////////////////////////////////////
    //Open files and filters


    //Open input file
    CAudioFileIf::create(pInputFile);
    pInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!pInputFile->isOpen())
    {
        cout << "Wave file open error!";
        CAudioFileIf::destroy(pInputFile);
        return -1;
    }
    pInputFile->getFileSpec(stFileSpec);
    
    //Open/create the "combed" wav output
    CAudioFileIf::create(pOutputFile);
    pOutputFile->openFile((sInputFilePath + "_comb.wav"), CAudioFileIf::kFileWrite, &stFileSpec);
    if (!pOutputFile->isOpen())
    {
        cout << "Wav file open error!";
        CAudioFileIf::destroy(pOutputFile);
        return -1;
    }
    
    //Format filter
    CCombFilterIf::create(pCombFilter);
    pCombFilter->init(combType, maxDelay, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    
    CCombFilterIf::FilterParam_t delayParam = CCombFilterIf::kParamDelay;
    pCombFilter->setParam(delayParam, delay);
    
    CCombFilterIf::FilterParam_t gainParam = CCombFilterIf::kParamGain;
    pCombFilter->setParam(gainParam, gain);
    
    
    
    
    
    /////////////////////////////////////////////////
    //Allocate memory
    
    
    //Allocate input memory
    ppfAudioInputData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
    ppfAudioInputData[i] = new float[kBlockSize];

    if (ppfAudioInputData == 0)
    {
        CAudioFileIf::destroy(pInputFile);
        return -1;
    }
    if (ppfAudioInputData[0] == 0)
    {
        CAudioFileIf::destroy(pInputFile);
        return -1;
    }
    
    //Allocate output memory
    ppfAudioOutputData = new float*[stFileSpec.iNumChannels];
    for (int o = 0; o < stFileSpec.iNumChannels; o++) {
        ppfAudioOutputData[o] = new float[kBlockSize];
    }
    if (ppfAudioOutputData == 0) {
        CAudioFileIf::destroy(pOutputFile);
        return -1;
    }
    if (ppfAudioOutputData[0] == 0) {
        CAudioFileIf::destroy(pOutputFile);
        return -1;
    }

    time = clock();
    
    
    
    //////////////////////////////////////////////////////////////////////////////
    //Read data, process with comb, and write to output
    while (!pInputFile->isEof())
    {
        // set block length variable
        long long iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        pInputFile->readData(ppfAudioInputData, iNumFrames);
        
        //process filtering
        pCombFilter->process(ppfAudioInputData, ppfAudioOutputData, iNumFrames);
        
        //write to output
        pOutputFile->writeData(ppfAudioOutputData, iNumFrames);
        
    }
    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;
    
    
    

    //////////////////////////////////////////////////////////////////////////////
    //Clean-up (close files and free memory)
    
    
    //Free input data
    CAudioFileIf::destroy(pInputFile);
    
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioInputData[i];
    delete[] ppfAudioInputData;
    ppfAudioInputData = 0;
    
    
    //Free output data (and comb)
    CAudioFileIf::destroy(pOutputFile);
    
    for (int b = 0; b < stFileSpec.iNumChannels; b++) {
        delete[] ppfAudioOutputData[b];
    }
    delete[] ppfAudioOutputData;
    ppfAudioOutputData = 0;
    
    CCombFilterIf::destroy(pCombFilter);
    

    // all done
    return 0;

}


void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

