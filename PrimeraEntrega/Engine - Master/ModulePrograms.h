#ifndef __ModulePrograms_h__
#define __ModulePrograms_h__

#include "Module.h"

class ModulePrograms : public Module
{
public:
    ModulePrograms();
    ~ModulePrograms();

	bool Init   ();
    bool CleanUp();

private:
    char* LoadFile(const char* file_name);
    bool  Compile(unsigned id, char* data);

public:
    unsigned def_program = 0;
};

#endif /* __ModulePrograms_h__ */
