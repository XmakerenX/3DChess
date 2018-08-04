#ifndef  _SLIDERUI_H
#define  _SLIDERUI_H

#include "ControlUI.h"

class SliderUI : public ControlUI
{
public:

    typedef boost::signals2::signal<void (SliderUI*)>  signal_slider;
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
                    SliderUI            (DialogUI* pParentDialog, int ID, int x, int y, int width, int height, int min, int max, int nValue );
                    SliderUI            (std::istream& inputFile);
    virtual         ~SliderUI           (void);

    virtual bool    ContainsPoint       (Point pt);

	//-------------------------------------------------------------------------
	// functions that handle user input to the control
	//-------------------------------------------------------------------------
    virtual bool    handleMouseEvent    (MouseEvent event, const ModifierKeysStates &modifierStates);
    virtual bool	Pressed				(Point pt, const ModifierKeysStates &modifierStates, double timeStamp);
    virtual bool    Released			(Point pt);
    virtual bool    Scrolled			(int nScrollAmount);
    virtual bool    Dragged				(Point pt);

    void		    connectToSliderChg	(const signal_slider::slot_type& subscriber);

	//-------------------------------------------------------------------------
	// functions that handle Rendering
	//-------------------------------------------------------------------------
    virtual void    UpdateRects         ();
    virtual bool    CanHaveFocus        ();

    virtual bool	SaveToFile          (std::ostream& SaveFile);

    virtual void    Render              (Sprite& sprite, Sprite& textSprite, double timeStamp);

            void    SetValue            (int nValue);
            int     GetValue            () const;

            void    GetRange            (int& nMin, int& nMax) const;
            void    SetRange            (int nMin, int nMax);

protected:
            void    SetValueInternal    (int nValue, bool bFromInput);
            int     ValueFromPos        (int x);

	int m_nValue;

	int m_nMin;
	int m_nMax;

	int m_nDragX;      // Mouse position at start of drag
	int m_nDragOffset; // Drag offset from the center of the button
	int m_nButtonX;

	bool m_bPressed;

    boost::signals2::signal<void (SliderUI*)> m_sliderChangedSig;

    Rect m_rcButton;

private:
	enum ELEMENTS{TRACK, BUTTON};
};

#endif  //_SLIDERUI_H
