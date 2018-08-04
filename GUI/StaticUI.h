#ifndef  _STATICUI_H
#define  _STATICUI_H

#include <string>
#include "ControlUI.h"

class StaticUI : public ControlUI
{
public:
                            StaticUI    (DialogUI* pParentDialog, int ID, const std::string &strText, int x, int y, GLuint width, GLuint height);
                            StaticUI    (std::istream& inputFile);
    virtual                 ~StaticUI   (void);

                void        setTextColor(glm::vec4 textColor);
    const       std::string getText     () const;

                void        Render      (Sprite& sprite, Sprite& textSprite , double timeStamp);

    virtual     bool        SaveToFile  ( std::ostream& SaveFile );

protected:
	std::string	m_strText;
	glm::vec4	m_textColor;
};

#endif  //_STATICUI_H
