#include "fit-phys-env.hpp"
#include <LatCore/XmlReader.hpp>
#include <LatAnalyze/CompiledModel.hpp>
#include <LatAnalyze/Io.hpp>

#define DRATIO(a,b) static_cast<double>(a)/static_cast<double>(b)

using namespace std;
using namespace Latan;

void FitEnv::reset(void)
{
    nT_.clear();
    nL_.clear();
    variable_.clear();
    varData_.clear();
    varName_.clear();
    varScalePow_.clear();
    quantity_.clear();
    quData_.clear();
    quName_.clear();
    ensemble_.clear();
    point_.clear();
    macro_.clear();
    scaleVar_ = nullptr;
}

Index FitEnv::getVarIndex(const string name)
{
    if (name == "nT")
    {
        return 0;
    }
    else if (name == "nL")
    {
        return 1;
    }
    else
    {
        auto it = variable_.find(name);
        
        if (it != variable_.end())
        {
            return it->second.index;
        }
        else
        {
            LATAN_ERROR(Range, "no variable with name '" + name + "'");
        }
    }
}

string FitEnv::getVarName(const Index i)
{
    if (i < static_cast<Index>(varName_.size()))
    {
        return varName_[i];
    }
    else
    {
        LATAN_ERROR(Range, "no variable with index " + strFrom(i));
    }
}

Index FitEnv::getQuIndex(const string name)
{
    auto it = quantity_.find(name);
    
    if (it != quantity_.end())
    {
        return it->second.index;
    }
    else
    {
        LATAN_ERROR(Range, "no quantity with name '" + name + "'");
    }
}

string FitEnv::getQuName(const Index i)
{
    if (i < static_cast<Index>(quName_.size()))
    {
        return quName_[i];
    }
    else
    {
        LATAN_ERROR(Range, "no variable with index " + strFrom(i));
    }
}

DVec FitEnv::getPhyPt(void)
{
    DVec phyPt(varName_.size());
    
    for (unsigned int i = 0; i < varName_.size(); ++i)
    {
        phyPt(i) = variable_[varName_[i]].physVal;
    }
    
    return phyPt;
}

vector<const DoubleModel *> FitEnv::getModels(void)
{
    vector<const DoubleModel *> m;
    
    for (auto &q: quantity_)
    {
        m.push_back(&q.second.model);
    }
    
    return m;
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
    scale_   = paramFile.getFirstValue<string>("scale");
    
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
    
    // fit variables
    {
        string  name;
        VarInfo var;
        
        name            = "T";
        var.physVal     = HUGE_VAL;
        var.dim         = -1;
        variable_[name] = var;
    }
    {
        string  name;
        VarInfo var;
        
        name            = "L";
        var.physVal     = HUGE_VAL;
        var.dim         = -1;
        variable_[name] = var;
    }
    node = paramFile.getFirstNode("variables", "variable");
    while (node)
    {
        string  name;
        VarInfo var;
        
        name            = XGFV(string, "name");
        var.physVal     = XGFV(double, "physical");
        var.dim         = XGFV(int, "dim");
        variable_[name] = var;
        if (name == scale_)
        {
            scaleVar_ = &(variable_[name]);
        }
        node = paramFile.getNextSameNode(node);
    }
    if (!scaleVar_)
    {
        LATAN_ERROR(Definition, "scaling variable '" + scale_
                    + "' not defined");
    }
    for (auto &v: variable_)
    {
        v.second.index = varName_.size();
        varName_.push_back(v.first);
    }
    for (auto &v: variable_)
    {
        varScalePow_.push_back(DRATIO(v.second.dim, scaleVar_->dim));
    }
    
    // fitted quantities
    node = paramFile.getFirstNode("quantities", "quantity");
    while (node)
    {
        string           name, code = "";
        Index            nPar;
        QuInfo           q;
        DoubleModel      m;
        shared_ptr<DVec> buf(new DVec(varName_.size()));
        
        name            = XGFV(string, "name");
        nPar            = XGFV(Index, "model", "nPar");
        q.dim           = XGFV(int, "dim");
        for (auto &v: variable_)
        {
            code += v.first + " = x_" + strFrom(v.second.index) + "; ";
            code += v.first + "_phy = " + strFrom(v.second.physVal) + "; ";
        }
        code           += XGFV(string, "model", "code");
        DEBUG_VAR(code);
        m               = compile(code, variable_.size(), nPar);
        auto wrap       = [m, buf, this, q](const double *x, const double *p)
        {
            double s = x[scaleVar_->index];
            
            for (unsigned int i = 0; i < varScalePow_.size(); ++i)
            {
                if (i != scaleVar_->index)
                {
                    (*buf)(i) = x[i]*pow(s, varScalePow_[i]);
                }
                else
                {
                    (*buf)(i) = x[i];
                }
            }
            
            return pow(s, -DRATIO(q.dim, scaleVar_->dim))*m(buf->data(), p);
        };
        q.model.setFunction(wrap, m.getNArg(), m.getNPar());
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
            if (v.first == "T")
            {
                fileName = strFrom(point.ensemble->nT);
            }
            else if (v.first == "L")
            {
                fileName = strFrom(point.ensemble->nL);
            }
            else
            {
                fileName = macroSubst(XGFV(string, v.first));
            }
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
        for (unsigned int i = 0; i < varName_.size(); ++i)
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
    for (unsigned int i = 0; i < varName_.size(); ++i)
    {
        auto &v = varData_[i];
        
        if ((varName_[i] == "T") or (varName_[i] == "L"))
        {
            for (auto &d: v)
            {
                d.value.resize(nSample_);
                d.value.fill(strTo<double>(d.fileName));
            }
        }
        else
        {
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

XYSampleData FitEnv::generateData(const bool phyUnits, const bool corr)
{
    XYSampleData  data(nSample_);
    Index         k, k1, k2, ind;
    const Index   sInd = getVarIndex(scale_);
    DSample       scale, tmp;
    int           dim;
    const int     sDim = scaleVar_->dim;
    
    // add dimensions
    for (unsigned int i = 0; i < varName_.size(); ++i)
    {
        data.addXDim(varData_[i].size(), varName_[i],
                     ((varName_[i] == "T") or (varName_[i] == "L")));
    }
    for (auto &q: quName_)
    {
        data.addYDim(q);
    }
    // add data
    for (auto &p: point_)
    {
        k     = data.dataIndex(p.coord);
        scale = varData_[sInd][varIndex_[scale_][p.fileName[scale_]]].value;
        for (unsigned int i = 0; i < varName_.size(); ++i)
        {
            ind = varIndex_[varName_[i]][p.fileName[varName_[i]]];
            dim = variable_[varName_[i]].dim;
            tmp = varData_[i][ind].value;
            if (phyUnits and (varName_[i] != scale_))
            {
                FOR_STAT_ARRAY(tmp, s)
                {
                    tmp[s] *= pow(scale[s], DRATIO(dim, sDim));
                }
            }
            data.x(p.coord[i], i) = tmp;
        }
        for (unsigned int j = 0; j < quName_.size(); ++j)
        {
            ind = quIndex_[quName_[j]][p.fileName[quName_[j]]];
            dim = quantity_[quName_[j]].dim;
            tmp = quData_[j][ind].value;
            if (phyUnits)
            {
                FOR_STAT_ARRAY(tmp, s)
                {
                    tmp[s] *= pow(scale[s], DRATIO(dim, sDim));
                }
            }
            data.y(k, j) = tmp;
        }
    }
    // add correlations
    if (corr)
    {
        for (unsigned int p1 = 0; p1 < point_.size(); ++p1)
        for (unsigned int p2 = p1; p2 < point_.size(); ++p2)
        {
            if (point_[p1].ensemble == point_[p2].ensemble)
            {
                k1 = data.dataIndex(point_[p1].coord);
                k2 = data.dataIndex(point_[p2].coord);
                for (unsigned int i1 = 0; i1 < varName_.size(); ++i1)
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
                for (unsigned int i = 0; i < varName_.size(); ++i)
                for (unsigned int j = 0; j < quName_.size(); ++j)
                {
                    data.assumeXYCorrelated(true, point_[p1].coord[i], i, k2, j);
                    data.assumeXYCorrelated(true, point_[p2].coord[i], i, k1, j);
                }
            }
        }
    }
    
    return data;
}

ostream & operator<<(ostream &out, FitEnv &f)
{
    for (unsigned int i = 0; i < f.varName_.size(); ++i)
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
