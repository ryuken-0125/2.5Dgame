import os

path = r'c:\Users\honoka.T\Desktop\GameJam\2.5Dgame\lightvroom\lightvroom.vcxproj'

with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

content = content.replace(
    '<ClCompile Include="scene_manager.cpp" />',
    '<ClCompile Include="scene_manager.cpp" />\n    <ClCompile Include="warp_zone.cpp" />\n    <ClCompile Include="tilemap.cpp" />\n    <ClCompile Include="field_scene.cpp" />\n    <ClCompile Include="sub_scene.cpp" />'
)

content = content.replace(
    '<ClInclude Include="scene_manager.h" />',
    '<ClInclude Include="scene_manager.h" />\n    <ClInclude Include="game_context.h" />\n    <ClInclude Include="warp_zone.h" />\n    <ClInclude Include="tilemap.h" />\n    <ClInclude Include="field_scene.h" />\n    <ClInclude Include="sub_scene.h" />'
)

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print("vcxproj updated!")

filters_path = r'c:\Users\honoka.T\Desktop\GameJam\2.5Dgame\lightvroom\lightvroom.vcxproj.filters'
with open(filters_path, 'r', encoding='utf-8') as f:
    fcontent = f.read()

fcontent = fcontent.replace(
    '<ClCompile Include="scene_manager.cpp">\n      <Filter>ソース ファイル</Filter>\n    </ClCompile>',
    '<ClCompile Include="scene_manager.cpp">\n      <Filter>ソース ファイル</Filter>\n    </ClCompile>\n    <ClCompile Include="warp_zone.cpp">\n      <Filter>ソース ファイル</Filter>\n    </ClCompile>\n    <ClCompile Include="tilemap.cpp">\n      <Filter>ソース ファイル</Filter>\n    </ClCompile>\n    <ClCompile Include="field_scene.cpp">\n      <Filter>ソース ファイル</Filter>\n    </ClCompile>\n    <ClCompile Include="sub_scene.cpp">\n      <Filter>ソース ファイル</Filter>\n    </ClCompile>'
)

fcontent = fcontent.replace(
    '<ClInclude Include="scene_manager.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>',
    '<ClInclude Include="scene_manager.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>\n    <ClInclude Include="game_context.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>\n    <ClInclude Include="warp_zone.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>\n    <ClInclude Include="tilemap.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>\n    <ClInclude Include="field_scene.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>\n    <ClInclude Include="sub_scene.h">\n      <Filter>ヘッダー ファイル</Filter>\n    </ClInclude>'
)

with open(filters_path, 'w', encoding='utf-8') as f:
    f.write(fcontent)

print("filters updated!")
