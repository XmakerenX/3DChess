#include"monoBuffer.h"

//-----------------------------------------------------------------------------
// Name : monoBuffer (constructor)
//-----------------------------------------------------------------------------
monoBuffer::monoBuffer()
{
    buffer_ = nullptr;
    empty = true;
    width_ = 0;
    height_ = 0;
}

//-----------------------------------------------------------------------------
// Name : monoBuffer (constructor)
//-----------------------------------------------------------------------------
monoBuffer::monoBuffer(unsigned char *sBuffer, int width, int height)
{
    buffer_ = nullptr;
    if (sBuffer != nullptr)
    {
        allocBuffer(width, height);

        int size = width * height;

        for (int i = 0; i < size; i++)
            buffer_[i] = sBuffer[i];
    }
    else
    {
        width = 0;
        height = 0;
        empty = true;
    }
}

//-----------------------------------------------------------------------------
// Name : monoBuffer (destructor)
//-----------------------------------------------------------------------------
monoBuffer::~monoBuffer()
{
    if (buffer_)
        delete[] buffer_;

    width_ = 0;
    height_ = 0;
}

//-----------------------------------------------------------------------------
// Name : allocBuffer
//-----------------------------------------------------------------------------
void monoBuffer::allocBuffer(int width, int height)
{
    if (buffer_)
        delete[] buffer_;

    if (width != 0 && height != 0)
    {
        buffer_ = new unsigned char[width * height];

        width_ = width;
        height_ = height;
        empty = false;
    }
}

//-----------------------------------------------------------------------------
// Name : isEmpty
//-----------------------------------------------------------------------------
bool monoBuffer::isEmpty()
{
    return empty;
}

//-----------------------------------------------------------------------------
// Name : copyBufferVert
//-----------------------------------------------------------------------------
void monoBuffer::copyBufferVert(int nCol, int &curColIndex, monoBuffer &sBuffer, int &nBlend)
{
    for (int k = 0; k < nCol; k++)
    {
        for (int i = 0; i < height_; i++)
        {
            if (sBuffer.isEmpty() == false)
            {
                if (nBlend == 0 || buffer_[(i * width_) + k] > 20 )
                    buffer_[(i * width_) + curColIndex] = sBuffer[(i * sBuffer.width_)  + k];
            }
            else
                if (nBlend == 0)
                    buffer_[(i * width_) + curColIndex] = 0;
        }

        curColIndex++;
        if (nBlend != 0)
            nBlend--;
    }
}

//-----------------------------------------------------------------------------
// Name : copyBufferHoriz
//-----------------------------------------------------------------------------
void monoBuffer::copyBufferHoriz(int nRow, int &curRowIndex, monoBuffer &sBuffer)
{
    for (int k = 0; k < nRow; k++)
    {
        for (int j = 0; j < width_; j++)
        {
            if (sBuffer.isEmpty() == false)
            {
                buffer_[(curRowIndex * width_) + j] = sBuffer[(k * sBuffer.width_)  + j];
            }
            else
                buffer_[(curRowIndex * width_) + j] = 0;
        }

        curRowIndex++;
    }
}

//-----------------------------------------------------------------------------
// Name : trimEmptyLines
// Desc : Trims all the empty rows in the buffer
//-----------------------------------------------------------------------------
void monoBuffer::trimEmptyLines()
{
    int start = 0;
    int end = 0;

    bool found = false;

    // index of rows that aren't empty
    std::vector<int> linesToCopy;
    for (int i = 0; i < height_; i++)
    {
        for (int j = 0; j < width_; j++)
        {
            // if row isn't empty add it index and move to next row
            if (buffer_[(i * width_) + j] != 0)
            {
                linesToCopy.push_back(i);
                start = i;
                found = true;
                break;
            }
        }
        if (found)
            break;
    }

    found = false;

    for (int i = height_ - 1; i >= 0; i--)
    {
        for (int j = 0; j < width_; j++)
        {
            // if row isn't empty add it index and move to next row
            if (buffer_[(i * width_) + j] != 0)
            {
                linesToCopy.push_back(i);
                end = i;
                found = true;
                break;
            }
        }

        if (found)
            break;
    }

    // calc size of new buffer trimed buffer
    //unsigned char * newBuffer = new unsigned char[(linesToCopy.size())*width_];
    unsigned char * newBuffer = new unsigned char[(end - start + 1)*width_];

    int i = 0;

    for (int k = start; k <= end; k++)
    {
        for (int j = 0; j < width_; j++)
        {
            newBuffer[(i * width_) + j] = buffer_[k * width_ + j];
        }
        i++;
    }


    // copy the non empty rows in the original buffer
//    for ( int k = 0; k < linesToCopy.size(); k++)
//    {
//        for (int j = 0; j < width_; j++)
//        {
//            newBuffer[(i * width_) + j] = buffer_[(linesToCopy[k] * width_) + j];
//        }
//        i++;
//    }

    // free the original buffer
    delete buffer_;
    // set the trimed buffer as buffer
    buffer_ = newBuffer;
    // set the new height of the buffer
    height_ = end - start + 1;
}

//-----------------------------------------------------------------------------
// Name : getBuffer
//-----------------------------------------------------------------------------
unsigned char* monoBuffer::getBuffer() const
{
    return buffer_;
}
