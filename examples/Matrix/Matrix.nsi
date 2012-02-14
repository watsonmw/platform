;Matrix screen saver install script

!define MUI_PRODUCT "Matrix Revisited screensaver"
!define VER_MAJOR "0"
!define VER_MINOR "1b"
!define MUI_VERSION "${VER_MAJOR}.${VER_MINOR}"
!define SCREENSAVER_NAME "MatrixRevisited.scr"

!include "MUI.nsh"

;--------------------------------
;Configuration

  ;General
  OutFile "MatrixRevisitedInstall.exe"

  ;Folder selection page
  InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
  
  ;Remember install folder
  InstallDirRegKey HKCU "Software\${MUI_PRODUCT}" ""

;--------------------------------
;Modern UI Configuration

  !define MUI_DIRECTORYPAGE
  
  !define MUI_ABORTWARNING
  
  !define MUI_UNINSTALLER
  !define MUI_UNCONFIRMPAGE
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"
  
;--------------------------------
;Installer Sections

Section "Screensaver"

  SetOverwrite on

  SetOutPath "$INSTDIR"
  File "${SCREENSAVER_NAME}"
  File "msvcrt.dll"
  
  SetOutPath "$INSTDIR\Data"
  File "Data\font.tga"
  File "Data\defaults.cfg"

  ;Set as current screen saver
  GetFullPathName /SHORT $R1 "$INSTDIR\${SCREENSAVER_NAME}"
  WriteRegStr HKCU "Control Panel\Desktop" "scrnsave.exe" $R1

  ;Store install folder
  WriteRegStr HKCU "Software\${MUI_PRODUCT}" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  WriteRegStr HKLM SOFTWARE\NSIS "" $INSTDIR
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "DisplayName" "${MUI_PRODUCT}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "DisplayIcon" "$INSTDIR\${SCREENSAVER_NAME},0"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "DisplayVersion" "${MUI_VERSION}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "VersionMajor" "${VER_MAJOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "VersionMinor" "${VER_MINOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "NoModify" "1"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS" "NoRepair" "1"

SectionEnd

;Display the Finish header
;Insert this macro after the sections if you are not using a finish page
!insertmacro MUI_SECTIONS_FINISHHEADER

;--------------------------------
;Uninstaller Section

Section "Uninstall"
  
  Delete "$INSTDIR\Data\font.tga"
  Delete "$INSTDIR\Data\defaults.cfg"
  
  RMDir "$INSTDIR\Data"

  Delete "$INSTDIR\${SCREENSAVER_NAME}"
  Delete "$INSTDIR\log.txt"
  Delete "$INSTDIR\Uninstall.exe"

  RMDir "$INSTDIR"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSIS"
  DeleteRegKey HKCU "Software\${MUI_PRODUCT}"
  
  ;Display the Finish header
  !insertmacro MUI_UNFINISHHEADER

SectionEnd
