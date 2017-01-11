#ifndef fit_phys_env_hpp_
#define fit_phys_env_hpp_

#include <LatAnalyze/MatSample.hpp>
#include <LatAnalyze/Model.hpp>
#include <LatAnalyze/XYSampleData.hpp>

class FitEnv
{
public:
    // fit variable info
    struct VarInfo
    {
        double       physVal;
        int          dim;
        Latan::Index index;
    };
    // fitted quantity info
    struct QuInfo
    {
        Latan::DoubleModel model;
        int                dim;
        Latan::Index       index;
    };
    // ensemble
    struct Ensemble
    {
        unsigned int  nT, nL;
    };
    // point
    struct Point
    {
        bool                               isActive{true};
        const Ensemble                     *ensemble{nullptr};
        std::map<std::string, std::string> fileName;
        std::vector<Latan::Index>          coord;
    };
    // data container
    struct Data
    {
        std::string    fileName;
        Latan::DSample value;
    };
    // table types
    typedef std::vector<std::vector<Data>> DataTable;
    typedef std::map<std::string, std::map<std::string, unsigned int>>
            IndexTable;
public:
    FitEnv(void)          = default;
    virtual ~FitEnv(void) = default;
    void                reset(void);
    Latan::Index        getVarIndex(const std::string name);
    std::string         getVarName(const Latan::Index i);
    Latan::Index        getQuIndex(const std::string name);
    std::string         getQuName(const Latan::Index i);
    Latan::DVec         getPhyPt(void);
    std::vector<const Latan::DoubleModel *> getModels(void);
    void                parseXml(const std::string paramFileName);
    std::string         macroSubst(const std::string str) const;
    void                load(void);
    Latan::XYSampleData generateData(const bool phyUnits, const bool corr);
    friend std::ostream & operator<<(std::ostream &out, FitEnv &f);
private:
    Latan::Index                       nSample_;
    std::string                        scale_;
    std::vector<unsigned int>          nT_, nL_;
    DataTable                          varData_, quData_;
    IndexTable                         varIndex_, quIndex_;
    std::map<std::string, VarInfo>     variable_;
    VarInfo                            *scaleVar_{nullptr};
    std::vector<std::string>           varName_;
    std::vector<double>                varScalePow_;
    std::map<std::string, QuInfo>      quantity_;
    std::vector<std::string>           quName_;
    std::map<std::string, Ensemble>    ensemble_;
    std::vector<Point>                 point_;
    std::map<std::string, std::string> macro_;
};

std::ostream & operator<<(std::ostream &out, FitEnv &f);

#endif // fit_phys_env_hpp_
