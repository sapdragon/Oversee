#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <algorithm>
#include <deque>
#include <any>
#include <mutex>
#include <shared_mutex>
#include <memory>
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <d3d9.h>

#pragma warning( disable : 4307 ) 
#pragma warning( disable : 4244 ) 

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "LibCurl.lib" )
#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "MinHook.lib" )
#pragma comment( lib, "ws2_32.lib")

#include "SDK/Utils/String/XorStr.hpp"
#include "SDK/Utils/String/FNV1A.hpp"
#include "SDK/Utils/MultiThread/Multithread.hpp"

#include "Interface/GUI/Raw/MenuIcons.hpp"
#include "Interface/GUI/Raw/SFUIDisplay.hpp"
#include "Interface/GUI/Raw/WeaponIcons.hpp"

#include "Data/IconFont.hpp"

#include "SettingsManager.hpp"
#include "SDK/Color/Color.hpp"
#include "SDK/Math/QAngle.hpp"
#include "SDK/Math/VMatrix.hpp"
#include "SDK/Math/Vector2D.hpp"
#include "SDK/Math/Vector4D.hpp"

#include "Libraries/MinHook/MinHook.h"