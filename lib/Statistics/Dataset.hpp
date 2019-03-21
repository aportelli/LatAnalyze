/*
 * Dataset.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli
 *
 * LatAnalyze 3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LatAnalyze 3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LatAnalyze 3.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Latan_Dataset_hpp_
#define Latan_Dataset_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Io/File.hpp>
#include <LatAnalyze/Statistics/StatArray.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                              Dataset class                                 *
 ******************************************************************************/
template <typename T>
class Dataset: public StatArray<T>
{
public:
    typedef std::random_device::result_type SeedType;
public:
    // constructors
    Dataset(void) = default;
    Dataset(const Index size);
    EIGEN_EXPR_CTOR(Dataset, Dataset<T>, StatArray<T>, ArrayExpr)
    // destructor
    virtual ~Dataset(void) = default;
    // IO
    template <typename FileType>
    void load(const std::string &listFileName, const std::string &dataName);
    // resampling
    Sample<T> bootstrapMean(const Index nSample, const SeedType seed);
    Sample<T> bootstrapMean(const Index nSample);
    void      dumpBootstrapSeq(std::ostream &out, const Index nSample,
                               const SeedType seed);
private:
    // mean from pointer vector for resampling
    void ptVectorMean(T &m, const std::vector<const T *> &v);
};

/******************************************************************************
 *                      Dataset template implementation                       *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
template <typename T>
Dataset<T>::Dataset(const Index size)
: StatArray<T>(size)
{}

// IO //////////////////////////////////////////////////////////////////////////
template <typename T>
template <typename FileType>
void Dataset<T>::load(const std::string &listFileName,
                      const std::string &dataName)
{
    FileType file;
    std::vector<std::string> dataFileName;

    dataFileName = readManifest(listFileName);
    this->resize(dataFileName.size());
    for (Index i = 0; i < static_cast<Index>(dataFileName.size()); ++i)
    {
        file.open(dataFileName[i], File::Mode::read);
        (*this)[i] = file.template read<T>(dataName);
        file.close();
    }
}

// resampling //////////////////////////////////////////////////////////////////
template <typename T>
Sample<T> Dataset<T>::bootstrapMean(const Index nSample, const SeedType seed)
{
    std::vector<const T *>               data(this->size());
    Sample<T>                            s(nSample);
    std::mt19937                         gen(seed);
    std::uniform_int_distribution<Index> dis(0, this->size() - 1);
    
    for (unsigned int j = 0; j < this->size(); ++j)
    {
        data[j] = &((*this)[static_cast<Index>(j)]);
    }
    ptVectorMean(s[central], data);
    for (Index i = 0; i < nSample; ++i)
    {
        for (unsigned int j = 0; j < this->size(); ++j)
        {
            data[j] = &((*this)[dis(gen)]);
        }
        ptVectorMean(s[i], data);
    }
    
    return s;
}

template <typename T>
Sample<T> Dataset<T>::bootstrapMean(const Index nSample)
{
    std::random_device rd;
    
    return bootstrapMean(nSample, rd());
}

template <typename T>
void Dataset<T>::dumpBootstrapSeq(std::ostream &out, const Index nSample,
                                  const SeedType seed)
{
    std::mt19937                         gen(seed);
    std::uniform_int_distribution<Index> dis(0, this->size() - 1);

    for (Index i = 0; i < nSample; ++i)
    {
        for (unsigned int j = 0; j < this->size(); ++j)
        {
            out << dis(gen) << " " << std::endl;
        }
        out << std::endl;
    }
}

template <typename T>
void Dataset<T>::ptVectorMean(T &m, const std::vector<const T *> &v)
{
    if (v.size())
    {
        m = *(v[0]);
        for (unsigned int i = 1; i < v.size(); ++i)
        {
            m += *(v[i]);
        }
        m /= static_cast<double>(v.size());
    }
}

END_LATAN_NAMESPACE

#endif // Latan_Dataset_hpp_
