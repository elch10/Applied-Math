#ifndef IBROCKER_H
#define IBROCKER_H
#include "ILogger.h"
#include "RC.h"

class IBrocker
{
public:
    enum InterfaceTypes
    {
        INTERFACE_0,
        DIMENSION_INTERFACE_IMPL
    };

    enum class Type
    {
        PROBLEM,
        SOLVER,
        DIMENSION_TYPE
    };


    virtual Type getType() const = 0;
    virtual void* getInterfaceImpl(Type type) const = 0;
    virtual void release() = 0; // harakiri

protected:
    /*dtor*/
    virtual ~IBrocker(){}

    IBrocker() = default;

private:
    /* no way copy/assignment */
    IBrocker(const IBrocker& other) = delete;
    void operator=(const IBrocker& other) = delete;
};


#endif // IBROCKER_H
