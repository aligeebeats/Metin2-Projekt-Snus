#include "debug_allocator.h"
#include "../../libthecore/src/stdafx.h"
#include "../../common/singleton.h"
#include "../../common/utils.h"
#include "../../common/service.h"

#include <algorithm>
#include <math.h>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <string>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#define isdigit iswdigit
#define isspace iswspace
#include "typedef.h"
#include "locale.hpp"
#include "event.h"
#include <float.h>
#define PASSES_PER_SEC(sec) ((sec) * passes_per_sec)

#ifdef _WIN32
	#ifndef M_PI
		#define M_PI    3.14159265358979323846
	#endif
#endif

#define IN
#define OUT
