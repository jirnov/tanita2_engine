#pragma once

/* Global options and definitions */

// Uncomment this to enable memory leak detection
//#define ENABLE_MEMORY_LEAK_DETECTION

// Configuration file name
const const_string ConfigFileName = "config.info";

// Log file name
const const_string LogFileName = "worklog.htm";

/* Global definitions */

// Application executable file name
extern std::string ExecutableName;

// Type definition for generic object identifier (sequence, sound etc)
typedef unsigned long ID;

#include "Config_.h"
// Config singleton definition
typedef utility::singleton<ConfigInstance> Config;
#include "Input_.h"
// DirectInput singleton definition
typedef utility::singleton<DirectInputInstance> DirectInput;
