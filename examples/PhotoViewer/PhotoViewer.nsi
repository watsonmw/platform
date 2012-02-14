;Photo viewer screen saver install script

!include "MUI.nsh"

;--------------------------------
;Configuration

  !define PRODUCT_NAME "PhotoViewer Screensaver"
  !define VER_MAJOR "0"
  !define VER_MINOR "1"
  !define VERSION "${VER_MAJOR}.${VER_MINOR}"
  !define SCREENSAVER_NAME "PhotoViewer.scr"
  !define MUI_ICON "PhotoViewer.ico"
  !define MUI_UNICON "PhotoViewer.ico"

  Name "${PRODUCT_NAME} ${VERSION}"

  RequestExecutionLevel admin

  ;General
  OutFile "photoviewer-setup.exe"

  ;Folder selection page
  InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
  
  ;Remember install folder
  InstallDirRegKey HKLM "Software\WatsonWare\${PRODUCT_NAME}" ""


;--------------------------------
;Modern UI Configuration

  !define MUI_ABORTWARNING
  
  !insertmacro MUI_PAGE_DIRECTORY 
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Screensaver" SecDummy

  ; Uninstall old version - This doesnt work for now
  ; ReadRegStr $R1 HKLM "Software\WatsonWare\${PRODUCT_NAME}" ""
  ; ExecWait '"$R1\Uninstall.exe"' $R2

  SetOverwrite on

  SetOutPath $INSTDIR
  File ${SCREENSAVER_NAME}
  File "FreeImage.dll"
  File "freeimage-license.txt"
  File "gdiplus.dll"

  SetOutPath $WINDIR
  File "Runner\Release\${SCREENSAVER_NAME}"

  ;Set as current screen saver
  GetFullPathName /SHORT $R1 "$INSTDIR\${SCREENSAVER_NAME}"
  WriteRegStr HKCU "Control Panel\Desktop" "scrnsave.exe" $R1

  ;Store install folder
  WriteRegStr HKLM "Software\WatsonWare\${PRODUCT_NAME}" "" $INSTDIR

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "InstallLocation" $INSTDIR
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayIcon" "$INSTDIR\${SCREENSAVER_NAME},0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "DisplayVersion" "${VERSION}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "VersionMajor" "${VER_MAJOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "VersionMinor" "${VER_MINOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoModify" "1"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" "NoRepair" "1"
SectionEnd

;--------------------------------
; Show screen saver settings dialog
Function .onInstSuccess
  Exec "RunDLL32.exe shell32.dll,Control_RunDLL desk.cpl,,1"
FunctionEnd


;--------------------------------
;Descriptions

  LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  Delete "$INSTDIR\${SCREENSAVER_NAME}"
  Delete "$INSTDIR\log.txt"
  Delete "$INSTDIR\FreeImage.dll"
  Delete "$INSTDIR\gdiplus.dll"
  Delete "$INSTDIR\freeimage-license.txt"
  Delete "$INSTDIR\Uninstall.exe"
  Delete "$WINDIR\${SCREENSAVER_NAME}"

  RMDir "$INSTDIR"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
  DeleteRegKey HKLM "Software\WatsonWare\${PRODUCT_NAME}"
  DeleteRegKey HKCU "Software\WatsonWare\${PRODUCT_NAME}"
  DeleteRegKey /ifempty HKLM "Software\WatsonWare"
  DeleteRegKey /ifempty HKCU "Software\WatsonWare"

SectionEnd
