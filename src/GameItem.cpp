#include "GameItem.hpp"
#include "Engine.hpp"

uint32_t GameItem::nbId = 0;

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

    item->addItem(new TextSprite(title, "Inter-VariableFont.ttf", 24, {255,255,255,SDL_ALPHA_OPAQUE}));
    item->addItem(new TextSprite(content, "Inter-VariableFont.ttf", 16, {255,255,255,SDL_ALPHA_OPAQUE}, {0.0, 26.0}));
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
        this->renderableItem->disable();
    }
}


void Popup::show(void) {
    this->doRender = true;
    this->renderableItem->enable();

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

Text::Text(std::string content, SDL_FPoint pos) : GameItem(pos) {
    this->content = content;
    TextSprite* item = new TextSprite(this->content, "Inter-VariableFont.ttf", 16, {255, 255, 255, SDL_ALPHA_OPAQUE}, this->position);
    this->setRenderableItem((RenderableItem*)item);
}

std::string Text::getText(void) {
    TextSprite* item = (TextSprite*)this->renderableItem;
    return item->getText();
}

void Text::setText(std::string content) {
    TextSprite* item = (TextSprite*)this->renderableItem;
    item->updateText(content);
}

/**********************************************************************************************************************/

TextArea::TextArea(std::string content, SDL_FPoint pos, std::string cursorContent) : Text(content, pos) {
    this->cursor = new TextSprite(cursorContent, "Inter-VariableFont.ttf", 16, {255, 255, 255, SDL_ALPHA_OPAQUE}, this->position);
    this->cursor->disable();

    newItem(this->cursor);

    this->lastTicks = 0;
    this->currentDuration = 0;

    this->renderableItem->setEventHandler(this);
}

void TextArea::keyPressed(SDL_Scancode key) {
    
}

void TextArea::process(Uint64 ticks) {
    this->handleEvent();
    int cursorPosition = 3;
    // TODO update cursor pos
    SDL_FRect textArea = this->renderableItem->getArea();
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
    for(std::vector<Event>::iterator it = this->event.begin(); it != this->event.end(); ++it)
    {
        Event item = *it;
        // SDL_Log("TextArea::handleEvent(): Event handled: '%s' '%s'", getEventTypeName(item.type), SDL_GetScancodeName(item.key));

        switch (item.type)
        {
        case EventType::onTextInput: {
            TextSprite* textSprite = (TextSprite*)this->renderableItem;
            std::string content = textSprite->getText();
            content += item.text;
            textSprite->updateText(content);
        }
        case EventType::onKeyUp:
        case EventType::onKeyHold: {
            TextSprite* textSprite = (TextSprite*)this->renderableItem;
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


        // remove event
        it = this->event.erase(it);
        it--;   // get previous iterator
    }
    this->mutex.unlock();
}