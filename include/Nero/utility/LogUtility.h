////////////////////////////////////////////////////////////
// Nero Game Engine
// Copyright (c) 2016-2019 SANOU A. K. Landry
////////////////////////////////////////////////////////////
#ifndef LOGUTILITY_H_INCLUDED
#define LOGUTILITY_H_INCLUDED
////////////////////////////////////////////////////////////
//Definition of the logger macros
#define EXPAND(x) x
#define nero_log_info(message) LOG(INFO) << message
#define nero_log_level(message, level)(level == nero::Debug ? LOG(DEBUG) << message : (level == nero::Warning ? LOG(WARNING) << message : LOG(INFO) << message))
#define nero_log_info_if(message, condition) LOG_IF(condition, INFO) << message
#define nero_log_level_if(message, level, condition)(condition == false ? : (level == nero::Debug ? LOG(DEBUG) << message : (level == nero::Warning ? LOG(WARNING) << message : LOG(INFO) << message )))
#define GET_NERO_LOG_MACRO(_1,_2,NAME,...) NAME
#define _nero_log(...) GET_NERO_LOG_MACRO(__VA_ARGS__, nero_log_level, nero_log_info)(__VA_ARGS__)
#define GET_NERO_LOG_IF_MACRO(_1,_2,_3,NAME,...) NAME
#define _nero_log_if(...) GET_NERO_LOG_IF_MACRO(__VA_ARGS__, nero_log_level_if, nero_log_info_if)(__VA_ARGS__)
#define nero_log(...) EXPAND(_nero_log(__VA_ARGS__))
#define nero_log_if(...) EXPAND(_nero_log_if(__VA_ARGS__))
#define _s(value) nero::toString(value) + nero::toString(" ")
#define _ss(value) nero::toString(#value)
#define _se(value) #value + nero::toString(" = ") + nero::toString(value)
#define _sn(object)  nero::toString(object->toString())
////////////////////////////////////////////////////////////
#endif // LOGUTILITY_H_INCLUDED
