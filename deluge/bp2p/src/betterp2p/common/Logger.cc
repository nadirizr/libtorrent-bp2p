
#include "Logger.h"

#include <iostream>

#include <log4cpp/OstreamAppender.hh>

namespace BT2 {

Logger* Logger::m_instance = NULL;

log4cpp::Category& Logger::getLog(const std::string& name) {
	// TODO: Change me!!! ugly patch in order to call Logger::Logger after all other static initializations.
	getInstance();
	// make the needed category
	log4cpp::Category& cat = log4cpp::Category::getInstance(name);
	return cat;
}

Logger::Logger() {
	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.setPriority(log4cpp::Priority::DEBUG);

	m_appender = new log4cpp::OstreamAppender("cerr", &std::cerr);
	m_appender->setLayout(new log4cpp::BasicLayout());

	root.removeAllAppenders();
	root.addAppender(m_appender);
}

}
