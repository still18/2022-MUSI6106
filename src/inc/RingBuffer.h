#if !defined(__RingBuffer_hdr__)
#define __RingBuffer_hdr__

#include <cassert>
#include <algorithm>
#include <cmath>
#include <cstring>

/*! \brief implement a circular buffer of type T
*/
template <class T>
class CRingBuffer
{
public:
    explicit CRingBuffer(int iBufferLengthInSamples) :
        m_iBuffLength(iBufferLengthInSamples),
        m_iWriteInd(0),
        m_iReadInd(0),
        m_iBuffData(0)
    {
        assert(iBufferLengthInSamples > 0);

        // allocate and init
        m_iBuffData = new T[m_iBuffLength];
    }

    virtual ~CRingBuffer()
    {
        // free memory
        delete [] m_iBuffData;
    }

    /*! add a new value of type T to write index and increment write index
    \param tNewValue the new value
    \return void
    */
    void putPostInc (T tNewValue)
    {
        put(tNewValue);
        m_iWriteInd++;
        if (m_iWriteInd >= m_iBuffLength) {
            m_iWriteInd = 0;
        }
    }

    /*! add a new value of type T to write index
    \param tNewValue the new value
    \return void
    */
    void put(T tNewValue)
    {
        m_iBuffData[m_iWriteInd] = tNewValue;
    }
    
    /*! return the value at the current read index and increment the read pointer
    \return float the value from the read index
    */
    T getPostInc()
    {
        T atVal = get();
        m_iReadInd++;
        if (m_iReadInd >= m_iBuffLength) {
            m_iReadInd = 0;
        }
        return (atVal);
    }

    /*! return the value at the current read index
    \return float the value from the read index
    */
    T get() const
    {
        return (m_iBuffData[m_iReadInd]);
    }
    
    /*! set buffer content and indices to 0
    \return void
    */
    void reset()
    {
        m_iReadInd = 0;
        m_iWriteInd = 0;
        delete [] m_iBuffData;
        m_iBuffData = new T[m_iBuffLength];
    }

    /*! return the current index for writing/put
    \return int
    */
    int getWriteIdx() const
    {
        return m_iWriteInd;
    }

    /*! move the write index to a new position
    \param iNewWriteIdx: new position
    \return void
    */
    void setWriteIdx(int iNewWriteIdx)
    {
        m_iWriteInd = iNewWriteIdx;
    }

    /*! return the current index for reading/get
    \return int
    */
    int getReadIdx() const
    {
        return m_iReadInd;
    }

    /*! move the read index to a new position
    \param iNewReadIdx: new position
    \return void
    */
    void setReadIdx(int iNewReadIdx)
    {
        m_iReadInd = iNewReadIdx;
    }

    /*! returns the number of values currently buffered (note: 0 could also mean the buffer is full!)
    \return int
    */
    int getNumValuesInBuffer() const
    {
        return m_iWriteInd - m_iReadInd;
    }

    /*! returns the length of the internal buffer
    \return int
    */
    int getLength() const
    {
        return m_iBuffLength;
    }
private:
    CRingBuffer();
    CRingBuffer(const CRingBuffer& that);

    int m_iBuffLength;              //!< length of the internal buffer
    int m_iWriteInd;
    int m_iReadInd;
    T* m_iBuffData;
    
};
#endif // __RingBuffer_hdr__

