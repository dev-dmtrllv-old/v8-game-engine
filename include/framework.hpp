#ifndef NOVA_ENGINE_FRAMEWORK_PCH
#define NOVA_ENGINE_FRAMEWORK_PCH

#include <vector>
#include <set>
#include <stdio.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <limits>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <filesystem>
#include <stdarg.h>
#include <ctime>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <csignal>
#include <cstdlib>
#include <stdlib.h>
#include <cassert>
#include <atomic>
#include <optional>
#include <stack>
#include <coroutine>

#include <libgen.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>

#include <v8/v8.h>
#include <v8/libplatform/libplatform.h>

#include <GLFW/glfw3.h>

#pragma region DEFINES

#ifdef DEBUG
#define CHECK(expr, msg) { if(!expr) { Logger::get()->error(msg); return false; } }
#else
#define CHECK(expr, msg) if(!expr) return false;
#endif

#ifdef DEBUG
#define ON_DEBUG(expr) expr
#else
#define ON_DEBUG(expr)
#endif

#pragma endregion

#endif
