#ifndef  _MONOBUFFER_H
#define  _MONOBUFFER_H

#include<cstddef>
#include<vector>

class monoBuffer
 {
 public:
     monoBuffer();
     monoBuffer(unsigned char* sBuffer, int width, int height);
     ~monoBuffer();

     void allocBuffer(int width,int height);
     void copyBufferVert(int nCol, int& curColIndex, monoBuffer& sBuffer, int& nBlend);
     void copyBufferHoriz(int nRow, int& curRowIndex, monoBuffer& sBuffer);
     void trimEmptyLines();

     bool isEmpty();

     unsigned char* getBuffer() const;

     unsigned char& operator[](std::size_t idx)
     {
         return buffer_[idx];
     }
     const unsigned char& operator[](std::size_t idx) const
     {
         return buffer_[idx];
     }

     unsigned int width_;
     unsigned int height_;

 protected:
     unsigned char* buffer_;
     bool empty;
 };

#endif  //_MONOBUFFER_H
