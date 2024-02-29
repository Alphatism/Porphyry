#include "ModItem.hpp"
#include <Geode/ui/GeodeUI.hpp>

bool BaseModItem::init() {
    if (!CCNode::init())
        return false;
    
    auto meta = this->getMetadata();

    m_logo = this->createModLogo();
    this->addChild(m_logo);

    m_title = CCLabelBMFont::create(meta.getName().c_str(), "bigFont.fnt");
    m_title->setAnchorPoint({ .0f, .5f });
    this->addChild(m_title);

    m_developers = CCMenu::create();
    m_developers->ignoreAnchorPointForPosition(false);
    m_developers->setAnchorPoint({ .0f, .5f });
    m_developers->setLayout(
        RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
    );
    this->addChild(m_developers);

    m_viewMenu = CCMenu::create();
    m_viewMenu->setAnchorPoint({ 1.f, .5f });
    m_viewMenu->setScale(.55f);
    
    auto viewBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("View", "bigFont.fnt", "GE_button_05.png"_spr, .8f),
        this, nullptr
    );
    m_viewMenu->addChild(viewBtn);

    m_viewMenu->setLayout(
        RowLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
    );
    this->addChildAtPosition(m_viewMenu, Anchor::Right, ccp(-10, 0));

    this->updateState();

    return true;
}

void BaseModItem::updateState() {
    m_developers->removeAllChildren();
    if (this->wantsRestart()) {
        m_developers->addChild(ButtonSprite::create("Restart Required", "goldFont.fnt", "black-square"_spr, .8f));
    }
    else {
        auto by = "By " + ModMetadata::formatDeveloperDisplayString(this->getMetadata().getDevelopers());
        auto developersBtn = CCMenuItemSpriteExtra::create(
            CCLabelBMFont::create(by.c_str(), "goldFont.fnt"),
            this, nullptr
        );
        m_developers->addChild(developersBtn);
    }
    m_developers->updateLayout();
}

void BaseModItem::updateSize(float width, bool big) {
    this->setContentSize({ width, big ? 40.f : 25.f });

    auto logoSize = m_obContentSize.height - 5;
    limitNodeSize(m_logo, { logoSize, logoSize }, 999, .1f);
    m_logo->setPosition(m_obContentSize.height / 2 + 5, m_obContentSize.height / 2);

    CCSize titleSpace {
        m_obContentSize.width / 2 - m_obContentSize.height,
        m_obContentSize.height / 2
    };
    m_title->setPosition(m_obContentSize.height + 10, m_obContentSize.height * .7f);
    limitNodeSize(m_title, titleSpace, 1.f, .1f);

    // Only limit developer size by height since we're setting the content width manually
    limitNodeSize(m_developers, ccp(9999, titleSpace.height * .8f), 1.f, .1f);
    m_developers->setPosition(m_obContentSize.height + 10, m_obContentSize.height * .3f);
    m_developers->setContentWidth(titleSpace.width / m_developers->getScale());
    m_developers->updateLayout();

    m_viewMenu->setContentWidth(m_obContentSize.width / 2 - 20);
    m_viewMenu->updateLayout();

    this->updateLayout();
}

bool InstalledModItem::init(Mod* mod) {
    m_mod = mod;

    if (!BaseModItem::init())
        return false;
    
    return true;
}

InstalledModItem* InstalledModItem::create(Mod* mod) {
    auto ret = new InstalledModItem();
    if (ret && ret->init(mod)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ModMetadata InstalledModItem::getMetadata() const {
    return m_mod->getMetadata();
}

CCNode* InstalledModItem::createModLogo() const {
    return geode::createModLogo(m_mod);
}

bool InstalledModItem::wantsRestart() const {
    return m_mod->getRequestedAction() != ModRequestedAction::None;
}

bool ServerModItem::init(server::ServerModMetadata const& metadata) {
    m_metadata = metadata;

    if (!BaseModItem::init())
        return false;
    
    if (metadata.featured) {
        m_checkmark = CCScale9Sprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
        m_checkmark->setContentSize({ 50, 38 });
        m_checkmark->setColor({ 255, 255, 120 });
        m_checkmark->setOpacity(45);

        auto tick = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
        m_checkmark->addChildAtPosition(tick, Anchor::Center);
        this->addChild(m_checkmark);
    }
    
    return true;
}

void ServerModItem::updateSize(float width, bool big) {
    BaseModItem::updateSize(width, big);

    if (m_checkmark) {
        auto size = m_title->getScaledContentSize();
        limitNodeSize(m_checkmark, ccp(100, size.height), 1.f, .1f);
        m_checkmark->setPosition(m_title->getPosition() + ccp(size.width + 10, 0));
    }
}

ServerModItem* ServerModItem::create(server::ServerModMetadata const& metadata) {
    auto ret = new ServerModItem();
    if (ret && ret->init(metadata)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

ModMetadata ServerModItem::getMetadata() const {
    return m_metadata.versions.front().metadata;
}

CCNode* ServerModItem::createModLogo() const {
    return createServerModLogo(m_metadata.id);
}

bool ServerModItem::wantsRestart() const {
    // todo: request restart after install
    return false;
}
