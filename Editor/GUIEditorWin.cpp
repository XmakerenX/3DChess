#include "GUIEditorWin.h" 

GUIEditorWin::GUIEditorWin() : m_editDialog(this)
{
    
}

void GUIEditorWin::initGUI()
{
    m_editDialog.init(500, 735, 18, "Edit Dialog", "", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), m_asset);
    m_editDialog.setLocation(550, 0);
    m_editDialog.initDefControlElements(m_asset);
    m_editDialog.CreateDialogUI(m_asset);
}

void GUIEditorWin::renderGUI()
{
    m_editDialog.OnRender(m_sprites, m_topSprites, m_asset, timer.getCurrentTime());
}

void GUIEditorWin::sendKeyEvent(unsigned char key, bool down)
{
    m_editDialog.handleKeyEvent(key, down);
}

void GUIEditorWin::sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    m_editDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
}

void GUIEditorWin::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    GameWin::sendMouseEvent(event, modifierStates);
    m_editDialog.handleMouseEvent(event, modifierStates);
}
