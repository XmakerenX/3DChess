#ifndef  _LISTBOXUI_CPP
#define  _LISTBOXUI_CPP

#include "ListBoxUI.h"
#include "DialogUI.h"
#include <algorithm>

//-----------------------------------------------------------------------------
// Name : ListBoxUI (constructor)
//-----------------------------------------------------------------------------
template<class T>
ListBoxUI<T>::ListBoxUI(DialogUI *pParentDialog, int ID, int x, int y, int width, int height, bool multiSelection)
    :ControlUI(pParentDialog, ID, x, y, width, height)
{
    m_type = ControlUI::LISTBOX;
    m_isMultiSelection = multiSelection;
    m_nSelected = -1;

    m_ScrollBar.setSize(16, m_height);
}

//-----------------------------------------------------------------------------
// Name : ListBoxUI (constructor from InputFile)
//-----------------------------------------------------------------------------
template<class T>
ListBoxUI<T>::ListBoxUI(std::istream& inputFile)
    :ControlUI(inputFile), m_ScrollBar(inputFile)
{
    m_type = ControlUI::LISTBOX;
    m_nSelected = -1;

    inputFile >> m_isMultiSelection;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
    int itemToLoadNum;
    inputFile >> itemToLoadNum;
    inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line

    for (int i = 0; i < itemToLoadNum; i++)
    {
        std::string itemText;
        T data;
        std::getline(inputFile, itemText, '|');
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
        inputFile >> data;
        inputFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skips to next line
        AddItem(itemText, data);
    }
}

//-----------------------------------------------------------------------------
// Name : ListBoxUI (destructor)
//-----------------------------------------------------------------------------
template<class T>
ListBoxUI<T>::~ListBoxUI(void)
{}

//-----------------------------------------------------------------------------
// Name : onInit()
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::onInit()
{
    return m_pParentDialog->initControl( &m_ScrollBar );
}

//-----------------------------------------------------------------------------
// Name : handleMouseEvent()
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    if (!m_bEnabled || !m_bVisible)
        return false;
    
    // Let the scroll bar handle it first.
    if( m_ScrollBar.handleMouseEvent(event, modifierStates))
        return true;

    switch(event.type)
    {
    case MouseEventType::LeftButton:
    case MouseEventType::DoubleLeftButton:
    {
        if (event.down)
        {
            if ( Pressed(event.cursorPos, modifierStates, event.timeStamp) )
                return true;
        }
    }break;

    case MouseEventType::ScrollVert:
    {
        return Scrolled(event.nLinesToScroll);
    }break;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Pressed()
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::Pressed(Point pt, const ModifierKeysStates &modifierStates, double timeStamp)
{
    if( !m_rcItembox.isPointInRect(pt) )
        return false;

    // Determine which item has been selected
    for(int i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++ )
    {
        Item<T>& item = m_Items[i];

        if( item.bVisible && item.rcActive.isPointInRect(pt) )
        {
            if (m_isMultiSelection && modifierStates.bShift)
            {
                    if (m_nSelected != -1)
                    {
                        for (int j = 0; j < m_selectedItems.size(); j++)
                            m_Items[m_selectedItems[j]].bSelected = false;
                        m_selectedItems.clear();

                        int begin = std::min(m_nSelected, i);
                        int end = std::max(m_nSelected, i);

                        int k;
                        for (k = begin; k <= end; k++)
                        {
                            m_Items[k].bSelected = true;
                            m_selectedItems.push_back(k);
                        }
                        if (k != begin)
                            m_listboxChangedig(this);
                        break;
                    }
            }
            else
            {
                if (!modifierStates.bCtrl)
                {
                    for (int j = 0; j < m_selectedItems.size(); j++)
                        m_Items[m_selectedItems[j]].bSelected = false;
                    m_selectedItems.clear();
                }
            }

            m_nSelected = i;
            m_Items[i].bSelected = !m_Items[i].bSelected;
            if (m_Items[i].bSelected)
                m_selectedItems.push_back(i);
            else
            {
                auto selPos = std::find(m_selectedItems.begin(), m_selectedItems.end(), i);
                if (selPos != m_selectedItems.end())
                    m_selectedItems.erase(selPos);
            }
            m_listboxChangedig(this);

            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Scrolled
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::Scrolled( int nScrollAmount)
{
    if (m_bMouseOver)
    {
        m_ScrollBar.Scroll( -nScrollAmount );
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name : Highlight()
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::Highlight(Point mousePoint)
{
    if( m_rcItembox.isPointInRect(mousePoint))
    {
        // Determine which item has been selected
        for(GLuint i = 0; i < m_Items.size(); i++)
        {
            Item<T>& item = m_Items[i];
            if( item.bVisible && item.rcActive.isPointInRect(mousePoint) )
            {
                if (m_selectedItems.size() > 0)
                    m_Items[m_selectedItems.back()].bSelected = false;
                m_selectedItems.pop_back();
                item.bSelected = true;
                m_selectedItems.push_back(i);
            }
        }
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// Name : ConnectToItemDoubleClick
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::ConnectToItemDoubleClick(const typename signal_listbox::slot_type& subscriber)
{
    m_itemDoubleClickSig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : ConnectToListboxChanged
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::ConnectToListboxChanged (const typename signal_listbox::slot_type& subscriber)
{
    m_listboxChangedig.connect(subscriber);
}

//-----------------------------------------------------------------------------
// Name : Render
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::Render(Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp)
{
    // check that there is actual fonts
    if (!m_bVisible ||  m_elementsFonts.size() == 0)
        return;

    Point dialogPos = calcPositionOffset();

    m_ScrollBar.Render(sprites, topSprites, timeStamp);

    renderRect(sprites[NORMAL], m_rcItembox, m_elementsGFX[MAIN].texture, m_elementsGFX[MAIN].rcTexture, WHITE_COLOR, dialogPos);

    int curY = m_rcItemboxText.top;
    int nRemainingHeight =  m_rcItemboxText.getHeight();

    // render all item within the dropdown box;
    for (int i = m_ScrollBar.GetTrackPos(); i < m_Items.size(); i++)
    {
        Item<T>& item = m_Items[i];

        Point strDim = m_elementsFonts[0].font->calcTextRect(item.strText);
        strDim.y += 4;
        // Make sure there's room left in the dropdown
        nRemainingHeight -= strDim.y;
        if( nRemainingHeight < 0 )
        {
            item.bVisible = false;
            continue;
        }

        item.rcActive = Rect(m_rcItemboxText.left, curY, m_rcItemboxText.right, curY + strDim.y);
        curY += strDim.y;
        item.bVisible = true;

        if(item.bSelected)
        {
            renderRect(sprites[NORMAL], item.rcActive, m_elementsGFX[SELECTION].texture, m_elementsGFX[SELECTION].rcTexture, WHITE_COLOR, dialogPos);
            renderText(sprites[TEXT], m_elementsFonts[0].font, item.strText, WHITE_COLOR, item.rcActive, dialogPos, mkFont::TextFormat::Center);
        }
        else
        {
            renderText(sprites[TEXT], m_elementsFonts[0].font, item.strText, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), item.rcActive, dialogPos, mkFont::TextFormat::Center);
        }
    }
}

//-----------------------------------------------------------------------------
// Name : UpdateRects
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::UpdateRects()
{
    ControlUI::UpdateRects();

    m_rcItembox = m_rcBoundingBox;
    m_rcItembox.right -= m_ScrollBar.getWidth();

    m_rcItemboxText = m_rcItembox;
    m_rcItemboxText.offset(0, (int)(0.05 * m_rcItembox.getHeight()));

    m_ScrollBar.setLocation( m_rcItembox.right, m_rcItembox.top + 2 );
    m_ScrollBar.setSize( m_ScrollBar.getWidth(), m_rcItembox.getHeight() - 2 );
    if (m_elementsFonts.size() > 0)
    {
        m_ScrollBar.SetPageSize( (m_rcItemboxText.getHeight())  /
                                (m_elementsFonts[0].font->getFontSize() + 0.1875*m_elementsFonts[0].font->getFontSize() + 4));
    }

    if (m_selectedItems.size() > 0)
        m_ScrollBar.ShowItem(m_selectedItems.back());
}

//-----------------------------------------------------------------------------
// Name : CanHaveFocus
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::CanHaveFocus()
{
    return ( m_bVisible && m_bEnabled );
}

//-----------------------------------------------------------------------------
// Name : SaveToFile
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::SaveToFile(std::ostream& SaveFile)
{
    ControlUI::SaveToFile(SaveFile);
    m_ScrollBar.SaveToFile(SaveFile);

    SaveFile << m_isMultiSelection << "| Listbox isMultiSelection\n";
    SaveFile << m_Items.size() << "| Listbox Items Amount\n";

    for (GLuint i = 0; i < m_Items.size(); i++)
    {
        SaveFile << m_Items[i].strText << "| Listbox Item "<< i <<" Text\n";
        SaveFile << m_Items[i].data << "| Listbox Item "<< i <<" data\n";
    }

    return true;
}

//-----------------------------------------------------------------------------
// Name : CopyItemsFrom
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::CopyItemsFrom(ListBoxUI<T>& sourceItems)
{
    // clears the items vector
    RemoveAllItems();

    for (GLuint i = 0; i < sourceItems.GetNumItems(); i++)
    {
        AddItem(sourceItems.GetItem(i)->strText, sourceItems.GetItem(i)->data );
    }
}

//-----------------------------------------------------------------------------
// Name : AddItem
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::AddItem(std::string strText, T& data)
{
    // Validate parameters
    if( strText == "" )
        return false;

    //TODO: add something that will catch the exception thrown by push_back in case of lack of memory
    m_Items.emplace_back(strText, data);

    // Update the scroll bar with new range
    m_ScrollBar.SetTrackRange( 0, m_Items.size() );

    return true;
}

//-----------------------------------------------------------------------------
// Name : AddItem(move)
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::AddItem(std::string strText, T&& data)
{
    // Validate parameters
    if( strText == "" )
        return false;

    //TODO: add something that will catch the exception thrown by push_back in case of lack of memory
    m_Items.emplace_back(strText, std::move(data));

    // Update the scroll bar with new range
    m_ScrollBar.SetTrackRange( 0, m_Items.size() );

    return true;
}

//-----------------------------------------------------------------------------
// Name : InsertItem
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::InsertItem(int nIndex, std::string &strText, T& data)
{
    //TODO: add something that will catch the exception
    m_Items.emplace(m_Items.begin() + nIndex, strText, data);

    m_ScrollBar.SetTrackRange( 0, m_Items.size() );

    return true;
}

//-----------------------------------------------------------------------------
// Name : InsertItem(move)
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::InsertItem(int nIndex, std::string &strText, T&& data)
{
    //TODO: add something that will catch the exception
    m_Items.emplace(m_Items.begin() + nIndex, strText, std::move(data));

    m_ScrollBar.SetTrackRange( 0, m_Items.size() );

    return true;
}

//-----------------------------------------------------------------------------
// Name : RemoveItem
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::RemoveItem(GLuint index)
{
    m_Items.erase(m_Items.begin() + index);
    auto selPos = std::find(m_selectedItems.begin(), m_selectedItems.end(), index);
    if (selPos != m_selectedItems.end())
        m_selectedItems.erase(selPos);

    m_ScrollBar.SetTrackRange( 0, m_Items.size() );
    m_listboxChangedig(this);
}

//-----------------------------------------------------------------------------
// Name : RemoveItemByData
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::RemoveItemByData(T& data)
{
    GLuint i;
    for (i = 0; i < m_Items.size(); i++)
    {
        if (m_Items[i].data == data)
        {
            m_Items.erase(i);
            auto selPos = std::find(m_selectedItems.begin(), m_selectedItems.end(), i);
            if (selPos != m_selectedItems.end())
                m_selectedItems.erase(selPos);
        }
    }
    if (i != 0 )
        m_listboxChangedig(this);
}

//-----------------------------------------------------------------------------
// Name : RemoveAllItems
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::RemoveAllItems()
{
    m_Items.clear();
    m_ScrollBar.SetTrackRange( 0, 1 );
    m_selectedItems.clear();
    m_listboxChangedig(this);
}

//-----------------------------------------------------------------------------
// Name : FindItem
// Desc : finds an item that match the given text and returns the item index
// Note : iStart can be given if there is need to start the check from a certain
//        index
//-----------------------------------------------------------------------------
template<class T>
int ListBoxUI<T>::FindItem(std::string strText, GLuint iStart/* = 0*/)
{
    if (strText == "")
        return -1;

    for (GLuint i = iStart; i < m_Items.size(); i++)
    {
        if (m_Items[i].strText == strText)
            return i;
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Name : ContainsItem()
// Desc : check is there is a Item that match the given text
// Note : iStart can be given if there is need to start the check from a certain
//        index
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::ContainsItem(std::string strText, GLuint iStart/* = 0*/)
{
    return ( FindItem( strText, iStart ) != -1 );
}

//-----------------------------------------------------------------------------
// Name : GetItem
//-----------------------------------------------------------------------------
template<class T>
Item<T>* ListBoxUI<T>::GetItem(GLuint nIndex)
{
    if( nIndex < 0 || nIndex >= m_Items.size() )
        return nullptr;

    return &m_Items[nIndex];
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item text
//-----------------------------------------------------------------------------
template<class T>
T* ListBoxUI<T>::GetItemData(std::string strText)
{
    int index = FindItem(strText);
    if( index == -1 )
    {
        return nullptr;
    }

    return &m_Items[index].data;
}

//-----------------------------------------------------------------------------
// Name : GetItemData()
// Desc : returns the item data
// Note : the search for the item is being done based on the item index
//-----------------------------------------------------------------------------
template<class T>
T* ListBoxUI<T>::GetItemData(int nIndex)
{
    if( nIndex < 0 || nIndex >= m_Items.size() )

        return nullptr;

    return &m_Items[nIndex].data;
}

//-----------------------------------------------------------------------------
// Name : GetSelectedIndices()
//-----------------------------------------------------------------------------
template<class T>
const std::vector<int>& ListBoxUI<T>::GetSelectedIndices() const
{
    return m_selectedItems;
}

//-----------------------------------------------------------------------------
// Name : SelectItem()
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::SelectItem(GLuint index, bool select)
{
    if (index > m_Items.size())
        return false;

    m_Items[index].bSelected = select;
    if (select)
        m_selectedItems.push_back(index);
    else
    {
        auto selPos = std::find(m_selectedItems.begin(), m_selectedItems.end(), index);
        if (selPos != m_selectedItems.end())
            m_selectedItems.erase(selPos);
    }
    m_listboxChangedig(this);

    return true;
}

//-----------------------------------------------------------------------------
// Name : SelectItem()
//-----------------------------------------------------------------------------
template<class T>
bool ListBoxUI<T>::SelectItem(std::string strText, bool select)
{
    for (GLuint i = 0; i < m_Items.size(); i++)
    {
        if (m_Items[i].strText == strText)
        {
            m_Items[i].bSelected = select;
            if (select)
                m_selectedItems.push_back(i);
            else
            {
                auto selPos = std::find(m_selectedItems.begin(), m_selectedItems.end(), i);
                if (selPos != m_selectedItems.end())
                    m_selectedItems.erase(selPos);
            }
            return true;
        }
    }
    m_listboxChangedig(this);

    return false;
}

//-----------------------------------------------------------------------------
// Name : GetNumItems()
//-----------------------------------------------------------------------------
template<class T>
GLuint ListBoxUI<T>::GetNumItems()
{
    return m_Items.size();
}

//-----------------------------------------------------------------------------
// Name : ShowItem()
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::ShowItem(int nIndex)
{
    m_ScrollBar.ShowItem(nIndex);
}

//-----------------------------------------------------------------------------
// Name : GetScrollBarWidth()
//-----------------------------------------------------------------------------
template<class T>
int ListBoxUI<T>::GetScrollBarWidth() const
{
    return m_ScrollBar.getWidth();
}

//-----------------------------------------------------------------------------
// Name : SetScrollBarWidth()
//-----------------------------------------------------------------------------
template<class T>
void ListBoxUI<T>::SetScrollBarWidth(int nWidth)
{
    m_ScrollBar.setSize(nWidth, m_ScrollBar.getHeight());
}

#endif  //_LISTBOXUI_CPP
