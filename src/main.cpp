#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;

// Global because fuck it
GameLevelManager* lvl1 = GameLevelManager::sharedState();
GameLevelManager* lvl2 = GameLevelManager::sharedState();

class $modify(MyMenuLayer, MenuLayer) {

	bool init() {

		// Loads levels on menu initalization cuz I needa figure out how to properly do this because poor docs and nobody gives answers
		lvl1->downloadLevel(Mod::get()->getSettingValue<int64_t>("levelID1"), false);
		lvl1->downloadLevel(Mod::get()->getSettingValue<int64_t>("levelID2"), false);

		if (!MenuLayer::init()) {
			return false;
		}

		auto levelButton1 = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("diffIcon_00_btn_001.png"),
			this,
			menu_selector(MyMenuLayer::onLevelButton)
		);

		auto levelButton2 = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::createWithSpriteFrameName("diffIcon_auto_btn_001.png"),
			this,
			menu_selector(MyMenuLayer::onLevelButton2)
		);

		levelButton1->setID("level-button-1"_spr);
		levelButton2->setID("level-button-2"_spr);
		
		auto menu = this->getChildByID("right-side-menu");

		menu->addChild(levelButton2);
		menu->addChild(levelButton1);

		menu->updateLayout();

		return true;
	}

	void onLevelButton(CCObject*) {
		GJGameLevel* level = lvl1->getSavedLevel(Mod::get()->getSettingValue<int64_t>("levelID1"));

		auto playScene = PlayLayer::scene(level, false, false);

		auto transition = CCTransitionFade::create(0.5f, playScene);
		CCDirector::sharedDirector()->pushScene(transition);
	}

	void onLevelButton2(CCObject*) {
		GJGameLevel* level = lvl2->getSavedLevel(Mod::get()->getSettingValue<int64_t>("levelID2"));

		auto playScene = PlayLayer::scene(level, false, false);

		auto transition = CCTransitionFade::create(0.5f, playScene);
		CCDirector::sharedDirector()->pushScene(transition);
	}
};

class $modify(MyInfoLayer, LevelInfoLayer) {


	bool init(GJGameLevel * p0, bool p1) {

		if (!LevelInfoLayer::init(p0, p1)) {
			return false;
		}

		auto menu = this->getChildByID("left-side-menu");
		
		if (menu) {

			auto btn = CCMenuItemSpriteExtra::create(CircleButtonSprite::createWithSpriteFrameName("diffIcon_auto_btn_001.png"), 
			this, 
			menu_selector(MyInfoLayer::onSaveLevelButton));

			btn->setID("save-level-btn"_spr);
			
			menu->addChild(btn);
			menu->updateLayout();
		}

		return true;
	}

	// Saves the levels ID to the settings value
	void onSaveLevelButton(CCObject * sender) {

		int64_t levelID = this->m_level->m_levelID;

		geode::createQuickPopup(
			"Save Level",
			"Save this level as primary or secondary menu screen level?",
			"Primary", "Secondary",
			[levelID](auto, bool secondary) {
				if (secondary) {
					Mod::get()->setSettingValue("levelID2", levelID);
				}
				else {
					Mod::get()->setSettingValue("levelID1", levelID);
				}
			}
		);
	}
};
