#!/bin/bash

# 应用程序的名称
APP_NAME="FinnConverter.app"

# 应用程序的可执行文件路径
EXECUTABLE_PATH="$APP_NAME/Contents/MacOS/$(defaults read $(pwd)/$APP_NAME/Contents/Info CFBundleExecutable)"

# 修正动态库路径的函数
fix_library_path() {
    local lib=$1
    local new_path="@executable_path/../Frameworks/$(basename $lib)"
    echo "Fixing $lib -> $new_path"
    install_name_tool -change $lib $new_path $EXECUTABLE_PATH
}

# 获取所有动态库依赖关系
DEPENDENCIES=$(otool -L $EXECUTABLE_PATH | grep -o '/usr/local/.*\(\.dylib\|\.so\)')

# 遍历依赖关系并修正路径
for lib in $DEPENDENCIES; do
    fix_library_path $lib
done

echo "All libraries paths have been fixed."