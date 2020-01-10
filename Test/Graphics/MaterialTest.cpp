#include <gtest/gtest.h>
#include "Global/KuangyeEngine.hpp"

TEST(Material, Initialization)
{
	auto engine = KuangyeEngine::EngineManager::GetKuangyeEngine();
	KuangyeEngine::Scene scene;
	engine->SetScene(&scene);
	engine->Start();
}