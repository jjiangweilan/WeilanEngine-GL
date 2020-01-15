#pragma once
#include <json.hpp>

namespace WeilanEngine
{
using Json = nlohmann::json;
namespace Utility
{
bool ReadJsonFromFile(const std::string &path, Json &out);

/**
 * @brief Write to file, and create the file if it doesn't exist
 * 
 * @param path 
 * @param in 
 * @return true 
 * @return false 
 */
bool WriteJsonToFile(const std::string& path, const Json &in);
} // namespace Utility
} // namespace WeilanEngine