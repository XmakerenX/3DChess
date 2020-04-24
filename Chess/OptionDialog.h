/* * 3D Chess - A cross platform Chess game for 2 players made using OpenGL and c++
 * Copyright (C) 2016-2020 Matan Keren <xmakerenx@gmail.com>
 *
 * This file is part of 3D Chess.
 *
 * 3D Chess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 3D Chess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 3D Chess.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef  _OPTIONDIALOG_H
#define  _OPTIONDIALOG_H

#include <Render/GUI/DialogUI.h>
#include <GameWindow/BaseWindow.h>
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
