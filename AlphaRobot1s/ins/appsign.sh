#! /bin/bash


#定义变量
#$SRC_DIR=../build-AlphaRobot-Desktop_Qt_5_5_1_clang_64bit-Release/AlphaRobot

rm -rf AlphaRobot.app AlphaRobot.pkg
cp -a ../build-AlphaRobot-Desktop_Qt_5_5_1_clang_64bit-Release/AlphaRobot/AlphaRobot.app AlphaRobot.app 
rm -rf AlphaRobot.app/Contents/Frameworks
cp -a  Frameworks5.5.1 AlphaRobot.app/Contents/Frameworks

#(app目录下所有文件都要codesign)
#framework     
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtCore.framework/Versions/5/QtCore
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtDBus.framework/Versions/5/QtDBus
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtGui.framework/Versions/5/QtGui
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtMultimedia.framework/Versions/5/QtMultimedia
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtMultimediaWidgets.framework/Versions/5/QtMultimediaWidgets
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtNetwork.framework/Versions/5/QtNetwork
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtOpenGL.framework/Versions/5/QtOpenGL
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtWidgets.framework/Versions/5/QtWidgets
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/Frameworks/QtXml.framework/Versions/5/QtXml

#PlugIns	
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/audio/libqtaudio_coreaudio.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/bearer/libqcorewlanbearer.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/bearer/libqgenericbearer.dylib

#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqdds.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqgif.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqicns.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqico.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqjp2.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqjpeg.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqmng.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqtga.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqtiff.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqwbmp.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/imageformats/libqwebp.dylib

#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/mediaservice/libqavfcamera.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/mediaservice/libqavfmediaplayer.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/mediaservice/libqtmedia_audioengine.dylib

#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/platforms/libqcocoa.dylib

#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/PlugIns/printsupport/libcocoaprintersupport.dylib
											

#MacOS
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libfbxsdk.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libLocalActSyncWidget.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libUBXRobot3DView.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libUBXRobotController.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libzxLib.1.dylib

#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libUBXActionLib.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libUBXLogin.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libUBXUpdateModule.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libquazip.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libUBXPlayer.1.dylib

#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libFLAC.8.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libFLAC++.6.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libmpg123.0.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libogg.0.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libsndfile.1.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libvorbis.0.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libvorbisenc.2.dylib
#codesign -s "3rd Party Mac Developer Application"  -i com.ubt.AlphaRobot  AlphaRobot.app/Contents/MacOS/libvorbisfile.3.dylib


#(签名整个文件夹)
#这里需要sandbox 文件AlphaRobot.entitlements
#codesign -s "3rd Party Mac Developer Application"   -i com.ubt.AlphaRobot --entitlements AlphaRobot.entitlements  AlphaRobot.app


#(制作pkg包) 
#普通版
pkgbuild --identifier com.ubt.AlphaRobot --root AlphaRobot.app  --install-location "/Applications/AlphaRobot.app" AlphaRobot.pkg

#appstrore专用
#productbuild --component AlphaRobot.app /Applications/ -s "3rd Party Mac Developer Installer" AlphaRobot.pkg  
