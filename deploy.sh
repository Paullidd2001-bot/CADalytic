#!/usr/bin/env bash

set -e

APP_NAME="cadalytic.exe"
BUILD_DIR="build"
DEPLOY_DIR="deploy"

QT_BIN="/c/msys64/clang64/bin"
QT_PLUGINS="/c/msys64/clang64/share/qt6/plugins"

echo "Creating deploy folder..."
rm -rf "$DEPLOY_DIR"
mkdir -p "$DEPLOY_DIR"
mkdir -p "$DEPLOY_DIR/platforms"

echo "Copying application..."
cp "$BUILD_DIR/$APP_NAME" "$DEPLOY_DIR/"

echo "Copying Qt6 DLLs..."
cp "$QT_BIN"/Qt6Core.dll "$DEPLOY_DIR/"
cp "$QT_BIN"/Qt6Gui.dll "$DEPLOY_DIR/"
cp "$QT_BIN"/Qt6Widgets.dll "$DEPLOY_DIR/"
cp "$QT_BIN"/Qt6OpenGL.dll "$DEPLOY_DIR/"
cp "$QT_BIN"/Qt6OpenGLWidgets.dll "$DEPLOY_DIR/"

echo "Copying ICU DLLs..."
cp "$QT_BIN"/libicu*.dll "$DEPLOY_DIR/"

echo "Copying C++ runtime..."
cp "$QT_BIN"/libc++.dll "$DEPLOY_DIR/" || true

echo "Copying Qt platform plugin..."
cp "$QT_PLUGINS/platforms/qwindows.dll" "$DEPLOY_DIR/platforms/"

echo "Deployment complete!"
echo "Run your app from: $DEPLOY_DIR/$APP_NAME"
