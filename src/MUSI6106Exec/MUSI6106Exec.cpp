
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
    std::string sInputFilePath,                 //!< file paths
                sOutputFilePath;

    static const int kBlockSize = 1024;

    clock_t time = 0;

    float **ppfAudioData = 0;

    CAudioFileIf *phAudioFile = 0;
    std::fstream hOutputFile;
    CAudioFileIf::FileSpec_t stFileSpec;

    showClInfo();
    
    //new vars
    CAudioFileIf                    *pOutputFile;
    CCombFilterIf::CombFilterType_t combType;
    float                           delay;
    float                           maxDelay = 1;
    float                           gain;
    float                           *pAudioData;
    float                           **ppfAudioOutputData;
    CCombFilterIf                   *pCombFilter;
    
    

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 5)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        //sOutputFilePath = sInputFilePath + ".txt";
        
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
    
    
    
    
    

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave file
    CAudioFileIf::create(phAudioFile);
    phAudioFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phAudioFile->isOpen())
    {
        cout << "Wave file open error!";
        CAudioFileIf::destroy(phAudioFile);
        return -1;
    }
    phAudioFile->getFileSpec(stFileSpec);
    
    
    
    
    
    

    //////////////////////////////////////////////////////////////////////////////
    // open the output text file
//    hOutputFile.open(sOutputFilePath.c_str(), std::ios::out);
//    if (!hOutputFile.is_open())
//    {
//        cout << "Text file open error!";
//        CAudioFileIf::destroy(phAudioFile);
//        return -1;
//    }
    
    
    //Format filter and setup output wav file
    CCombFilterIf::create(pCombFilter);
    pCombFilter->init(combType, maxDelay, stFileSpec.fSampleRateInHz, stFileSpec.iNumChannels);
    
    CCombFilterIf::FilterParam_t delayParam = CCombFilterIf::kParamDelay;
    pCombFilter->setParam(delayParam, delay);
    
    CCombFilterIf::FilterParam_t gainParam = CCombFilterIf::kParamGain;
    pCombFilter->setParam(gainParam, gain);
    
    
    //Create the "combed" wav output
    CAudioFileIf::create(pOutputFile);
    pOutputFile->openFile((sInputFilePath + "_comb.wav"), CAudioFileIf::kFileWrite, &stFileSpec);
    if (!pOutputFile->isOpen())
    {
        cout << "Wave file open error!";
        CAudioFileIf::destroy(pOutputFile);
        return -1;
    }
    
    
    

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfAudioData = new float*[stFileSpec.iNumChannels];
    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        ppfAudioData[i] = new float[kBlockSize];

    if (ppfAudioData == 0)
    {
        CAudioFileIf::destroy(phAudioFile);
        hOutputFile.close();
        return -1;
    }
    if (ppfAudioData[0] == 0)
    {
        CAudioFileIf::destroy(phAudioFile);
        hOutputFile.close();
        return -1;
    }
    
    //Allocate output data memory
    ppfAudioOutputData = new float*[stFileSpec.iNumChannels];
    for (int b = 0; b < stFileSpec.iNumChannels; b++) {
        ppfAudioOutputData[b] = new float[kBlockSize];
    }

    time = clock();
    
    
    
    
    

    //////////////////////////////////////////////////////////////////////////////
    // get audio data and write it to the output text file (one column per channel)
    while (!phAudioFile->isEof())
    {
        // set block length variable
        long long iNumFrames = kBlockSize;

        // read data (iNumOfFrames might be updated!)
        phAudioFile->readData(ppfAudioData, iNumFrames);

//        cout << "\r" << "reading and writing";
//
//        // write
//        for (int i = 0; i < iNumFrames; i++)
//        {
//            for (int c = 0; c < stFileSpec.iNumChannels; c++)
//            {
//                hOutputFile << ppfAudioData[c][i] << "\t";
//            }
//            hOutputFile << endl;
//        }
        
        //process filtering
        pCombFilter->process(ppfAudioData, ppfAudioOutputData, iNumFrames);
        
        pOutputFile->writeData(ppfAudioOutputData, iNumFrames);
        
    }
    cout << "\nreading/writing done in: \t" << (clock() - time) * 1.F / CLOCKS_PER_SEC << " seconds." << endl;
    
    
    
    
    

    //////////////////////////////////////////////////////////////////////////////
    // clean-up (close files and free memory)
    CAudioFileIf::destroy(phAudioFile);
    //hOutputFile.close();

    for (int i = 0; i < stFileSpec.iNumChannels; i++)
        delete[] ppfAudioData[i];
    delete[] ppfAudioData;
    ppfAudioData = 0;
    
    
    //cleanup output/filter
    CAudioFileIf::destroy(pOutputFile);
    CCombFilterIf::destroy(pCombFilter);
    for (int b = 0; b < stFileSpec.iNumChannels; b++) {
        delete[] ppfAudioOutputData[b];
    }
    delete[] ppfAudioOutputData;
    ppfAudioOutputData = 0;
    

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

