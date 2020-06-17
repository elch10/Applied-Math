#ifndef IPROBLEM_H
#define IPROBLEM_H

#include "IVector.h"
#include "ICompact.h"

class IProblem
{
public:
  enum InterfaceTypes
    {
        INTERFACE_0,
        DIMENSION_INTERFACE_IMPL
    };
  //args - x , params - coef
    virtual RESULT_CODE goalFunction(IVector const* args, IVector const* params, double& res) const = 0;
    virtual RESULT_CODE goalFunctionByArgs(IVector const*  args, double& res) const = 0;
    virtual size_t getArgsDim() const = 0;
    virtual size_t getParamsDim() const = 0;

    virtual RESULT_CODE setParams(IVector const* params) = 0;
    virtual bool isCompactValid(ICompact const * const & compact) const = 0;

protected:
    virtual ~IProblem(){}

    IProblem() = default;

private:
    /* no way default copy/assignment */
    IProblem(const IProblem& other) = delete;
    void operator=(const IProblem& other) = delete;

};


#endif // IPROBLEM_H
