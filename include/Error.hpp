#include <exception>

class MultipleDefinitionLocException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Multiple definitions of the same location variable.\n";
		}
	};

	class FormatException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Configuration file is not formatted correctly.\n";
		}
	};

	class BadInputException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Bad input in configuration file.\n";
		}
	};

	class NoLocationException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: No location in configuration file.\n";
		}
	};

	class BadServerFormatException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Server block is not formatted correctly.\n";
		}
	};

	class MultipleDefinitionSerException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Multiple definitions of the same server variable.\n";
		}
	};

    class FileOpenException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Could not open configuration file.\n";
		}
	};

	class SemiColonException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Missing semi-colon in configuration file.\n";
		}
	};

	class BracketException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Bracket syntax is wrong.\n";
		}
	};

	class NoServerException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: No server in configuration file.\n";
		}
	};

	class NegativeValueException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Negative value in configuration file.\n";
		}
	};

	class NotAnIntException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Not an integer in configuration file.\n";
		}
	};

	class ClientMaxBodySizeException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Client_max_body_size is set, but is not a number or is smaller than 0.\n";
		}
	};

	class ErrorCodeException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Error code is not a number or is smaller than 0.\n";
		}
	};

	class DoubleErrorException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: The same error code is defined multiple times.\n";
		}
	};

	class PortException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Port is not a number or is smaller than 0.\n";
		}
	};

	class OpeningBracketException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Opening bracket is missing.\n";
		}
	};

	class ClosingBracketException : public std::exception
	{
		virtual const char *what() const throw()
		{
			return "Error: Closing bracket is missing.\n";
		}
	};