; 阿尔法QT软件的安装脚本。

#define AppName            "AlphaRobot1s_QT" 
#define AppVersion         "2.4.0.1128"
#define Publisher          "深圳市优必选科技有限公司"

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
AppId={{3099FA8B-0DB0-401A-B273-8979881248A0}
AppName={#AppName}
AppVersion={#AppVersion}
VersionInfoVersion={#AppVersion}
AppPublisher={#Publisher}
DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
OutputDir=.
OutputBaseFilename={#AppName}_setup
SetupIconFile={#AppName}.ico
Compression=lzma
SolidCompression=yesPrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Languages\English.isl"
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl" 
;Name: "chinesetrad"; MessagesFile: "compiler:Languages\ChineseTrad.isl" 
;Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
;Name: "german"; MessagesFile: "compiler:Languages\German.isl"
;Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
;添加其他语言...

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone; OnlyBelowVersion: 0, 6.3

[Files]
Source: "{#AppName}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
Name: "{group}\{#AppName}"; Filename: "{app}\{#AppName}"
Name: "{group}\{cm:UninstallProgram,{#AppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#AppName}"; Filename: "{app}\{#AppName}"; Tasks: desktopicon

[Run] 
Filename: "{app}\{#AppName}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: postinstall nowait skipifsilent shellexec; Components: Main

[UninstallDelete]
Type: files; Name: "{app}\{#AppName}.ini"
Type: filesandordirs; Name: "{app}\Robot"

[Types]
Name: "full"; Description: "Full installation" ; Flags: iscustom

[Components]  
Name: "Main"; Description: "Main"; Types: full ; Flags: fixed 

[Code]

type
    {语言类型和exe中的定义一致}
    TLangType = (  
        eLangOther,  {其他语言}
        eLangChs,    {简体中文}
        eLangEng,    {英文}
        eLangJpa,    {日文}
        eLangCht,    {繁体中文}
        eLangGer,    {德语}
        eLangSpa     {西班牙语}
        {添加其他语言...}
    ); 

procedure CurStepChanged (CurStep: TSetupStep ); 

var 
    sellang : String;  {当前选择的语言  定义在[Languages]中}
    elang: TLangType;  {语言类型}
    sinipath: String;  {ini文件路径} 
    sexedir: String;   
    rescode: Integer;

begin    
      
     if CurStep = ssPostInstall then  {安装完成} 
         begin
             sexedir:= WizardDirValue(); 
             sinipath:= sexedir + '\Robot\Config.ini';
    
             elang := eLangOther;   
             sellang := ActiveLanguage();

             case sellang of
                'chinesesimp':
                  elang := eLangChs;
                'english':
                  elang := eLangEng;
                'japanese':
                  elang := eLangJpa;
                'chinesetrad':
                  elang := eLangCht;
                'german':
                  elang := eLangGer;
                'spanish':
                  elang := eLangSpa;
                {添加其他语言...}
                else
                    elang := eLangEng;
             end; 
             
             SetIniInt('Config', 'Language', Ord(elang), sinipath); {set ini}              
            
             if Exec(sexedir + '\drvins.exe', '', '', SW_HIDE, ewWaitUntilTerminated, rescode) = False then 
                 MsgBox('install driver failed', mbError, MB_OK);
         end;
end;







