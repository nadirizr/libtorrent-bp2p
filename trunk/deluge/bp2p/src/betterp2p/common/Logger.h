
#ifndef COMMON_LOGGER_H_
#define COMMON_LOGGER_H_

#include <log4cpp/FileAppender.hh>
#include <log4cpp/Category.hh>

#include <boost/noncopyable.hpp>

#include <string>

namespace BT2 {

/**
 * A Logger with basic logging mechanizem.
 * singleton.
 */
class Logger: public boost::noncopyable {
public:
	/**
	 * get category for given name.
	 * @return		a category logger for you with the given name
	 */
	static log4cpp::Category& getLog(const std::string& name);

	~Logger() { }
private:
	/**
	 * private ctor.
	 * for singleton
	 */
	Logger();
	static Logger* m_instance;

	static Logger* getInstance()
	{
		if (NULL == m_instance)
			m_instance = new Logger();
		return m_instance;
	}

public:
	log4cpp::Appender* m_appender;
};

}
#endif /* COMMON_LOGGER_H_ */
