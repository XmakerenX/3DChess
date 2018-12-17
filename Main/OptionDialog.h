#ifndef  _OPTIONDIALOG_H
#define  _OPTIONDIALOG_H

#include "../GUI/DialogUI.h"
#include "../BaseWindow.h"
#include "settingsDef.h"

class OptionDialog : public DialogUI
{
public:
    OptionDialog();
    
    bool init(GLuint width, GLuint height, int nCaptionHeight, std::string captionText, std::string newTexturePath, glm::vec4 dialogColor, AssetManager& assetManger);
    
    void initGUI();
    void initResolutionCombobox();
    
    void setParentWindow(const BaseWindow* parentWindow);
    void setResoultionCombobox(int monitorIndex);
    
    void onWindowedPressed(ButtonUI* windowedButton);
    void onFullScreenPressed(ButtonUI* fullScreenButton);
    void onMonitorSelectChange(ComboBoxUI<int>* monitorCombobox);
    
private:
    const BaseWindow* m_parentWindow;
    std::vector<std::vector<Mode1>> m_monitorsModes;
    
};

#endif  //_OPTIONDIALOG_H
