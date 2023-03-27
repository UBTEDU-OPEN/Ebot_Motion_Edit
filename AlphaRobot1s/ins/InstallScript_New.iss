; ������QT����İ�װ�ű���

#define AppName            "AlphaRobot1s_QT" 
#define AppVersion         "2.4.0.1128"
#define Publisher          "�������ű�ѡ�Ƽ����޹�˾"

[Setup]
; ע: AppId��ֵΪ������ʶ��Ӧ�ó���
; ��ҪΪ������װ����ʹ����ͬ��AppIdֵ��
; (�����µ�GUID����� ����|��IDE������GUID��)
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
;�����������...

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: checkablealone; OnlyBelowVersion: 0, 6.3

[Files]
Source: "{#AppName}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; ע��: ��Ҫ���κι���ϵͳ�ļ���ʹ�á�Flags: ignoreversion��

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
    {�������ͺ�exe�еĶ���һ��}
    TLangType = (  
        eLangOther,  {��������}
        eLangChs,    {��������}
        eLangEng,    {Ӣ��}
        eLangJpa,    {����}
        eLangCht,    {��������}
        eLangGer,    {����}
        eLangSpa     {��������}
        {�����������...}
    ); 

procedure CurStepChanged (CurStep: TSetupStep ); 

var 
    sellang : String;  {��ǰѡ�������  ������[Languages]��}
    elang: TLangType;  {��������}
    sinipath: String;  {ini�ļ�·��} 
    sexedir: String;   
    rescode: Integer;

begin    
      
     if CurStep = ssPostInstall then  {��װ���} 
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
                {�����������...}
                else
                    elang := eLangEng;
             end; 
             
             SetIniInt('Config', 'Language', Ord(elang), sinipath); {set ini}              
            
             if Exec(sexedir + '\drvins.exe', '', '', SW_HIDE, ewWaitUntilTerminated, rescode) = False then 
                 MsgBox('install driver failed', mbError, MB_OK);
         end;
end;







