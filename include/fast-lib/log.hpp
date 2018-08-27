#ifndef FAST_LOG_HPP
#define FAST_LOG_HPP

namespace fast {
	namespace log {
		/**
		 * \brief Logging class used as a sink for disabled logging.
		 *
		 * Use FASTLIB_LOG_INIT to initilize the logs.
		 * FASTLIB_LOG_SET_LEVEL may be used to set an urgency level for a log in local scope (e.g. in a function).
		 * FASTLIB_LOG_SET_LEVEL_GLOBAL sets an urgency level in global scope (e.g. at the top of the file).
		 * By setting the urgency level for a log only messages with equally or more urgent levels are actually logged while the other messages are discarded.
		 * Use FASTLIB_LOG(var, lvl) as a stream sink for logging: `FASTLIB_LOG(mylog, trace) << "Important log msg!";`
		 * If FASTLIB_ENABLE_LOGGING is defined, logging is enabled, else there is no logging done.
		 */
		class Dev_null
		{
		};
		extern Dev_null dev_null;
		template<typename T> Dev_null & operator<<(Dev_null &dest, T)
		{
			return dest;
		}
	}
}

#ifdef FASTLIB_ENABLE_LOGGING
	#include <spdlog/spdlog.h>

	#define FASTLIB_LOG_INIT(var, name) namespace fast { namespace log {auto var = spdlog::stdout_logger_mt(name); } }

	#define FASTLIB_LOG_SET_LEVEL(var, lvl) (fast::log::var->set_level(spdlog::level::lvl))

	#define FASTLIB_LOG_SET_LEVEL_GLOBAL(var, lvl) \
		namespace fast { namespace log { \
			struct Level_setter_##var { \
				Level_setter_##var (){ \
					fast::log::var->set_level(spdlog::level::lvl); \
				} \
			}; \
			Level_setter_##var level_setter_##var;\
		}}

	#define FASTLIB_LOG(var, lvl) (fast::log::var->lvl())
#else
	#define FASTLIB_LOG_INIT(var, name)
	#define FASTLIB_LOG_SET_LEVEL(var, lvl)
	#define FASTLIB_LOG_SET_LEVEL_GLOBAL(var, lvl)
	#define FASTLIB_LOG(var, lvl) fast::log::dev_null
#endif

#endif
