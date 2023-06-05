#include "vtk.hpp"
#include "helpers.hpp"

namespace app 
{
    std::string vtk::info()
    {
        return std::string( "vtk { length: " + std::to_string(len) +
                            ", descriminator: " + utils::descr_stringify((api::source)descrim) +
                            ", tlvs: \n{\n" + tlvs_to_string() + "}");
    }

    std::string vtk::tlvs_to_string()
    {
        std::string res;
        for(auto& it : tlvs)
            res += "\t" + it.info() + "\n"; 
        return res;
    }
}