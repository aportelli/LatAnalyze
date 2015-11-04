/*
 * Io.hpp, part of LatAnalyze 3
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

#ifndef Latan_Io_hpp_
#define Latan_Io_hpp_

#include <LatAnalyze/Global.hpp>
#include <LatAnalyze/AsciiFile.hpp>
#include <LatAnalyze/Hdf5File.hpp>

BEGIN_LATAN_NAMESPACE

/******************************************************************************
 *                          Static IO functions                               *
 ******************************************************************************/
class Io
{
public:
    template <typename IoT, typename FileType>
    static IoT  load(const std::string &fileName, const std::string &name = "");
    template <typename IoT>
    static IoT  load(const std::string &fileName, const std::string &name = "");
    template <typename IoT, typename FileType>
    static void save(const IoT &data, const std::string &fileName,
                     const unsigned int mode = File::Mode::write,
                     const std::string &name = "");
    template <typename IoT>
    static void save(const IoT &data, const std::string &fileName,
                     const unsigned int mode = File::Mode::write,
                     const std::string &name = "");
    template <typename FileType>
    static std::string getFirstName(const std::string &fileName);
    static std::string getFirstName(const std::string &fileName);
    static std::unique_ptr<File> open(const std::string &fileName,
                                      const unsigned int mode = File::Mode::write);
};

// template implementation /////////////////////////////////////////////////////
template <typename IoT, typename FileType>
IoT Io::load(const std::string &fileName, const std::string &name)
{
    FileType file(fileName, File::Mode::read);

    return file.template read<IoT>(name);
}

template <typename IoT>
IoT Io::load(const std::string &fileName, const std::string &name)
{
    std::string ext = extension(fileName);

    if (ext == "h5")
    {
        return load<IoT, Hdf5File>(fileName, name);
    }
    else if ((ext == "dat")||(ext == "sample")||(ext == "seed"))
    {
        return load<IoT, AsciiFile>(fileName, name);
    }
    else
    {
        LATAN_ERROR(Io, "unknown file extension '" + ext + "'");
    }
}

template <typename IoT, typename FileType>
void Io::save(const IoT &data, const std::string &fileName,
              const unsigned int mode, const std::string &name)
{
    FileType file(fileName, mode);
    std::string realName = (name.empty()) ? fileName : name;

    file.save(data, realName);
}

template <typename IoT>
void Io::save(const IoT &data, const std::string &fileName,
              const unsigned int mode, const std::string &name)
{
    std::string ext = extension(fileName);

    if (ext == "h5")
    {
        save<IoT, Hdf5File>(data, fileName, mode, name);
    }
    else if ((ext == "dat")||(ext == "sample")||(ext == "seed"))
    {
        save<IoT, AsciiFile>(data, fileName, mode, name);
    }
    else
    {
        LATAN_ERROR(Io, "unknown file extension '" + ext + "'");
    }
}

template <typename FileType>
std::string Io::getFirstName(const std::string &fileName)
{
    FileType file(fileName, File::Mode::read);

    return file.getFirstName();
}

END_LATAN_NAMESPACE

#endif // Latan_Io_hpp_
