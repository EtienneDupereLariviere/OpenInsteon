/*
 * Log.cpp
 *
 *  Created on: 2014-10-29
 *      Author: etienne
 */

#include "Log.h"
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sinks.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using namespace boost;

// Complete file sink type
typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink;

void init_file_collecting(boost::shared_ptr<file_sink> sink)
{
    sink->locked_backend()->set_file_collector(sinks::file::make_collector(
        keywords::target = "Logs",                      /*< the target directory >*/
        keywords::max_size = 16 * 1024 * 1024,          /*< maximum total size of the stored files, in bytes >*/
        keywords::min_free_space = 100 * 1024 * 1024    /*< minimum free space on the drive, in bytes >*/
    ));
}

void Log::open() {
	// Create a text file sink
	boost::shared_ptr<file_sink> sink(new file_sink(
			keywords::file_name = "openInsteon_%Y%m%d.log",
			keywords::rotation_size = 5 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(gregorian::greg_day(1)),
			keywords::format = "[%TimeStamp%]: %Message%"
	));

	// Set up where the rotated files will be stored
	init_file_collecting(sink);

	// Upon restart, scan the directory for files matching the file_name pattern
	sink->locked_backend()->scan_for_files();

	// Add the sink to the core
	logging::core::get()->add_sink(sink);
}

void Log::writeLine(Severity_level level, string message)
{
	boost::log::sources::severity_logger_mt<Severity_level> slg;
	BOOST_LOG_SEV(slg, level) << message;
}

