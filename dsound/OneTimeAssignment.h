#pragma once

template <typename T>
struct OneTimeAssignment
{
    inline OneTimeAssignment() { isAssigned = false; }

    inline const T& get()                { return data; }
    inline bool     canUse()             { return (isAssigned == true); }
    inline bool     canAssign()          { return (isAssigned == false); }
    inline void     postAssignment()     { isAssigned = true; }
    inline void     assign(const T& src) { data = src; postAssignment(); }

    T    data;
private:
    bool isAssigned;
};