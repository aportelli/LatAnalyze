/*
 * Dataset.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2014 Antonin Portelli
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

#include <latan/Global.hpp>
#include <latan/Io.hpp>
#include <latan/Sample.hpp>
#include <latan/RandGen.hpp>
#include <fstream>
#include <vector>

BEGIN_NAMESPACE

/******************************************************************************
 *                              Dataset class                                 *
 ******************************************************************************/
template <typename T, typename FileType>
class Dataset: public StatArray<T>
{
private:
    typedef StatArray<T> Base;
public:
    // constructor
    Dataset(void);
    Dataset(const std::string &listFileName, const std::string &dataName);
    template <typename Derived>
    Dataset(const Eigen::EigenBase<Derived> &dataset);
    // destructor
    virtual ~Dataset(void);
    // IO
    void load(const std::string &listFileName, const std::string &dataName);
    // resampling
    Sample<T> bootstrapMean(const unsigned int nSample, RandGen& generator);
private:
    // mean from pointer vector for resampling
    void ptVectorMean(T &m, const std::vector<const T *> &v);
private:
    FileType file_;
};

/******************************************************************************
 *                      Dataset template implementation                       *
 ******************************************************************************/
// constructor /////////////////////////////////////////////////////////////////
template <typename T, typename FileType>
Dataset<T, FileType>::Dataset(void)
{}

template <typename T, typename FileType>
Dataset<T, FileType>::Dataset(const std::string &listFileName,
                              const std::string &dataName)
{
    load(listFileName, dataName);
}

template <typename T, typename FileType>
template <typename Derived>
Dataset<T, FileType>::Dataset(const Eigen::EigenBase<Derived> &dataset)
: Base(dataset)
{}

// destructor //////////////////////////////////////////////////////////////////
template <typename T, typename FileType>
Dataset<T, FileType>::~Dataset(void)
{}

// IO //////////////////////////////////////////////////////////////////////////
template <typename T, typename FileType>
void Dataset<T, FileType>::load(const std::string &listFileName,
                                const std::string &dataName)
{
    std::ifstream listFile;
    char dataFileNameBuf[MAX_PATH_LENGTH];
    std::vector<std::string> dataFileName;
    
    listFile.open(listFileName, std::ios::in);
    while (!listFile.eof())
    {
        listFile.getline(dataFileNameBuf, MAX_PATH_LENGTH);
        if (!std::string(dataFileNameBuf).empty())
        {
            dataFileName.push_back(dataFileNameBuf);
        }
    }
    listFile.close();
    this->resize(dataFileName.size());
    for (unsigned int i = 0; i < dataFileName.size(); ++i)
    {
        file_.open(dataFileName[i], File::Mode::read);
        (*this)[i] = file_.template read<T>(dataName);
        file_.close();
    }
}

// resampling //////////////////////////////////////////////////////////////////
template <typename T, typename FileType>
Sample<T> Dataset<T, FileType>::bootstrapMean(const unsigned int nSample,
                                              RandGen& generator)
{
    unsigned int nData = this->size();
    std::vector<const T *> data(nData);
    Sample<T> s(nSample);
    
    for (unsigned int j = 0; j < nData; ++j)
    {
        data[j] = &((*this)[j]);
    }
    ptVectorMean(s[central], data);
    for (unsigned int i = 0; i < nSample; ++i)
    {
        for (unsigned int j = 0; j < nData; ++j)
        {
            data[j] = &((*this)[generator.discreteUniform(nData)]);
        }
        ptVectorMean(s[i], data);
    }
    
    return s;
}

template <typename T, typename FileType>
void Dataset<T, FileType>::ptVectorMean(T &m, const std::vector<const T *> &v)
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

END_NAMESPACE

#endif // Latan_Dataset_hpp_
