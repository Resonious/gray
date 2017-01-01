Set-Alias grayfx .\Tools\grayfx\target\debug\grayfx.exe

grayfx .\Resource-Dev\character-wip.svg  -i body,head  -c color  -d _idleFrame `
       | Out-File .\Classes\Generated\CharIdle.h -Encoding utf8

grayfx .\Resource-Dev\test-level.svg  -d ground  -p "ground->getPhysicsBody()" `
       | Out-File .\Classes\Generated\TestLevel.h -Encoding utf8
