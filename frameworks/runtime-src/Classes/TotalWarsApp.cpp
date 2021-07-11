#include "TotalWarsApp.h"
#include "filesys.h"
#include "cocos2d.h"

USING_NS_CC;

TotalWarsApp::TotalWarsApp()
{

}

TotalWarsApp::~TotalWarsApp()
{

}

bool TotalWarsApp::init()
{
	return BaseApp::init();
}

std::string TotalWarsApp::getResPath()
{
	static std::string resPath;
	if (resPath.empty())
	{
		resPath = FileUtils::getInstance()->fullPathForFilename("temp");
		resPath = fs::GetDirectoryFromPath(resPath.c_str());
	}

	return resPath;
}

