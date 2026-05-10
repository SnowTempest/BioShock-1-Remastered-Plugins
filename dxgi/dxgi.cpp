#include "pch.h"
#include "proxy.h"
#include "util.h"

#pragma comment(linker,"/export:ApplyCompatResolutionQuirking=C:\\Windows\\SysWOW64\\dxgi.ApplyCompatResolutionQuirking,@1")
#pragma comment(linker,"/export:CompatString=C:\\Windows\\SysWOW64\\dxgi.CompatString,@2")
#pragma comment(linker,"/export:CompatValue=C:\\Windows\\SysWOW64\\dxgi.CompatValue,@3")
#pragma comment(linker,"/export:CreateDXGIFactory=C:\\Windows\\SysWOW64\\dxgi.CreateDXGIFactory,@12")
#pragma comment(linker,"/export:CreateDXGIFactory1=C:\\Windows\\SysWOW64\\dxgi.CreateDXGIFactory1,@10")
#pragma comment(linker,"/export:CreateDXGIFactory2=C:\\Windows\\SysWOW64\\dxgi.CreateDXGIFactory2,@11")
#pragma comment(linker,"/export:DXGID3D10CreateDevice=C:\\Windows\\SysWOW64\\dxgi.DXGID3D10CreateDevice,@13")
#pragma comment(linker,"/export:DXGID3D10CreateLayeredDevice=C:\\Windows\\SysWOW64\\dxgi.DXGID3D10CreateLayeredDevice,@14")
#pragma comment(linker,"/export:DXGID3D10GetLayeredDeviceSize=C:\\Windows\\SysWOW64\\dxgi.DXGID3D10GetLayeredDeviceSize,@15")
#pragma comment(linker,"/export:DXGID3D10RegisterLayers=C:\\Windows\\SysWOW64\\dxgi.DXGID3D10RegisterLayers,@16")
#pragma comment(linker,"/export:DXGIDeclareAdapterRemovalSupport=C:\\Windows\\SysWOW64\\dxgi.DXGIDeclareAdapterRemovalSupport,@17")
#pragma comment(linker,"/export:DXGIDisableVBlankVirtualization=C:\\Windows\\SysWOW64\\dxgi.DXGIDisableVBlankVirtualization,@18")
#pragma comment(linker,"/export:DXGIDumpJournal=C:\\Windows\\SysWOW64\\dxgi.DXGIDumpJournal,@4")
#pragma comment(linker,"/export:DXGIGetDebugInterface1=C:\\Windows\\SysWOW64\\dxgi.DXGIGetDebugInterface1,@19")
#pragma comment(linker,"/export:DXGIReportAdapterConfiguration=C:\\Windows\\SysWOW64\\dxgi.DXGIReportAdapterConfiguration,@20")
#pragma comment(linker,"/export:PIXBeginCapture=C:\\Windows\\SysWOW64\\dxgi.PIXBeginCapture,@5")
#pragma comment(linker,"/export:PIXEndCapture=C:\\Windows\\SysWOW64\\dxgi.PIXEndCapture,@6")
#pragma comment(linker,"/export:PIXGetCaptureState=C:\\Windows\\SysWOW64\\dxgi.PIXGetCaptureState,@7")
#pragma comment(linker,"/export:SetAppCompatStringPointer=C:\\Windows\\SysWOW64\\dxgi.SetAppCompatStringPointer,@8")
#pragma comment(linker,"/export:UpdateHMDEmulationStatus=C:\\Windows\\SysWOW64\\dxgi.UpdateHMDEmulationStatus,@9")

namespace fs = std::filesystem;

namespace dxgi::proxy {

	HMODULE dxgi = nullptr;
	const util::ThreadFreezeGuard suspender{};

	static std::vector<HMODULE> g_plugins;
	static std::wofstream g_logFile;

	static void InitLogFile(const fs::path& logPath) {
		g_logFile.open(logPath, std::ios::out | std::ios::trunc);
	}

	static std::wstring GetTimestamp() {
		std::time_t t = std::time(nullptr);
		std::tm tm{};

		localtime_s(&tm, &t);
		std::wostringstream timeSnapShot;

		timeSnapShot << std::put_time(&tm, L"[%Y-%m-%d %H:%M:%S]");

		return timeSnapShot.str();
	}

	static void Log(const std::wstring& msg) {
		std::wstring line = GetTimestamp() + L" " + msg;

		if (g_logFile.is_open()) {
			g_logFile << line << std::endl;
		}
	}

	static void LoadPlugins() {
		wchar_t exePath[MAX_PATH];

		GetModuleFileName(NULL, exePath, MAX_PATH);

		fs::path baseDir = fs::path(exePath).parent_path();
		fs::path pluginDir = baseDir / L"BioPlugins";
		fs::path logDir = baseDir / L"BioPlugins" / L"Plugins.log";

		if (!fs::exists(pluginDir))
			fs::create_directory(pluginDir);

		InitLogFile(logDir);

		Log(L"---------------------------------------");
		Log(L"BioPlugins loader initialized.");
		Log(L"Plugin directory: " + pluginDir.wstring());

		int pluginCount = 0;

		for (auto& entry : fs::directory_iterator(pluginDir)) {
			if (entry.path().extension() != L".dll")
				continue;

			std::wstring pluginName = entry.path().filename().wstring();

			HMODULE hMod = LoadLibraryW(entry.path().c_str());

			if (hMod) {
				g_plugins.push_back(hMod);
				Log(L"Loaded plugin: " + pluginName);
			}
			else {
				DWORD err = GetLastError();
				std::wostringstream ss;
				ss << L"  >> FAILED to load: " << pluginName << L" (Error: " << err << L")";
				Log(ss.str());
			}

			pluginCount++;
		}

		if (pluginCount == 0) {
			Log(L"No plugins found within the BioPlugins folder.");
		}
		else {
			std::wostringstream ss;
			ss << L"Done. " << g_plugins.size() << L"/" << pluginCount << L" plugin(s) loaded successfully.";
			Log(ss.str());
		}

		Log(L"---------------------------------------");
	}

	static void UnloadPlugins() {
		for (HMODULE hMod : g_plugins) {
			FreeLibrary(hMod);
		}
		g_plugins.clear();
	}

	void init(HMODULE) {

		char sysPath[MAX_PATH];

		UINT wowPath = GetSystemWow64DirectoryA(sysPath, MAX_PATH);

		if (wowPath && wowPath < MAX_PATH) {
			strcat_s(sysPath, "\\dxgi.dll");
			dxgi = LoadLibraryA(sysPath);
		}
		else {
			return;
		}

		if (!dxgi) {
			return;
		}

		LoadPlugins();
	}

	void free() {
		UnloadPlugins();
		if (dxgi != nullptr) {
			FreeLibrary(dxgi);
			dxgi = nullptr;
		}
	}
}