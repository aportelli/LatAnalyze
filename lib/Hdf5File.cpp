/*
 * Hdf5File.cpp, part of LatAnalyze 3
 *
 * Copyright (C) 2013 - 2015 Antonin Portelli
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

#include <LatAnalyze/Hdf5File.hpp>
#include <LatAnalyze/IoObject.hpp>
#include <LatAnalyze/includes.hpp>

using namespace std;
using namespace Latan;
#ifndef H5_NO_NAMESPACE
using namespace H5NS;
#endif

constexpr unsigned int maxGroupNameSize = 1024u;

const short dMatType       = static_cast<short>(IoObject::IoType::dMat);
const short dMatSampleType = static_cast<short>(IoObject::IoType::dMatSample);
const short rgStateType    = static_cast<short>(IoObject::IoType::rgState);

/******************************************************************************
 *                          Hdf5File implementation                           *
 ******************************************************************************/
// constructors ////////////////////////////////////////////////////////////////
Hdf5File::Hdf5File(void)
{}

Hdf5File::Hdf5File(const std::string &name, const unsigned int mode)
{
    open(name, mode);
}

// destructor //////////////////////////////////////////////////////////////////
Hdf5File::~Hdf5File(void)
{
    close();
}

// access //////////////////////////////////////////////////////////////////////
void Hdf5File::save(const DMat &m, const string &name)
{
    if (name.empty())
    {
        LATAN_ERROR(Io, "trying to save data with an empty name");
    }

    Group     group;
    Attribute attr;
    DataSet   dataset;
    hsize_t   dim[2]  = {static_cast<hsize_t>(m.rows()),
                         static_cast<hsize_t>(m.cols())};
    hsize_t   attrDim = 1;

    DataSpace dataSpace(2, dim), attrSpace(1, &attrDim);

    group = h5File_->createGroup(name.c_str() + nameOffset(name));
    attr  = group.createAttribute("type", PredType::NATIVE_SHORT, attrSpace);
    attr.write(PredType::NATIVE_SHORT, &dMatType);
    dataset = group.createDataSet("data", PredType::NATIVE_DOUBLE, dataSpace);
    dataset.write(m.data(), PredType::NATIVE_DOUBLE);
}

void Hdf5File::save(const DMatSample &sample, const string &name)
{
    if (name.empty())
    {
        LATAN_ERROR(Io, "trying to save data with an empty name");
    }

    Group          group;
    Attribute      attr;
    DataSet        dataset;
    hsize_t        dim[2]  = {static_cast<hsize_t>(sample[central].rows()),
                              static_cast<hsize_t>(sample[central].cols())};
    hsize_t        attrDim = 1;
    DataSpace      dataSpace(2, dim), attrSpace(1, &attrDim);
    const long int nSample = sample.size();
    string         datasetName;

    group = h5File_->createGroup(name.c_str() + nameOffset(name));
    attr  = group.createAttribute("type", PredType::NATIVE_SHORT, attrSpace);
    attr.write(PredType::NATIVE_SHORT, &dMatSampleType);
    attr  = group.createAttribute("nSample", PredType::NATIVE_LONG, attrSpace);
    attr.write(PredType::NATIVE_LONG, &nSample);
    FOR_STAT_ARRAY(sample, s)
    {
        datasetName = (s == central) ? "data_C" : ("data_S_" + strFrom(s));
        dataset     = group.createDataSet(datasetName, PredType::NATIVE_DOUBLE,
                                          dataSpace);
        dataset.write(sample[s].data(), PredType::NATIVE_DOUBLE);
    }
}

void Hdf5File::save(const RandGenState &state, const string &name)
{
    if (name.empty())
    {
        LATAN_ERROR(Io, "trying to save data with an empty name");
    }

    Group     group;
    Attribute attr;
    DataSet   dataset;
    hsize_t   dim     = RLXG_STATE_SIZE;
    hsize_t   attrDim = 1;
    DataSpace dataSpace(1, &dim), attrSpace(1, &attrDim);

    group = h5File_->createGroup(name.c_str() + nameOffset(name));
    attr  = group.createAttribute("type", PredType::NATIVE_SHORT, attrSpace);
    attr.write(PredType::NATIVE_SHORT, &rgStateType);
    dataset = group.createDataSet("data", PredType::NATIVE_DOUBLE, dataSpace);
    dataset.write(state.data(), PredType::NATIVE_DOUBLE);
}

// read first name ////////////////////////////////////////////////////////////
string Hdf5File::getFirstName(void)
{
    return getFirstGroupName();
}

// tests ///////////////////////////////////////////////////////////////////////
bool Hdf5File::isOpen(void) const
{
    return (h5File_ != nullptr);
}

// check names for forbidden characters ////////////////////////////////////////
size_t Hdf5File::nameOffset(const string &name)
{
    size_t ret      = 0;
    string badChars = "/";

    for (auto c : badChars)
    {
        size_t pos = name.rfind(c);

        if (pos != string::npos and pos > ret)
        {
            ret = pos;
        }
    }

    return ret;
}

// IO //////////////////////////////////////////////////////////////////////////
void Hdf5File::close(void)
{
    if (isOpen())
    {
        h5File_->close();
    }
    h5File_.reset(nullptr);
    name_ = "";
    mode_ = Mode::null;
    deleteData();
}

void Hdf5File::open(const string &name, const unsigned int mode)
{
    if (isOpen())
    {
        LATAN_ERROR(Io, "file already opened with name '" + name_ + "'");
    }
    else
    {
        unsigned int h5Mode = 0;

        name_ = name;
        mode_ = mode;

        if (mode & Mode::write)
        {
            h5Mode |= H5F_ACC_TRUNC;
        }
        if (mode & Mode::read)
        {
            h5Mode |= H5F_ACC_RDONLY;
        }
        if (mode & Mode::append)
        {
            h5Mode |= H5F_ACC_RDWR|H5F_ACC_CREAT;
        }
        h5File_.reset(new H5File(name_.c_str(), h5Mode));
    }
}

string Hdf5File::getFirstGroupName(void)
{
    string res;

    if ((mode_ & Mode::read)&&(isOpen()))
    {
        auto firstGroupName = [](hid_t loc_id, const char *name, void *fname)
        {
            H5G_stat_t statbuf;

            H5Gget_objinfo(loc_id, name, 0, &statbuf);
            if ((statbuf.type == H5G_GROUP) && (strlen((char *)fname) == 0))
            {
                strncpy((char *)fname, name, maxGroupNameSize);
            }
            
            return 0;
        };

        char groupName[maxGroupNameSize] = "";

        h5File_->iterateElems("/", nullptr, firstGroupName, groupName);
        res = groupName;
    }
    else
    {
        if (isOpen())
        {
            LATAN_ERROR(Io, "file '" + name_ + "' is not opened in read mode");
        }
        else
        {
            LATAN_ERROR(Io, "file '" + name_ + "' is not opened");
        }

        return "";
    }

    return res;
}

void Hdf5File::load(DMat &m, const DataSet &d)
{
    DataSpace dataspace;
    hsize_t   dim[2];

    dataspace = d.getSpace();
    dataspace.getSimpleExtentDims(dim);
    m.resize(dim[0], dim[1]);
    d.read(m.data(), PredType::NATIVE_DOUBLE);
}

void Hdf5File::load(RandGenState &state, const DataSet &d)
{
    DataSpace dataspace;
    hsize_t   dim[1];

    dataspace = d.getSpace();
    dataspace.getSimpleExtentDims(dim);
    if (dim[0] != RLXG_STATE_SIZE)
    {
        // error here
    }
    d.read(state.data(), PredType::NATIVE_INT);
}

string Hdf5File::load(const string &name)
{
    if ((mode_ & Mode::read)&&(isOpen()))
    {
        string           groupName;
        Group            group;
        Attribute        attribute;
        DataSet          dataset;
        IoObject::IoType type;

        groupName = (name.empty()) ? getFirstGroupName() : name;
        if (groupName.empty())
        {
            LATAN_ERROR(Io, "file '" + name_ + "' is empty");
        }
        group     = h5File_->openGroup(groupName.c_str());
        attribute = group.openAttribute("type");
        attribute.read(PredType::NATIVE_SHORT, &type);
        switch (type)
        {
            case IoObject::IoType::dMat:
            {
                DMat *pt = new DMat;

                data_[groupName].reset(pt);
                dataset = group.openDataSet("data");
                load(*pt, dataset);
                break;
            }
            case IoObject::IoType::dMatSample:
            {
                DMatSample *pt = new DMatSample;
                long int   nSample;

                data_[groupName].reset(pt);
                attribute = group.openAttribute("nSample");
                attribute.read(PredType::NATIVE_LONG, &nSample);
                pt->resize(nSample);
                FOR_STAT_ARRAY(*pt, s)
                {
                    if (s == central)
                    {
                        dataset = group.openDataSet("data_C");
                    }
                    else
                    {
                        dataset = group.openDataSet("data_S_" + strFrom(s));
                    }
                    load((*pt)[s], dataset);
                }
                break;
            }
            case IoObject::IoType::rgState:
            {
                RandGenState *pt = new RandGenState;

                data_[groupName].reset(pt);
                dataset = group.openDataSet("data");
                load(*pt, dataset);
                break;
            }
            default:
            {
                LATAN_ERROR(Io, "unknown data type ("
                                + strFrom(static_cast<int>(type)) + ") "
                                " (" + name_ + ":" + groupName + ")");
                break;
            }
        }

        return groupName;
    }
    else
    {
        if (isOpen())
        {
            LATAN_ERROR(Io, "file '" + name_ + "' is not opened in read mode");
        }
        else
        {
            LATAN_ERROR(Io, "file '" + name_ + "' is not opened");
        }

        return "";
    }
}
