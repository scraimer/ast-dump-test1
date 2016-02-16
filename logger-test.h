#ifndef LOGGER_TEST_H__
#define LOGGER_TEST_H__

class LoggerEndLine { };

class Logger;

class ILogLine
{
	private:
		Logger& _owner;
		bool _cleared;

	public:
		ILogLine() = delete; // No default constructor
		ILogLine(ILogLine const &) = delete; // No copy constructor
		ILogLine(Logger& owner) : _owner(owner), _cleared(true)
		{
			//std::cout << "ILogLine::copyctor" << std::endl;
		}

		ILogLine& operator<<(char const * const string)
		{
			_cleared = false;
			return *this;
		}

		void operator<<(LoggerEndLine const &);

		void clear()
		{
			_cleared = true;
		}
};

class Logger
{
	private:
		ILogLine _line;

	public:
		Logger( ) : _line(*this) { }

		LoggerEndLine endl;
		ILogLine& operator<<(int const level)
		{
			return _line;
		}

		void flush(ILogLine& line)
		{
			line.clear();
		}
};

void ILogLine::operator<<(LoggerEndLine const &)
{
	_owner.flush(*this);
}

#endif // ifndef LOGGER_TEST_H__

