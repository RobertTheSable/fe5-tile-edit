#!/bin/bash
# A script to generate an application bundle for Mac OS
# And clean up some uneeded libraries QT adds automatically

if [ "$(uname)" != "Darwin" ]; then
    >&2 echo Not running on OSX
    exit 1
fi

if [ $# -lt 1 ]; then
    >&2 echo Missing source folder argument
    exit 2
fi

SRC_FOLDER=$1

PROJECT_NAME=FE5TileMap

if [ ! -f $SRC_FOLDER/$PROJECT_NAME.pro ]; then
    >&2 echo Provided source folder does not contain the expected project file.
    exit 3
fi

# Create the bundle
qmake -spec macx-xcode $SRC_FOLDER/$PROJECT_NAME.pro && \
    xcodebuild -list -project $PROJECT_NAME.xcodeproj && \
    xcodebuild -scheme $PROJECT_NAME -configuration Release build && \
    cd Release && macdeployqt $PROJECT_NAME.app

# Remove unnecessary libraries
ALLOWED_PLUGINS="printsupport platforms styles"
for x in $PROJECT_NAME.app/Contents/PlugIns/* ; do 
    [[ -z $(echo $ALLOWED_PLUGINS | grep $(basename $x)) ]] && rm -Rf $x 
done

PLATFORM_PATH=$PROJECT_NAME.app/Contents/PlugIns/platforms/libqcocoa.dylib
for x in $PROJECT_NAME.app/Contents/Frameworks/* ; do 
    [[ -z $(otool -L $PLATFORM_PATH | grep $(basename $x)) ]] && rm -Rf $x 
done
    
