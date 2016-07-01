#include "fit-phys-env.hpp"
#include <LatCore/XmlReader.hpp>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/Io.hpp>

using namespace std;
using namespace Latan;

void FitEnv::reset(void)
{
    nT_.clear();
    nL_.clear();
    variable_.clear();
    varData_.clear();
    varName_.clear();
    quantity_.clear();
    quData_.clear();
    quName_.clear();
    ensemble_.clear();
    point_.clear();
    macro_.clear();
}

#define XGFV(type, ...) XmlReader::getFirstValue<type>(node, __VA_ARGS__)

void FitEnv::parseXml(const string paramFileName)
{
    XmlReader                paramFile(paramFileName);
    const XmlNode            *node = nullptr;
    set<unsigned int>        nTs, nLs;
    map<string, set<string>> varFileNames, quFileNames;
    
    reset();
    nSample_ = paramFile.getFirstValue<Index>("nSample");
    
    // macros
    if (paramFile.hasNode("macros", "macro"))
    {
        node = paramFile.getFirstNode("macros", "macro");
        while (node)
        {
            macro_[XGFV(string, "symbol")] = XGFV(string, "value");
            node                           = paramFile.getNextSameNode(node);
        }
    }
    
    // ensembles
    node = paramFile.getFirstNode("ensembles", "ensemble");
    while (node)
    {
        string   name, spacing;
        Ensemble ens;
        
        name            = XGFV(string, "name");
        ens.nT          = XGFV(unsigned int, "nT");
        ens.nL          = XGFV(unsigned int, "nL");
        ensemble_[name] = ens;
        node            = paramFile.getNextSameNode(node);
        nTs.insert(ens.nT);
        nLs.insert(ens.nL);
    }
    varData_.push_back(vector<Data>());
    for (auto nT: nTs)
    {
        Data d;
        
        nT_.push_back(nT);
        d.fileName = "";
        d.value.fill(nT);
        varData_.back().push_back(d);
    }
    varData_.push_back(vector<Data>());
    for (auto nL: nLs)
    {
        Data d;
        
        nL_.push_back(nL);
        d.fileName = "";
        d.value.fill(nL);
        varData_.back().push_back(d);
    }
    
    // fit variables
    node = paramFile.getFirstNode("variables", "variable");
    while (node)
    {
        string  name;
        VarInfo var;
        
        name            = XGFV(string, "name");
        var.physVal     = XGFV(double, "physical");
        variable_[name] = var;
        node            = paramFile.getNextSameNode(node);
    }
    varName_.push_back("nT");
    varName_.push_back("nL");
    for (auto &v: variable_)
    {
        v.second.index = varName_.size();
        varName_.push_back(v.first);
    }
    
    // fitted quantities
    node = paramFile.getFirstNode("quantities", "quantity");
    while (node)
    {
        string  name, code = "nT = x_0; nL = x_1; ";
        Index   nPar;
        QuInfo  q;
        
        name            = XGFV(string, "name");
        nPar            = XGFV(Index, "model", "nPar");
        for (auto &v: variable_)
        {
            code += v.first + " = x_" + strFrom(v.second.index) + "; ";
            code += v.first + "_phy = " + strFrom(v.second.physVal) + "; ";
        }
        code           += XGFV(string, "model", "code");
        q.model         = compile(code, variable_.size() + 3, nPar);
        quantity_[name] = q;
        node            = paramFile.getNextSameNode(node);
    }
    for (auto &q: quantity_)
    {
        q.second.index = quName_.size();
        quName_.push_back(q.first);
    }
    
    // data points
    node = paramFile.getFirstNode("points", "point");
    while (node)
    {
        string ensemble, fileName;
        Point  point;
        
        ensemble = XGFV(string, "ensemble");
        auto it  = ensemble_.find(ensemble);
        if (it == ensemble_.end())
        {
            LATAN_ERROR(Parsing, "unknown ensemble '" + ensemble + "'");
        }
        macro_["_ensemble_"] = ensemble;
        point.isActive       = XGFV(bool, "active");
        point.ensemble       = &(it->second);
        for (auto &v: variable_)
        {
            fileName                = macroSubst(XGFV(string, v.first));
            point.fileName[v.first] = fileName;
            varFileNames[v.first].insert(fileName);
        }
        for (auto &q: quantity_)
        {
            fileName                = macroSubst(XGFV(string, q.first));
            point.fileName[q.first] = fileName;
            quFileNames[q.first].insert(fileName);
        }
        point_.push_back(point);
        node = paramFile.getNextSameNode(node);
    }
    macro_.erase("_ensemble_");
    
    // compute data indices
    for (auto &v: varFileNames)
    {
        varData_.push_back(vector<Data>());
        for (auto &f: v.second)
        {
            Data d;
            
            d.fileName            = f;
            varIndex_[v.first][f] = varData_.back().size();
            varData_.back().push_back(d);
        }
    }
    for (auto &q: quFileNames)
    {
        quData_.push_back(vector<Data>());
        for (auto &f: q.second)
        {
            Data d;
            
            d.fileName           = f;
            quIndex_[q.first][f] = quData_.back().size();
            quData_.back().push_back(d);
        }
    }
    
    // compute point coordinates
    for (auto &p: point_)
    {
        p.coord.resize(varName_.size());
        p.coord[0] = find(nT_.begin(), nT_.end(), p.ensemble->nT) - nT_.begin();
        p.coord[1] = find(nL_.begin(), nL_.end(), p.ensemble->nL) - nL_.begin();
        for (unsigned int i = 2; i < varName_.size(); ++i)
        {
            p.coord[i] = varIndex_[varName_[i]][p.fileName[varName_[i]]];
        }
    }
}

#undef XGFV

std::string FitEnv::macroSubst(const std::string str) const
{
    std::string res = str, pat;
    
    for (auto &m: macro_)
    {
        pat      = "@" + m.first + "@";
        auto pos = res.find(pat);
        
        if (pos != string::npos)
        {
            res.replace(pos, pat.size(), m.second);
        }
    }
    
    return res;
}

void FitEnv::load(void)
{
    for (unsigned int i = 2; i < varName_.size(); ++i)
    {
        auto &v = varData_[i];
        
        for (auto &d: v)
        {
            d.value = Io::load<DSample>(d.fileName);
            if (d.value.size() != nSample_)
            {
                LATAN_ERROR(Size, "sample loaded from file '" + d.fileName
                            + "' has a wrong number of element (expected "
                            + strFrom(nSample_) + ", got "
                            + strFrom(d.value.size()) + ")");
            }
        }
    }
    for (auto &q: quData_)
    {
        for (auto &d: q)
        {
            d.value = Io::load<DSample>(d.fileName);
            if (d.value.size() != nSample_)
            {
                LATAN_ERROR(Size, "sample loaded from file '" + d.fileName
                            + "' has a wrong number of element (expected "
                            + strFrom(nSample_) + ", got "
                            + strFrom(d.value.size()) + ")");
            }
        }
    }

}

XYSampleData FitEnv::generateData(void)
{
    XYSampleData  data(nSample_);
    Index         k, k1, k2;
    
    // add dimensions
    data.addXDim(nT_.size(), "nT", true);
    data.addXDim(nL_.size(), "nL", true);
    for (unsigned int i = 2; i < varName_.size(); ++i)
    {
        data.addXDim(varData_[i].size(), varName_[i], false);
    }
    for (auto &q: quName_)
    {
        data.addYDim(q);
    }
    // add X data
    for (unsigned int i = 0; i < varName_.size(); ++i)
    for (unsigned int r = 0; r < varData_[i].size(); ++r)
    {
        data.x(r, i) = varData_[i][r].value;
    }
    // add Y data
    for (auto &p: point_)
    {
        k = data.dataIndex(p.coord);
        for (unsigned int j = 0; j < quName_.size(); ++j)
        {
            auto &n = quName_[j];
            
            data.y(k, j) = quData_[j][quIndex_[n][p.fileName[n]]].value;
        }
    }
    // add correlations
    for (unsigned int p1 = 0; p1 < point_.size(); ++p1)
    for (unsigned int p2 = p1; p2 < point_.size(); ++p2)
    {
        if (point_[p1].ensemble == point_[p2].ensemble)
        {
            k1 = data.dataIndex(point_[p1].coord);
            k2 = data.dataIndex(point_[p2].coord);
            for (unsigned int i1 = 2; i1 < varName_.size(); ++i1)
            for (unsigned int i2 = i1; i2 < varName_.size(); ++i2)
            {
                data.assumeXXCorrelated(true, point_[p1].coord[i1], i1,
                                        point_[p2].coord[i2], i2);
            }
            for (unsigned int j1 = 0; j1 < quName_.size(); ++j1)
            for (unsigned int j2 = j1; j2 < quName_.size(); ++j2)
            {
                data.assumeYYCorrelated(true, k1, j1, k2, j2);
            }
            for (unsigned int i = 2; i < varName_.size(); ++i)
            for (unsigned int j = 0; j < quName_.size(); ++j)
            {
                data.assumeXYCorrelated(true, point_[p1].coord[i], i, k2, j);
                data.assumeXYCorrelated(true, point_[p2].coord[i], i, k1, j);
            }
        }
    }
    
    return data;
}

ostream & operator<<(ostream &out, FitEnv &f)
{
    out << "nT:" << endl;
    for (auto nT: f.nT_)
    {
        out << "  * " << nT << endl;
    }
    out << "nL:" << endl;
    for (auto nL: f.nL_)
    {
        out << "  * " << nL << endl;
    }
    for (unsigned int i = 2; i < f.varName_.size(); ++i)
    {
        out << f.varName_[i] << ":" << endl;
        for (auto &d: f.varData_[i])
        {
            out << "  * " << d.fileName << endl;
        }
    }
    for (unsigned int i = 0; i < f.quName_.size(); ++i)
    {
        out << f.quName_[i] << ":" << endl;
        for (auto &d: f.quData_[i])
        {
            out << "  * " << d.fileName << endl;
        }
    }
    
    return out;
}
