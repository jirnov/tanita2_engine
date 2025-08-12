pushd ..\..
bjam --toolset=msvc -d1 tanita2 %* -j2
popd