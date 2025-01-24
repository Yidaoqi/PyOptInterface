#include "pyoptinterface/clp_model.hpp"
#include "fmt/core.h"


namespace clp
{
#define B DYLIB_DECLARE
APILIST
#undef B

static DynamicLibrary lib;
static bool is_loaded = false;

bool is_library_loaded()
{
	return is_loaded;
}

bool load_library(const std::string &path)
{
	bool success = lib.try_load(path.c_str());
	if (!success)
	{
		return false;
	}

	DYLIB_LOAD_INIT;

#define B DYLIB_LOAD_FUNCTION
	APILIST
#undef B

	// We have to deal with Clp 1.17.0 where some functions are absent from the dynamic library
	{
		int major, minor;
		auto Clp_versionmajor_p =
		    reinterpret_cast<decltype(Clp_VersionMajor)>(_function_pointers["Clp_VersionMajor"]);
		auto Clp_versionminor_p =
		    reinterpret_cast<decltype(Clp_VersionMinor)>(_function_pointers["Clp_VersionMinor"]);
		
		if (Clp_versionmajor_p != nullptr && Clp_versionminor_p)
		{
			major = Clp_versionmajor_p();
			minor = Clp_versionminor_p();
			if (major == 1 && minor == 17)
			{
			
				// Now check there is no nullptr in _function_pointers
				_load_success = true;
				for (auto &pair : _function_pointers)
				{
					if (pair.second == nullptr)
					{
						fmt::print("function {} is not loaded correctly\n", pair.first);
						_load_success = false;
					}
				}

				if (_load_success)
				{
					/*DYLIB_SAVE_FUNCTION(GRBloadenvinternal);
					DYLIB_SAVE_FUNCTION(GRBemptyenvinternal);*/
				}
			}
		}
	}

	if (IS_DYLIB_LOAD_SUCCESS)
	{
#define B DYLIB_SAVE_FUNCTION
		APILIST
#undef B
		is_loaded = true;
		return true;
	}
	else
	{
		return false;
	}
}

}  // namespace clp
