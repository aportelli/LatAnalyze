/*
 * BinReader.hpp, part of LatAnalyze
 *
 * Copyright (C) 2015 Antonin Portelli
 *
 * LatAnalyze is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LatAnalyze_BinReader_hpp_
#define LatAnalyze_BinReader_hpp_

#include <LatAnalyze/Global.hpp>

BEGIN_LATAN_NAMESPACE

// I/O exception
class BinIO: public std::runtime_error
{
public:
    BinIO(std::string msg, std::string loc);
};

/******************************************************************************
 *                        Byte manipulation utilities                         *
 ******************************************************************************/
class Endianness
{
public:
    enum: uint32_t
    {
        little  = 0x00000001,
        big     = 0x01000000,
        unknown = 0xffffffff
    };
};

class ByteManip
{
public:
    static constexpr uint32_t getHostEndianness(void)
    {
        return ((0xffffffff & 1) == Endianness::little) ? Endianness::little
            : (((0xffffffff & 1) == Endianness::big) ? Endianness::big
               : Endianness::unknown);
    }
    template <typename T>
    static T swapBytes(const T);
};

/******************************************************************************
 *                         template implementation                            *
 ******************************************************************************/
template <typename T>
T ByteManip::swapBytes(const T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");
    
    union
    {
        T             u;
        unsigned char u8[sizeof(T)];
    } source, dest;
    
    source.u = u;
    for (size_t k = 0; k < sizeof(T); ++k)
    {
        dest.u8[k] = source.u8[sizeof(T) - k - 1];
    }
    
    return dest.u;
}

/******************************************************************************
 *                        Utility to read binary files                        *
 ******************************************************************************/
class BinReader
{
public:
    // constructor
    BinReader(void) = default;
    BinReader(const std::string fileName,
              const uint32_t endianness = ByteManip::getHostEndianness(),
              const bool isColMaj = false);
    // destructor
    virtual ~BinReader(void) = default;
    // I/O
    void       open(const std::string fileName,
                    const uint32_t endianness = ByteManip::getHostEndianness(),
                    const bool isColMaj = false);
    void       close(void);
    template <typename T>
    void       read(T *pt, Index size);
    template <typename T>
    T          read(void);
    template <typename T>
    MatBase<T> read(const Index nRow, const Index nCol);
private:
    std::unique_ptr<std::ifstream> file_{nullptr};
    std::string                    fileName_;
    size_t                         size_;
    uint32_t                       endianness_;
    bool                           isColMaj_;
};

/******************************************************************************
 *                         template implementation                            *
 ******************************************************************************/
template <typename T>
void BinReader::read(T *pt, Index n)
{
    if (file_ != nullptr)
    {
        file_->read(reinterpret_cast<char *>(pt),
                    static_cast<long>(sizeof(T))*n);
        if (endianness_ != ByteManip::getHostEndianness())
        {
            for (Index i = 0; i < n; ++i)
            {
                pt[i] = ByteManip::swapBytes(pt[i]);
            }
        }
        
    }
    else
    {
        LATAN_ERROR(Io, "file is not opened");
    }
}

template <typename T>
T BinReader::read(void)
{
    T x;
    
    if (file_ != nullptr)
    {
        read(&x, 1);
    }
    else
    {
        LATAN_ERROR(Io, "file is not opened");
    }
    
    return x;
}

template <>
std::string BinReader::read(void);

template <typename T>
MatBase<T> BinReader::read(const Index nRow, const Index nCol)
{
    MatBase<T> m;
    
    // Eigen matrices use column-major ordering
    if (isColMaj_)
    {
        m.resize(nRow, nCol);
    }
    else
    {
        m.resize(nCol, nRow);
    }
    if (file_ != nullptr)
    {
        read(m.data(), nRow*nCol);
    }
    else
    {
        LATAN_ERROR(Io, "file is not opened");
    }
    if (isColMaj_)
    {
        return m;
    }
    else
    {
        return m.transpose();

    }
}

END_LATAN_NAMESPACE

#endif // LatAnalyze_BinReader_hpp_
