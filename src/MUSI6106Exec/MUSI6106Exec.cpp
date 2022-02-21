
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "CombFilterIf.h"

using std::cout;
using std::endl;

//Declarations for local function (tests and filter method)
void    showClInfo ();
int filter(std::string sInputFilePath,
           std::string sOutputFilePath,
           CCombFilterIf::CombFilterType_t combType,
           clock_t time,
           int kBlockSize,
           float gain,
           float delay,
           float maxDelay);
void test1(std::string inputFileName, std::string pathToDir, clock_t time);
void test2(std::string inputFileName, std::string pathToDir, clock_t time);
void test3(std::string inputFileName, std::string pathToDir, clock_t time,
           CCombFilterIf::CombFilterType_t combType);
void test4(std::string inputFileName, std::string pathToDir, clock_t time,
           CCombFilterIf::CombFilterType_t combType);
void test5(std::string inputFileName, std::string pathToDir, clock_t time);

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    //Paths to be read from input line
    std::string sInputFilePath,                 //!< file paths
                sOutputFilePath;

    //Standard variables
    clock_t time = 0;
    static const int kBlockSize = 1024;
    
    //Type vars
    CCombFilterIf::CombFilterType_t combType;
    
    //Comb value vars
    float gain;
    float delay;
    float maxDelay = 1;
    
    //Adjust local/global path to the files below:
    std::string const PATH_TO_DIR = "../../";
    
    showClInfo();
    
    
    
    ////////////////////////////////////////////////////////////////
    //Parse command line arguments
    if (argc == 1) {
        //RUN TESTS
        cout << "\n-RUNNING TESTS-" << endl;
        
        //Test 1
        std::string test1Input = "pureSine440.wav";
        test1(test1Input, PATH_TO_DIR, time);
        
        //Test 2
        std::string test2Input = "pureSine220.wav";
        test2(test2Input, PATH_TO_DIR, time);
        
        //Test 3
        std::string test3Input = "pureSine440.wav";
        test3(test3Input, PATH_TO_DIR, time, CCombFilterIf::kCombFIR);
        test3(test3Input, PATH_TO_DIR, time, CCombFilterIf::kCombIIR);
        
        //Test 4
        std::string test4Input = "supposedSilence.wav";
        test4(test4Input, PATH_TO_DIR, time, CCombFilterIf::kCombFIR);
        test4(test4Input, PATH_TO_DIR, time, CCombFilterIf::kCombIIR);
        
        //Test 5 (custom: checks to make sure FIR and IIR are not same)
        std::string test5Input = "pureSine440.wav";
        test5(test5Input, PATH_TO_DIR, time);
        
        cout << "\n-TESTS CONCLUDED-" << endl;
        //tests finished, return
        return 0;
    } else if (argc < 5) {
        cout << "Missing args!" << endl;;
        return -1;
    } else {
        //Input path (output path takes this as well)
        sInputFilePath = PATH_TO_DIR + argv[1];
        std::string s = argv[1];
        int indexOfDot = s.find('.');
        std::string nonExtent = s.substr(0, indexOfDot);
        sOutputFilePath = PATH_TO_DIR + nonExtent + "_comb.wav";
        
        //Filter type
        if (strcmp(argv[2], "fir") == 0) {
            //FIR
            cout << "FIR comb filter selected" << endl;
            combType = CCombFilterIf::kCombFIR;
        } else if (strcmp(argv[2], "iir") == 0) {
            //IIR
            cout << "IIR comb filter selected" << endl;
            combType = CCombFilterIf::kCombIIR;
        } else {
            cout << "INCORRECT FILTER TYPE: please enter \"fir\" or \"iir\" (or \"TEST\" for test functions)";
            return -1;
        }
        
        //Delay length in seconds
        delay = atof(argv[3]);
        
        //Gain
        gain = atof(argv[4]);
        
    }
    
    //Run new filter method (returns int so if filter fails main fails)
    return filter(sInputFilePath,
                  sOutputFilePath,
                  combType,
                  time,
                  kBlockSize,
                  gain,
                  delay,
                  maxDelay);
    
}


//Seperated processing of data so that tests can be done easier
int filter(std::string sInputFilePath,
           std::string sOutputFilePath,
           CCombFilterIf::CombFilterType_t combType,
           clock_t time,
           int kBlockSize,
           float gain,
           float delay,
           float maxDelay)
{
    
    ///////////////////////////////////
    //Pointer variables
    
    //Audio data
    float **ppfAudioInputData =  0;
    float **ppfAudioOutputData = 0;
    
    //Files
    CAudioFileIf *pInputFile =  0;
    CAudioFileIf *pOutputFile = 0;
    
    //Comb
    CCombFilterIf *pCombFilter = 0;
    
    //File spec (not pointer)
    CAudioFileIf::FileSpec_t stFileSpec;
    
    
    
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
    pOutputFile->openFile(sOutputFilePath, CAudioFileIf::kFileWrite, &stFileSpec);
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
    

    // all done, call return 0 will be bounced to main
    return 0;
    
}

void test1(std::string inputFileName, std::string pathToDir, clock_t time)
{
    //Test 1: Run FIR, should pass if 1 single tone with no pitch variation
    
    //Set output path
    std::string sInputFilePath = pathToDir + inputFileName;
    int indexOfDot = inputFileName.find('.');
    std::string nonExtent = inputFileName.substr(0, indexOfDot);
    std::string sOutputFilePath = pathToDir + nonExtent + "_combTEST1.wav";
    
    //Preset vars
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::CombFilterType_t::kCombFIR;
    int kBlockSize = 1024;
    float delay = 1;
    float gain = 1;
    
    //Run filter with set params
    filter(sInputFilePath, sOutputFilePath, combFilterType, time, kBlockSize, gain, delay, 1);
    
    //Reopen the newly created file to read
    CAudioFileIf *pCheckedFile = 0;
    CAudioFileIf::FileSpec_t checkedFileSpec;
    float **ppCheckedAudioData = 0;
    
    CAudioFileIf::create(pCheckedFile);
    pCheckedFile->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    pCheckedFile->getFileSpec(checkedFileSpec);
    
    ppCheckedAudioData = new float*[checkedFileSpec.iNumChannels];
    for (int a = 0; a < checkedFileSpec.iNumChannels; a++) {
        ppCheckedAudioData[a] = new float[kBlockSize];
    }
    
    //Read input and check if to see if everything is 0
    bool ruined = false;
    while (!pCheckedFile->isEof() && !ruined) {
        long long frames = kBlockSize;
        pCheckedFile->readData(ppCheckedAudioData, frames);
        for (int b = 0; b < checkedFileSpec.iNumChannels; b++)
        {
            if (ruined)
            {
                break;
            }
            for (int c = 0; c < frames; c++)
            {
                if (ppCheckedAudioData[b][c] != 0)
                {
                    //Test is failed, can skip the rest
                    ruined = true;
                    cout << "TEST 1 FALIED: DATA NOT ALL ZERO" << endl;
                    break;
                }
            }
        }
    }
    if (!ruined) {
        cout << "TEST 1 PASSED" << endl;
    }
    
    //Close files, free memory
    for (int a = 0; a < checkedFileSpec.iNumChannels; a++)
    {
        delete [] ppCheckedAudioData[a];
    }
    delete [] ppCheckedAudioData;
    ppCheckedAudioData = 0;
    
    CAudioFileIf::destroy(pCheckedFile);
    
}

void test2(std::string inputFileName, std::string pathToDir, clock_t time)
{
    //Test 2: Run IIR, check magnitude with original if no pitch variation
    
    //Set output path
    std::string sInputFilePath = pathToDir + inputFileName;
    int indexOfDot = inputFileName.find('.');
    std::string nonExtent = inputFileName.substr(0, indexOfDot);
    std::string sOutputFilePath = pathToDir + nonExtent + "_combTEST2.wav";
    
    //Preset vars
    CCombFilterIf::CombFilterType_t combFilterType = CCombFilterIf::CombFilterType_t::kCombIIR;
    int kBlockSize = 1024;
    float delay = 1;
    float gain = 1;
    
    //Run filter with set params
    filter(sInputFilePath, sOutputFilePath, combFilterType, time, kBlockSize, gain, delay, 1);
    
    //Reopen the newly created file to read
    CAudioFileIf *pCheckedFile = 0;
    CAudioFileIf::FileSpec_t checkedFileSpec;
    float **ppCheckedAudioData = 0;
    
    CAudioFileIf::create(pCheckedFile);
    pCheckedFile->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    pCheckedFile->getFileSpec(checkedFileSpec);
    
    ppCheckedAudioData = new float*[checkedFileSpec.iNumChannels];
    for (int a = 0; a < checkedFileSpec.iNumChannels; a++) {
        ppCheckedAudioData[a] = new float[kBlockSize];
    }
    
    //Reopen the already read file to check with combed
    CAudioFileIf *pOriginalFile = 0;
    CAudioFileIf::FileSpec_t originalFileSpec;
    float **ppOriginalAudioData = 0;
    
    CAudioFileIf::create(pOriginalFile);
    pOriginalFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    pOriginalFile->getFileSpec(originalFileSpec);
    
    ppOriginalAudioData = new float*[originalFileSpec.iNumChannels];
    for (int a = 0; a < originalFileSpec.iNumChannels; a++) {
        ppOriginalAudioData[a] = new float[kBlockSize];
    }
    
    //Read input of both and find the difference
    bool ruined = false;
    while (!pCheckedFile->isEof() && !ruined) {
        long long frames = kBlockSize;
        pCheckedFile->readData(ppCheckedAudioData, frames);
        pOriginalFile->readData(ppOriginalAudioData, frames);
        for (int b = 0; b < checkedFileSpec.iNumChannels; b++)
        {
            if (ruined)
            {
                break;
            }
            for (int c = 0; c < frames; c++)
            {
                //Check for difference but must include compensation for the gain increase with the added signal the comb has
                if ((ppCheckedAudioData[b][c] - ((1 / 1 - gain) * ppOriginalAudioData[b][c])) != 0)
                {
                    //Test is failed, can skip the rest
                    ruined = true;
                    cout << "TEST 2 FALIED: DATA NOT SAME" << endl;
                    break;
                }
            }
        }
    }
    if (!ruined) {
        cout << "TEST 2 PASSED" << endl;
    }
    
    //Close files, free memory
    for (int a = 0; a < checkedFileSpec.iNumChannels; a++)
    {
        delete [] ppCheckedAudioData[a];
    }
    delete [] ppCheckedAudioData;
    ppCheckedAudioData = 0;
    
    for (int a = 0; a < originalFileSpec.iNumChannels; a++)
    {
        delete [] ppOriginalAudioData[a];
    }
    delete [] ppOriginalAudioData;
    ppOriginalAudioData = 0;
    
    CAudioFileIf::destroy(pCheckedFile);
    CAudioFileIf::destroy(pOriginalFile);
    
}

void test3(std::string inputFileName, std::string pathToDir, clock_t time, CCombFilterIf::CombFilterType_t combType)
{
    //Test 3: Run either type, check results with varying block size
    
    //Set output(s) path
    std::string sInputFilePath = pathToDir + inputFileName;
    int indexOfDot = inputFileName.find('.');
    std::string nonExtent = inputFileName.substr(0, indexOfDot);
    std::string a_sOutputFilePath = pathToDir + nonExtent + "_combTEST3A.wav";
    std::string b_sOutputFilePath = pathToDir + nonExtent + "_combTEST3B.wav";
    
    //Preset vars
    CCombFilterIf::CombFilterType_t combFilterType = combType;
    int a_kBlockSize = 512;
    int b_kBlockSize = 1024;
    float delay = 1;
    float gain = 1;
    
    //Run filter with set params, once per block size
    filter(sInputFilePath, a_sOutputFilePath, combFilterType, time, a_kBlockSize, gain, delay, 1);
    filter(sInputFilePath, b_sOutputFilePath, combFilterType, time, b_kBlockSize, gain, delay, 1);
    
    //Reopen the file for block size A
    CAudioFileIf *a_pCheckedFile = 0;
    CAudioFileIf::FileSpec_t a_checkedFileSpec;
    float **a_ppCheckedAudioData = 0;
    
    CAudioFileIf::create(a_pCheckedFile);
    a_pCheckedFile->openFile(a_sOutputFilePath, CAudioFileIf::kFileRead);
    a_pCheckedFile->getFileSpec(a_checkedFileSpec);
    
    a_ppCheckedAudioData = new float*[a_checkedFileSpec.iNumChannels];
    for (int a = 0; a < a_checkedFileSpec.iNumChannels; a++) {
        a_ppCheckedAudioData[a] = new float[a_kBlockSize];
    }
    
    //Reopen the file for block size B
    CAudioFileIf *b_pCheckedFile = 0;
    CAudioFileIf::FileSpec_t b_checkedFileSpec;
    float **b_ppCheckedAudioData = 0;
    
    CAudioFileIf::create(b_pCheckedFile);
    b_pCheckedFile->openFile(b_sOutputFilePath, CAudioFileIf::kFileRead);
    b_pCheckedFile->getFileSpec(b_checkedFileSpec);
    
    b_ppCheckedAudioData = new float*[b_checkedFileSpec.iNumChannels];
    for (int z = 0; z < b_checkedFileSpec.iNumChannels; z++) {
        b_ppCheckedAudioData[z] = new float[b_kBlockSize];
    }
    
    //Read input of both and find the difference
    bool ruined = false;
    while (!a_pCheckedFile->isEof() && !ruined) {
        long long frames = b_kBlockSize;
        a_pCheckedFile->readData(a_ppCheckedAudioData, frames);
        b_pCheckedFile->readData(b_ppCheckedAudioData, frames);
        for (int x = 0; x < b_checkedFileSpec.iNumChannels; x++)
        {
            if (ruined)
            {
                break;
            }
            for (int y = 0; y < frames; y++)
            {
                //Compare equality among consistent (larger) block size
                if (a_ppCheckedAudioData[x][y] != b_ppCheckedAudioData[x][y])
                {
                    //Test is failed, can skip the rest
                    ruined = true;
                    cout << "TEST 3 FALIED: DATA NOT SAME" << endl;
                    break;
                }
            }
        }
    }
    if (!ruined) {
        cout << "TEST 3 PASSED" << endl;
    }
    
    //Close files, free memory
    for (int z = 0; z < a_checkedFileSpec.iNumChannels; z++)
    {
        delete [] a_ppCheckedAudioData[z];
    }
    delete [] a_ppCheckedAudioData;
    a_ppCheckedAudioData = 0;
    
    for (int z = 0; z < b_checkedFileSpec.iNumChannels; z++)
    {
        delete [] b_ppCheckedAudioData[z];
    }
    delete [] b_ppCheckedAudioData;
    b_ppCheckedAudioData = 0;
    
    CAudioFileIf::destroy(a_pCheckedFile);
    CAudioFileIf::destroy(b_pCheckedFile);
    
}

void test4(std::string inputFileName, std::string pathToDir, clock_t time, CCombFilterIf::CombFilterType_t combType)
{
    //Test 4: Run either, with a completely silent file
    
    //Set output path
    std::string sInputFilePath = pathToDir + inputFileName;
    int indexOfDot = inputFileName.find('.');
    std::string nonExtent = inputFileName.substr(0, indexOfDot);
    std::string sOutputFilePath = pathToDir + nonExtent + "_combTEST4.wav";
    
    //Preset vars
    CCombFilterIf::CombFilterType_t combFilterType = combType;
    int kBlockSize = 1024;
    float delay = 1;
    float gain = 0;
    
    //Run filter with set params
    filter(sInputFilePath, sOutputFilePath, combFilterType, time, kBlockSize, gain, delay, 1);
    
    //Reopen the newly created file to read
    CAudioFileIf *pCheckedFile = 0;
    CAudioFileIf::FileSpec_t checkedFileSpec;
    float **ppCheckedAudioData = 0;
    
    CAudioFileIf::create(pCheckedFile);
    pCheckedFile->openFile(sOutputFilePath, CAudioFileIf::kFileRead);
    pCheckedFile->getFileSpec(checkedFileSpec);
    
    ppCheckedAudioData = new float*[checkedFileSpec.iNumChannels];
    for (int a = 0; a < checkedFileSpec.iNumChannels; a++) {
        ppCheckedAudioData[a] = new float[kBlockSize];
    }
    
    //Read input and check if to see if everything is 0
    bool ruined = false;
    while (!pCheckedFile->isEof() && !ruined) {
        long long frames = kBlockSize;
        pCheckedFile->readData(ppCheckedAudioData, frames);
        for (int b = 0; b < checkedFileSpec.iNumChannels; b++)
        {
            if (ruined)
            {
                break;
            }
            for (int c = 0; c < frames; c++)
            {
                //Check if not 0? seems like first test but this makes sense
                if (ppCheckedAudioData[b][c] != 0)
                {
                    //Test is failed, can skip the rest
                    ruined = true;
                    cout << "TEST 4 FALIED: DATA NOT ALL ZERO" << endl;
                    break;
                }
            }
        }
    }
    if (!ruined) {
        cout << "TEST 4 PASSED" << endl;
    }
    
    //Close files, free memory
    for (int a = 0; a < checkedFileSpec.iNumChannels; a++)
    {
        delete [] ppCheckedAudioData[a];
    }
    delete [] ppCheckedAudioData;
    ppCheckedAudioData = 0;
    
    CAudioFileIf::destroy(pCheckedFile);
    
}

void test5(std::string inputFileName, std::string pathToDir, clock_t time)
{
    //Test 5: Run both types, compare for difference
    
    //Set output(s) path
    std::string sInputFilePath = pathToDir + inputFileName;
    int indexOfDot = inputFileName.find('.');
    std::string nonExtent = inputFileName.substr(0, indexOfDot);
    std::string a_sOutputFilePath = pathToDir + nonExtent + "_combTEST5FIR.wav";
    std::string b_sOutputFilePath = pathToDir + nonExtent + "_combTEST5IIR.wav";
    
    //Preset vars
    int kBlockSize = 1024;
    float delay = 1;
    float gain = 1;
    
    //Run filter with set params, once per type
    filter(sInputFilePath, a_sOutputFilePath, CCombFilterIf::CombFilterType_t::kCombFIR, time, kBlockSize, gain, delay, 1);
    filter(sInputFilePath, b_sOutputFilePath, CCombFilterIf::CombFilterType_t::kCombIIR, time, kBlockSize, gain, delay, 1);
    
    //Reopen the file for FIR
    CAudioFileIf *a_pCheckedFile = 0;
    CAudioFileIf::FileSpec_t a_checkedFileSpec;
    float **a_ppCheckedAudioData = 0;
    
    CAudioFileIf::create(a_pCheckedFile);
    a_pCheckedFile->openFile(a_sOutputFilePath, CAudioFileIf::kFileRead);
    a_pCheckedFile->getFileSpec(a_checkedFileSpec);
    
    a_ppCheckedAudioData = new float*[a_checkedFileSpec.iNumChannels];
    for (int a = 0; a < a_checkedFileSpec.iNumChannels; a++) {
        a_ppCheckedAudioData[a] = new float[kBlockSize];
    }
    
    //Reopen the file for IIR
    CAudioFileIf *b_pCheckedFile = 0;
    CAudioFileIf::FileSpec_t b_checkedFileSpec;
    float **b_ppCheckedAudioData = 0;
    
    CAudioFileIf::create(b_pCheckedFile);
    b_pCheckedFile->openFile(b_sOutputFilePath, CAudioFileIf::kFileRead);
    b_pCheckedFile->getFileSpec(b_checkedFileSpec);
    
    b_ppCheckedAudioData = new float*[b_checkedFileSpec.iNumChannels];
    for (int z = 0; z < b_checkedFileSpec.iNumChannels; z++) {
        b_ppCheckedAudioData[z] = new float[kBlockSize];
    }
    
    //Read input of both and confirm a difference
    bool ruined = true;
    while (!a_pCheckedFile->isEof() && ruined) {
        long long frames = kBlockSize;
        a_pCheckedFile->readData(a_ppCheckedAudioData, frames);
        b_pCheckedFile->readData(b_ppCheckedAudioData, frames);
        for (int x = 0; x < b_checkedFileSpec.iNumChannels; x++)
        {
            if (!ruined) {
                break;
            }
            for (int y = 0; y < frames; y++)
            {
                //Compare equality among consistent (larger) block size
                if (a_ppCheckedAudioData[x][y] != b_ppCheckedAudioData[x][y])
                {
                    //Test is a success, can skip the rest
                    ruined = false;
                    cout << "TEST 5 PASSED" << endl;
                    break;
                }
            }
        }
    }
    if (ruined) {
        cout << "TEST 5 FAILED: NO DIFFERENCES DETECTED" << endl;
    }
    
    //Close files, free memory
    for (int z = 0; z < a_checkedFileSpec.iNumChannels; z++)
    {
        delete [] a_ppCheckedAudioData[z];
    }
    delete [] a_ppCheckedAudioData;
    a_ppCheckedAudioData = 0;
    
    for (int z = 0; z < b_checkedFileSpec.iNumChannels; z++)
    {
        delete [] b_ppCheckedAudioData[z];
    }
    delete [] b_ppCheckedAudioData;
    b_ppCheckedAudioData = 0;
    
    CAudioFileIf::destroy(a_pCheckedFile);
    CAudioFileIf::destroy(b_pCheckedFile);
    
}




void     showClInfo()
{
    cout << "MUSI6106 Assignment Executable" << endl;
    cout << "(c) 2014-2022 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

