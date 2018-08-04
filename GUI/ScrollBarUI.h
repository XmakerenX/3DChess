#ifndef  _SCROLLBARUI_H
#define  _SCROLLBARUI_H

#include "ControlUI.h"

class ScrollBarUI : public ControlUI
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
                    ScrollBarUI     (void);
                    ScrollBarUI     (std::istream& inputFile);
    virtual         ~ScrollBarUI    (void);

	//-------------------------------------------------------------------------
	// functions that handle user Input to control
	//-------------------------------------------------------------------------
    virtual bool    handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);

    virtual bool	Pressed         (Point pt, const ModifierKeysStates &modifierStates, double timeStamp);
    virtual bool    Released        (Point pt);
    virtual bool    Dragged         (Point pt);

    virtual bool	SaveToFile      (std::ostream& SaveFile);

    virtual void    Render          (Sprite& sprite, Sprite& textSprite, double timeStamp);

	//-------------------------------------------------------------------------
	// functions that handle control specific properties
	//-------------------------------------------------------------------------
            void    SetTrackRange   (int nStart, int nEnd);
            void    SetTrackPos     (int nPosition);

            int     GetTrackPos     ();
            int     GetPageSize     ();

            void    SetPageSize     (int nPageSize);

            void    Scroll          (int nDelta);    // Scroll by nDelta items (plus or minus)
            void    ShowItem        (int nIndex);  // Ensure that item nIndex is displayed, scroll if necessary

private:
	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR            No arrow is down.
	// CLICKED_UP       Up arrow is clicked.
	// CLICKED_DOWN     Down arrow is clicked.
	// HELD_UP          Up arrow is held down for sustained period.
	// HELD_DOWN        Down arrow is held down for sustained period.
	enum ARROWSTATE { CLEAR, CLICKED_UP, CLICKED_DOWN, HELD_UP, HELD_DOWN};

	enum ELEMENTS {TRACK, UPBUTTON, DOWNBUTTON, THUMB };

protected:

	virtual void    UpdateRects();
	void            UpdateThumbRect();
	void            Cap();  // Clips position at boundaries. Ensures it stays within legal range.

	bool m_bShowThumb;	 // would only be true if there is more items than the size of a single page
	bool m_bDrag;		 // true only while the thumb is being dragged
    Rect m_rcElements[4];// the Rects for the scrollbar elements
	int m_nPosition;	 // Position of the first displayed item currently being shown
	int m_nPageSize;	 // How many items are displayable in one page
	int m_nStart;		 // First item index
	int m_nEnd;			 // The index after the last item
	int m_thumbOffsetY;
    Point m_LastMouse;	 // Last mouse position
	ARROWSTATE m_Arrow;  // State of the arrows
    double m_dArrowTS;   // Timestamp of last arrow event.

	// Minimum scroll bar thumb size
    static const int  SCROLLBAR_MINTHUMBSIZE;
	// Delay and repeat period when clicking on the scroll bar arrows
	static const float SCROLLBAR_ARROWCLICK_DELAY;
	static const float SCROLLBAR_ARROWCLICK_REPEAT;


};
#endif  //_SCROLLBARUI_H
