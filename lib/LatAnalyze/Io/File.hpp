/*
 * File.hpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2020 Antonin Portelli
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

#ifndef Latan_File_hpp_
#define	Latan_File_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/Io/IoObject.hpp>
#include <LatAnalyze/Core/Mat.hpp>
#include <LatAnalyze/Statistics/MatSample.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                          Abstract datafile class                           *
 ******************************************************************************/
typedef std::unordered_map<std::string, std::unique_ptr<IoObject>> IoDataTable;

class File
{
public:
    class Mode
    {
    public:
        enum
        {
            null   = 0,
            write  = 1 << 0,
            read   = 1 << 1,
            append = 1 << 2
        };
    };
public:
    // constructors
    File(void) = default;
    File(const std::string &name, const unsigned int mode);
    // destructor
    virtual ~File(void);
    // access
    const std::string & getName(void) const;
    unsigned int        getMode(void) const;
    template <typename IoT>
    const IoT &  read(const std::string &name = "");
    virtual void save(const DMat &m, const std::string &name)             = 0;
    virtual void save(const DSample &ds, const std::string &name)         = 0;
    virtual void save(const DMatSample &ms, const std::string &name)      = 0;
    // read first name
    virtual std::string getFirstName(void) = 0;
    // tests
    virtual bool isOpen(void) const = 0;
    // IO
    virtual void close(void)                                            = 0;
    virtual void open(const std::string &name, const unsigned int mode) = 0;
protected:
    // access
    void setName(const std::string &name);
    void setMode(const unsigned int mode);
    // data access
    void deleteData(void);
    // error checking
    void checkWritability(void);
private:
    // data access
    template <typename IoT>
    const IoT& getData(const std::string &name = "") const;
    // IO
    virtual std::string load(const std::string &name = "") = 0;
protected:
    std::string  name_{""};
    unsigned int mode_{Mode::null};
    IoDataTable  data_;
};

// Template implementations
template <typename IoT>
const IoT& File::read(const std::string &name)
{
    std::string dataName;
    
    dataName = load(name);
    
    return getData<IoT>(dataName);
}

template <typename IoT>
const IoT& File::getData(const std::string &name) const
{
    try
    {
        return dynamic_cast<const IoT &>(*(data_.at(name)));
    }
    catch(std::out_of_range)
    {
        LATAN_ERROR(Definition, "no data with name '" + name + "' in file "
                    + name_);
    }
    catch(std::bad_cast)
    {
        LATAN_ERROR(Definition, "data with name '" + name + "' in file "
                    + name_ + " does not have type '" + typeid(IoT).name()
                    + "'");
    }
}

END_LATAN_NAMESPACE

#endif
