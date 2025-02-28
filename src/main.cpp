#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {

	bool init() {

		if (!MenuLayer::init()) return false;
		
		auto menu = this->getChildByID("right-side-menu");
		if (!menu) return true;

		auto levelButton1 = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("diffIcon_00_btn_001.png"),
			this, menu_selector(MyMenuLayer::onLevelButton)
		);

		auto levelButton2 = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("diffIcon_auto_btn_001.png"),
			this, menu_selector(MyMenuLayer::onLevelButton2)
		);

		levelButton1->setID("level-button-1"_spr);
		levelButton2->setID("level-button-2"_spr);

		menu->addChild(levelButton2);
		menu->addChild(levelButton1);

		menu->updateLayout();

		return true;
	}

	void onLevelButton(CCObject*) {
		MyMenuLayer::openLevel(Mod::get()->getSavedValue<int64_t>("levelID1", 73667628));
	}

	void onLevelButton2(CCObject*) {
		MyMenuLayer::openLevel(Mod::get()->getSavedValue<int64_t>("levelID2", 83724866));
	}

	void openLevel(const int64_t levelID) {
		if (levelID < 128) return MyMenuLayer::woahThereBuddy(fmt::format("{} is not a valid level ID.", levelID)); // reject lists and robtop levels
		GameLevelManager* glm = GameLevelManager::sharedState();
		glm->downloadLevel(levelID, false);
		if (!glm->hasDownloadedLevel(levelID)) return MyMenuLayer::woahThereBuddy(fmt::format("Unable to download level {}. Try again later.", levelID));
		GJGameLevel* level = glm->getSavedLevel(levelID);
		if (!level) return MyMenuLayer::woahThereBuddy(fmt::format("Unable to open level {}. Try again later.", levelID));
		auto playScene = PlayLayer::scene(level, false, false);
		if (utils::string::toLower(typeinfo_cast<CCLabelBMFont*>(playScene->getChildByID("debug-text"))->getString()) == "load failed!") {
			playScene->release(); // abort mission
			return MyMenuLayer::woahThereBuddy(fmt::format("Unable to play level {}. Try again later.", levelID));
		}
		auto transition = CCTransitionFade::create(0.5f, playScene);
		CCDirector::sharedDirector()->pushScene(transition);
	}

	void woahThereBuddy(const std::string& reason) {
		return FLAlertLayer::create("Uh oh!", reason, "OK")->show();
	}
};

class $modify(MyInfoLayer, LevelInfoLayer) {
	bool init(GJGameLevel * p0, bool p1) {

		if (!LevelInfoLayer::init(p0, p1)) return false;

		auto menu = this->getChildByID("left-side-menu");
		
		if (!menu) return true;

		auto btn = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("diffIcon_auto_btn_001.png"), 
			this, menu_selector(MyInfoLayer::onSaveLevelButton)
		);

		btn->setID("save-level-btn"_spr);
		
		menu->addChild(btn);
		menu->updateLayout();

		return true;
	}

	// Saves the levels ID to the settings value
	// note from raydeeux: maybe use flalertlayer inheritance for this?
	void onSaveLevelButton(CCObject*) {
		int64_t levelID = this->m_level->m_levelID;
		geode::createQuickPopup(
			"Save Level",
			"Save this level as primary or secondary menu screen level?\n\n<cy>To cancel, press ESC (PC/Mac) or the Back button on your three-button navigation bar (Android).</c>",
			"Primary", "Secondary",
			[levelID](auto, bool secondary) {
				if (secondary) Mod::get()->setSavedValue("levelID2", levelID);
				else Mod::get()->setSavedValue("levelID1", levelID);
			}
		);
	}
};