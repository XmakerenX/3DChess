#ifndef  _EDITORDIALOGUI_H
#define  _EDITORDIALOGUI_H

#include "../GUI/DialogUI.h"
#include "../GameWin.h"

#define IDC_CONTROLTYPESTATIC	 101
#define IDC_COMBOX				 102
#define IDC_WIDTHSTATIC			 103
#define IDC_HEIGHTSTATIC		 104
#define IDC_WIDTHEDITBOX		 105
#define IDC_HEIGHTEDITBOX		 106
#define IDC_IDSTATIC			 107
#define IDC_IDEDITBOX			 108
#define IDC_CONTROLTEXT			 109
#define IDC_TEXTEDITBOX			 110
#define IDC_CREATECONTROL		 111
#define IDC_SLIDERMIN			 112
#define IDC_SLIDERMAX			 113
#define IDC_SLIDERMINTEXT		 114
#define IDC_SLIDERMAXTEXT		 115
#define IDC_RADIOGROUPTEXT       116
#define IDC_RADIOBUTTONGROUP     117
#define IDC_SLIDERSTATIC		 118
#define IDC_SLIDERMINEDITBOX	 119
#define IDC_SLIDERMAXEDITBOX	 120
#define IDC_LISTBOXITEMS		 121
#define IDC_LISTBOXSTATIC		 122
#define IDCLISTOXEDITBOX	     123
#define IDC_LISTBOXITEMSADD      124
#define IDC_LISTBOXITEMSREMOVE   125
#define IDC_COMBOXITEMS			 126
#define IDC_COMBOBOXITEMSADD	 127
#define IDC_COMBOBOXITEMSREMOVE	 128
#define IDC_DIALOGSTATIC		 129
#define IDC_DIALOGWIDTH			 130
#define IDC_DIALOGHEIGHT		 131
#define IDC_DIALOGSETSIZE		 132
#define IDC_FILENAMESTATIC		 133
#define IDC_FILENAMEEDITBOX		 134
#define IDC_LOADFILEBUTTON		 135
#define IDC_SAVEFILEBUTTON		 136
#define IDC_SETCHANGESBUTTON	 137
#define IDC_CONTROLXSTATIC	     138
#define IDC_CONTROLYSTATIC		 139
#define IDC_CONTROLX			 140
#define IDC_CONTROLY			 141
#define IDC_RELOCATEBUTTON		 142
#define IDC_DELETEBUTTON		 143
#define IDC_OPTIONSBUTTON		 144

#define IDC_GENCONTROLID 200

class EditDialogUI : public DialogUI
{
public:
        EditDialogUI(GameWin* window);
        virtual ~EditDialogUI(void);

        bool handleKeyEvent(unsigned char key, bool down);
        bool handleVirtualKeyEvent(GK_VirtualKey virtualKey, bool down, const ModifierKeysStates &modifierStates);
        bool handleMouseEvent(MouseEvent event, const ModifierKeysStates &modifierStates);

        virtual bool     OnRender       (Sprite sprites[ControlUI::SPRITES_SIZE], Sprite topSprites[ControlUI::SPRITES_SIZE], AssetManager& assetManger, double timeStamp);

        void             CreateDialogUI (AssetManager& assetManager);
        
        void             GenControlRightClicked         (ControlUI* pRightClickedControl);

        void             CreateControlClicked           (ButtonUI* createControl);
        void             AddListBoxItemClicked          (ButtonUI* pAddListBoxItemButton);
        void             RemoveListBoxItemClikced       (ButtonUI* pRemoveListBoxItemButton);
        void             AddComboBoxItemClicked         (ButtonUI* pAddComboBoxItemButton);
        void             RemoveComboBoxItemClicked      (ButtonUI* pRemoveComboBoxItemButton);
        void             SetChangesButtonClicked        (ButtonUI* pSetChangesButton);
        void             SaveDialogButtonClicked        (ButtonUI* pSaveButton);
        void             LoadDialogButtonClicked        (ButtonUI* pLoadButton);
        void             SetGenDialogSize               (ButtonUI* pDialogSetButton);
        void             RelocateControlClicked         (ButtonUI* pRecloateControlButton);
        void             DeleteControlClicked           (ButtonUI* pDeleteButton);
        void             OptionsControlClicked          (ButtonUI* pOptionsButton);
        void             ComboboxSelChg                 (ComboBoxUI* pCombobox);

        void             SetStaticGUI                   (bool ControlSelected = false);
        void             SetRadioButtonGUI              (bool ControlSelected = false);
        void             SetSliderGUI                   (bool ControlSelected = false);
        void             SetListBoxGUI                  (bool ControlSelected = false);
        void             SetComboBoxGUI                 (bool ControlSelected = false);

private:
        GameWin* m_window;
        
        DialogUI m_GenDialog;
        unsigned long m_GenControlNum;

        GLuint m_curControlID;
	bool m_controlInCreation;
	bool m_controlRelocate;
        ControlUI * m_pCurSelectedControl;
};

#endif  //_EDITORDIALOGUI_H
