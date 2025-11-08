#include "GameItem.hpp"
#include "Engine.hpp"

uint32_t GameItem::nbId = 0;

extern AppOptions options;

GameItem::GameItem(SDL_FPoint position) : id(nbId++) {
    this->position = position;
    this->renderableItem = NULL;

    bool canDelete = false;
}

GameItem::~GameItem() {
    if (this->renderableItem != NULL)
        this->renderableItem->canDelete = true;
}

void GameItem::setPosition(SDL_FPoint position) {
    this->position = position;
}

void GameItem::setRenderableItem(RenderableItem* renderableItem) {
    if (this->renderableItem != NULL)
        this->renderableItem->canDelete = true;    

    this->renderableItem = renderableItem;
    newItem(this->renderableItem);
}

RenderableItem* GameItem::getRenderableItem(void) {
    return this->renderableItem;
}


/**********************************************************************************************************************/

Popup::Popup(std::string title, std::string content, SDL_FPoint pos) : GameItem(pos) {
    this->currentDuration = 0;
    this->duration = 0;

    this->doRender = false;

    this->title = content;
    this->content = content;

    this->startTicks = 0;

    RenderableGroups* item = new RenderableGroups(this->position);

    item->addItem(new TextSprite(title, 24, {255,255,255,SDL_ALPHA_OPAQUE}));
    item->addItem(new TextSprite(content, 16, {255,255,255,SDL_ALPHA_OPAQUE}, {0.0, 26.0}));
    item->disable();

    this->setRenderableItem(item);
}

void Popup::process(Uint64 ticks) {
    if (this->doRender == false)
        return;

    if (this->startTicks == 0)
        this->startTicks = ticks;

    this->currentDuration = ticks - this->startTicks;

    if ((this->currentDuration > this->duration) && (this->duration != 0)) {
        this->canDelete = true;
        this->getRenderableItem()->disable();
    }
}


void Popup::show(void) {
    this->doRender = true;
    this->getRenderableItem()->enable();

    // reset
    this->currentDuration = 0;
    this->startTicks = 0;
}

void Popup::setcallback(GameItem* callback) {
    this->callback = callback;
}

void Popup::setDuration(int duration) {
    this->duration = duration;
}

int Popup::getDuration(void) {
    return this->duration;
}

/**********************************************************************************************************************/

Text::Text(std::string content, SDL_FPoint pos, SDL_Color color) : GameItem(pos) {
    this->content = content;
    TextSprite* item = new TextSprite(this->content, 16, {color.r, color.g, color.b, color.a}, this->position);
    this->setRenderableItem((RenderableItem*)item);
    this->getRenderableItem()->setEventHandler(this);
}

std::string Text::getText(void) {
    TextSprite* item = (TextSprite*)this->getRenderableItem();
    return item->getText();
}

void Text::setText(std::string content) {
    TextSprite* item = (TextSprite*)this->getRenderableItem();
    item->updateText(content);
}

/**********************************************************************************************************************/

TextArea::TextArea(std::string content, SDL_FPoint pos, SDL_Color color, std::string cursorContent) : Text(content, pos, color) {
    this->cursor = new TextSprite(cursorContent, 16, color, this->position);
    this->cursor->disable();

    newItem(this->cursor);

    this->lastTicks = 0;
    this->currentDuration = 0;

    this->getRenderableItem()->setEventHandler(this);

    this->cursorPosition = 0;
}

void TextArea::process(Uint64 ticks) {
    this->handleEvent();
    
    // TODO update cursor pos

    SDL_FRect textArea = this->getRenderableItem()->getArea();
    SDL_FPoint cursorPos;
    cursorPos.x = textArea.x + textArea.w;
    cursorPos.y = textArea.y;
    this->cursor->setPosition(cursorPos);

    int cursorDuration = 500;
    this->currentDuration = ticks - this->lastTicks;

    if (this->currentDuration > cursorDuration) {
        this->cursor->toggle();
        this->lastTicks = ticks;
    }
}

int check_utf8_char(std::string &utf, long i) {
    unsigned char check = utf[i] & 0xF0;

    switch (check)
    {
    case 0xC0:
        return bool((utf[i + 1] & 0x80) == 0x80) * 1;
    case 0xE0:
        return bool(((utf[i + 1] & 0x80) == 0x80 && 
                     (utf[i + 2] & 0x80) == 0x80)) * 2;
    case 0xF0:
        return bool(((utf[i + 1] & 0x80) == 0x80 && 
                     (utf[i + 2] & 0x80) == 0x80 && 
                     (utf[i + 3] & 0x80) == 0x80)) * 3;
    }
    return 0;
}

int getLastCharLenght(std::string string) {
    long size;
    std::string chr;

    for (long i = 0; i < string.size(); i++)
    {
    size = check_utf8_char(string, i);
    //size >= 0 && size <= 3, we need to add 1 for the full size
    chr = string.substr(i, size + 1);
    //we add this value to skip the whole character at once
    //hence the reason why we return full size - 1
    i += size;   
    }
    return size;
}

void TextArea::handleEvent(void) {
    this->mutex.lock();
    for(std::vector<Event>::iterator it = this->events.begin(); it != this->events.end(); ++it)
    {
        Event item = *it;
        // SDL_Log("TextArea::handleEvent(): Event handled: '%s' '%s'", getEventTypeName(item.type), SDL_GetScancodeName(item.key));

        switch (item.type)
        {
        case EventType::onTextInput: {
            TextSprite* textSprite = (TextSprite*)this->getRenderableItem();
            std::string content = textSprite->getText();
            content += item.text;
            textSprite->updateText(content);
        }
        case EventType::onKeyUp:
        case EventType::onKeyHold: {
            TextSprite* textSprite = (TextSprite*)this->getRenderableItem();
            std::string content = textSprite->getText();

            switch (item.key)
            {
            case '\b':{
                if (content.size() > 0) {
                    int lenght = getLastCharLenght(content) + 1;
                    content.erase(content.size()-lenght, lenght);
                }
                }break;
            case 127: 
                break;
            case '\t':
                content += "    ";
                break;
            case '\r':
                content += "\r";
                break;
            default:
                break; 
            }
            textSprite->updateText(content);
            break;
        }
        default:
            // SDL_Log("Unhandled: '%s'", getEventTypeName(item.type));
            break;
        }
    }
    this->clearEvent();
    this->mutex.unlock();
}

/**********************************************************************************************************************/

MenuItem::MenuItem() : GameItem() {

}

void MenuItem::setCallback(GameItem* callback) {
    this->callback = callback;
    this->setRenderableItem(new RenderableGroups());
}

void MenuItem::setOption(std::string option) {
    this->option = option;
}

void MenuItem::change(void) {
    if (this->option != "") {
        options.setOption({this->option, this->value});
    }

    if (this->callback != NULL) {
        Event event;
        event.emitter = this->getRenderableItem();
        event.type = EventType::menuItemUpdate;
        event.value = this->value;
        this->callback->receiveEvent(event);
    }
}

/**********************************************************************************************************************/

Button::Button(std::string content, SDL_FRect area, std::string option) : MenuItem() {
    this->setOption(option);
    
    SDL_FPoint position;
    position.x = area.x;
    position.y = area.y;

    RenderableGroups* group = new RenderableGroups();
    group->enable();
    group->setPosition(position);
    group->setLevel(10);
    group->setEventHandler(this);
    
    this->IDbg = group->addItem(new Sprite("background_menu.png"));
    this->IDtext = group->addItem(new TextSprite(content, 16, BLACK));
    this->IDfg = -1; 

    SDL_FRect bgArea = area;
    bgArea.x = bgArea.y = 0;
    group->getItem(this->IDbg)->setArea(bgArea);

    this->setRenderableItem((RenderableItem*)group);
}

void Button::process(Uint64 ticks) {
    this->handleEvent();


}

void Button::handleEvent(void) {
    this->mutex.lock();
    for(std::vector<Event>::iterator it = this->events.begin(); it != this->events.end(); ++it) {
        Event event = (Event)*it;
        switch (event.type)
        {
        case EventType::onMouseDown:
            ((TextSprite*)((RenderableGroups*)this->getRenderableItem())->getItem(this->IDtext))->setColor(WHITE);
            break;
        case EventType::onClick:
        case EventType::onDlbClick:
            ((TextSprite*)((RenderableGroups*)this->getRenderableItem())->getItem(this->IDtext))->setColor(BLACK);
            this->clicked = true;
            this->change();
            break;
        
        default:
            break;
        }
    }
    this->clearEvent();
    this->mutex.unlock();
}