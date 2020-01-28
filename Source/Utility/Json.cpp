#include <string>
#include <fstream>
#include <sstream>
#include "Utility/Json.hpp"

namespace WeilanEngine
{
bool Utility::ReadJsonFromFile(const std::string &path, Json &out)
{
	std::ifstream ifs;
	ifs.open(path);
	if (ifs.is_open())
	{
		std::ostringstream oss;
		oss << ifs.rdbuf();
		out = Json::parse(oss.str());
		return true;
	}
	ifs.close();
	return false;
}

bool Utility::WriteJsonToFile(const std::string &path, const Json &in)
{
	std::ofstream ofs;
	ofs.open(path);
	if (ofs.good())
	{
		ofs << in.dump(4); //indent with level 4
		ofs.close();
		return true;
	}
	else
	{
		ofs.open(path, std::ios_base::app);
		if (ofs.good())
		{
			ofs << in.dump(4); //indent with level 4
			ofs.close();
		}
		else
		{
			return false;
		}

		return true;
	}

	ofs.close();
	return false;
}
} // namespace WeilanEngine