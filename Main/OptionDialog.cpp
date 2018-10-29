#include "OptionDialog.h"
#include "../GUI/DialogUI.cpp"

//-----------------------------------------------------------------------------
// Name : OptionDialog (constructor)
//-----------------------------------------------------------------------------
OptionDialog::OptionDialog(const GameWin& parentWindow)
    : m_parentWindow(parentWindow)
{}

//-----------------------------------------------------------------------------
// Name : init ()
//-----------------------------------------------------------------------------
bool OptionDialog::init(GLuint width, GLuint height, int nCaptionHeight, std::string captionText, std::string newTexturePath, glm::vec4 dialogColor, AssetManager& assetManger)
{
    if (!DialogUI::init(width, height, nCaptionHeight, captionText, newTexturePath, dialogColor, assetManger))
        return false;
    
    initWoodControlElements(assetManger);
    //initDefControlElements(assetManger);
    
    if(!LoadDialogFromFile("data/dialogs/settings.txt"))
        return false;
    
    initGUI();
    
    return true;
}

//-----------------------------------------------------------------------------
// Name : initGUI ()
//-----------------------------------------------------------------------------
void OptionDialog::initGUI()
{
    m_monitorsModes = m_parentWindow.getMonitorsModes();
    for (int i = 0; i < m_monitorsModes.size(); i++)
        getComboBox<int>(IDC_MONITOR)->AddItem(std::to_string(i) ,i);
    
    getRadioButton(IDC_WINRADIO)->setChecked(true);

    initResolutionCombobox();
    
    setResoultionCombobox(0);
        
    getComboBox<int>(IDC_DPETHSTENCOM)->AddItem("None", 0);
    
    getComboBox<int>(IDC_MULSAMPLECOM)->AddItem("Disabled", 0);
    getComboBox<int>(IDC_MULSAMPLECOM)->AddItem("X2", 2);
    getComboBox<int>(IDC_MULSAMPLECOM)->AddItem("X4", 4);
    getComboBox<int>(IDC_MULSAMPLECOM)->AddItem("X8", 8);
    getComboBox<int>(IDC_MULSAMPLECOM)->AddItem("X16", 16);
    
    getComboBox<int>(IDC_ROTATIONCOM)->AddItem("On", 1);
    getComboBox<int>(IDC_ROTATIONCOM)->AddItem("Off", 0);
    
    getRadioButton(IDC_WINRADIO)->connectToClick(boost::bind(&OptionDialog::onWindowedPressed, this, _1));
    getRadioButton(IDC_FULLSCREENRADIO)->connectToClick(boost::bind(&OptionDialog::onFullScreenPressed, this, _1));
    getComboBox<int>(IDC_MONITOR)->ConnectToSelectChg(boost::bind(&OptionDialog::onMonitorSelectChange, this , _1));
    
}

//-----------------------------------------------------------------------------
// Name : initResolutionCombobox ()
//-----------------------------------------------------------------------------
void OptionDialog::initResolutionCombobox()
{
    ComboBoxUI<int>* pTempReoultionComboBox = getComboBox<int>(IDC_RESOLUTIONCOM);
    GLuint ID = pTempReoultionComboBox->getID();
    GLuint height = pTempReoultionComboBox->getHeight();
    GLuint width = pTempReoultionComboBox->getWidth();
    int x = pTempReoultionComboBox->getX();
    int y = pTempReoultionComboBox->getY();
    
    RemoveControl(IDC_RESOLUTIONCOM);
    
    addComboBox<Resolution>(ID, "", x, y, width, height, 0);
    getComboBox<Resolution>(IDC_RESOLUTIONCOM)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : setResoultionCombobox ()
//-----------------------------------------------------------------------------
void OptionDialog::setResoultionCombobox(int monitorIndex)
{
    if (monitorIndex >= m_monitorsModes.size())
        return;
    
    getComboBox<Resolution>(IDC_RESOLUTIONCOM)->RemoveAllItems();
    for (Mode1 mode : m_monitorsModes[monitorIndex])
        getComboBox<Resolution>(IDC_RESOLUTIONCOM)->AddItem(std::to_string(mode.width) + "X" + std::to_string(mode.height), Resolution(mode.width, mode.height));
}

//-----------------------------------------------------------------------------
// Name : onWindowedPressed ()
//-----------------------------------------------------------------------------
void OptionDialog::onWindowedPressed(ButtonUI* windowedButton)
{
    getComboBox<Resolution>(IDC_RESOLUTIONCOM)->setEnabled(false);
}

//-----------------------------------------------------------------------------
// Name : onFullScreenPressed ()
//-----------------------------------------------------------------------------
void OptionDialog::onFullScreenPressed(ButtonUI* fullScreenButton)
{
    getComboBox<Resolution>(IDC_RESOLUTIONCOM)->setEnabled(true);
}

//-----------------------------------------------------------------------------
// Name : onMonitorSelectChange ()
//-----------------------------------------------------------------------------
void OptionDialog::onMonitorSelectChange(ComboBoxUI<int>* monitorCombobox)
{
    int monitorSelected = *(getComboBox<int>(IDC_MONITOR)->GetSelectedData());
    setResoultionCombobox(monitorSelected);
}
