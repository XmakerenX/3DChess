#include "GUIEditor.h" 

GUIEditor::GUIEditor()
{}

void GUIEditor::initGUI()
{
    m_editDialog.SetWindow(m_window);
    m_editDialog.init(500, 735, 18, "Edit Dialog", "", glm::vec4(1.0f, 1.0f, 1.0f, 0.0f), this->m_asset);
    m_editDialog.setClipboardFunctions(m_window->getCopyToClipboardFunc(), m_window->getPasteClipboardFunc());
    m_editDialog.setLocation(550, 0);
    m_editDialog.initDefControlElements(this->m_asset);
    m_editDialog.CreateDialogUI(this->m_asset);
}

void GUIEditor::renderGUI()
{
    m_editDialog.OnRender(this->m_sprites, this->m_topSprites, this->m_asset, this->m_timer.getCurrentTime());
}

void GUIEditor::sendKeyEvent(unsigned char key, bool down)
{
    m_editDialog.handleKeyEvent(key, down);
}

void GUIEditor::sendVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates& modifierStates)
{
    m_editDialog.handleVirtualKeyEvent(virtualKey, down, modifierStates);
}

void GUIEditor::sendMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates)
{
    BaseGame::sendMouseEvent(event, modifierStates);
    m_editDialog.handleMouseEvent(event, modifierStates);
}
