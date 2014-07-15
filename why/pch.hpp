#pragma once

#define WIN32_LEAN_AND_MEAN

#include <deque>
#include <map>
#include <tuple>
#include <stack>
#include <random>
#include <set>
#include <numeric>

#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/support/date_time.hpp>

BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(why_logger, boost::log::sources::logger_mt)

#define WHY_LOG() BOOST_LOG(why_logger::get())

#define BOOST_ASSERT_MSG_OSTREAM WHY_LOG()

#include <boost/assert.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/move/utility.hpp>
#include <boost/any.hpp>
#include <boost/timer/timer.hpp>
#include <boost/range/adaptor/reversed.hpp>

#include <ClanLib/core.h>
#include <ClanLib/application.h>
#include <ClanLib/display.h>
#include <ClanLib/d3d.h>
#include <ClanLib/swrender.h>
#include <ClanLib/gl.h>
#include <ClanLib/physics2d.h>
#include <ClanLib/sound.h> 

#include "why_real2int.hpp"