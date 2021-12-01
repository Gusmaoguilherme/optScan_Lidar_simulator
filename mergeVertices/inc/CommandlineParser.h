
#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>
#include <vector>
#include <geogram/basic/line_stream.h>
#include <geogram/delaunay/LFS.h>
//argumentos de comando
namespace CommandlineParser
{
	// Default parameters
	class CommandlineArguments
	{
		public:
		std::string outputPath;
		int Number_scans;
		int processo_atual = 1;
		float tolerance;
		std::vector<std::string> meshFilesPath;
		std::string configFilePath;
		std::string dir_meshFilesPath;
		int format1, format2;
		char normalize = 'n'; // "yes(y) | no(n) - default"
		char ply = 'n'; // "yes(y) | no(n) - default"

	};

	bool Parse(const std::vector<std::string>& args, CommandlineArguments& arguments);
}
#endif