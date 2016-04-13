/*
 * Hdf5File.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2016 Antonin Portelli, Matt Spraggs
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

#ifndef Latan_Hdf5File_hpp_
#define Latan_Hdf5File_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/File.hpp>
#include <LatAnalyze/Mat.hpp>
#include <LatAnalyze/MatSample.hpp>
#include <H5Cpp.h>

BEGIN_LATAN_NAMESPACE

#ifndef H5_NO_NAMESPACE
#define H5NS H5
#endif

/******************************************************************************
 *                          HDF5 datafile class                               *
 ******************************************************************************/
class Hdf5File: public File
{
public:
    // constructors
    Hdf5File(void);
    Hdf5File(const std::string &name, const unsigned int mode);
    // destructor
    virtual ~Hdf5File(void);
    // access
    virtual void save(const DMat &m, const std::string &name);
    virtual void save(const DSample &ds, const std::string &name);
    virtual void save(const DMatSample &ms, const std::string &name);
    // read first name
    virtual std::string getFirstName(void);
    // tests
    virtual bool isOpen(void) const;
    // IO
    virtual void close(void);
    virtual void open(const std::string &name, const unsigned int mode);
private:
    // IO
            std::string getFirstGroupName(void);
    virtual std::string load(const std::string &name = "");
                   void load(DMat &m, const H5NS::DataSet &d);
                   void load(DSample &ds, const H5NS::DataSet &d);
                   void load(DMatSample &s, const H5NS::DataSet &d);
    // check name for forbidden characters
    static size_t nameOffset(const std::string &name);
private:
    // file name
    std::unique_ptr<H5NS::H5File> h5File_{nullptr};
};

END_LATAN_NAMESPACE

#endif // Latan_Hdf5File_hpp_
