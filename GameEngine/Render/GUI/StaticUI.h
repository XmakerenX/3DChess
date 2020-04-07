#ifndef  _STATICUI_H
#define  _STATICUI_H

#include <string>
#include "ControlUI.h"

class StaticUI : public ControlUI
{
public:
                            StaticUI            (DialogUI* pParentDialog, int ID, const std::string &strText, int x, int y, GLuint width, GLuint height);
                            StaticUI            (std::istream& inputFile);
    virtual                 ~StaticUI           (void);

                void        setTextColor        (glm::vec4 textColor);
                void        setText             (const std::string& text);
                void        setTextOrientation  (mkFont::TextFormat textOrientation);
    const       std::string getText             () const;

                void        Render              (Sprite sprites[SPRITES_SIZE], Sprite topSprites[SPRITES_SIZE], double timeStamp);

    virtual     bool        SaveToFile          (std::ostream& SaveFile);

protected:
    std::string m_strText;
    glm::vec4   m_textColor;
    mkFont::TextFormat m_textOrientation;
};

#endif  //_STATICUI_H
