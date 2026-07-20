#include "pch.h"
#include "tickconfig.h"
#include <algorithm>

namespace wintertick::tickconfig {

	float MaxTickRate = 120.0f;

    std::filesystem::path GetDLLPath() {
        char path[MAX_PATH] = {};
        HMODULE dll = NULL;

        static int dummy = 0;

        if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&dummy, &dll)) {
            GetModuleFileNameA(dll, path, sizeof(path));
        }

        return std::filesystem::path(path).parent_path();
    }

    static void CreateTickConfig() {
        std::filesystem::path configPath = GetDLLPath() / "WinterTick.ini";

        if (!std::filesystem::exists(configPath)) {
            std::ofstream configFile(configPath);
            if (configFile.is_open()) {
                configFile << "##############################################################################################\n";
                configFile << "# WinterTick Made by HOKTempest\n";
                configFile << "# Set the max tick rate. Min: 0 (Uncapped TickRate) | Max: 500 | Default: 120\n\n";
                configFile << "# Reminder: VSYNC has to be disabled for larger tickrate caps to take effect.\n";
                configFile << "# You can disable VSYNC by going to Options -> Graphics Options -> Vertical Sync : OFF.\n";
                configFile << "##############################################################################################\n\n";
                configFile << "[SetMaxTickRate]\n";
                configFile << "MaxTickRate=120\n";
                configFile.close();
            }
        }
    }

	void LoadTickConfig() {
        CreateTickConfig();
		std::filesystem::path tickConfig = GetDLLPath() / "WinterTick.ini";
		int configTickRate = GetPrivateProfileIntA("SetMaxTickRate", "MaxTickRate", static_cast<int>(MaxTickRate), tickConfig.string().c_str());

        MaxTickRate = std::clamp((float)(configTickRate), 0.0f, 500.0f);
	}
}