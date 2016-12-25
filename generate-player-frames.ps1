Set-Alias grayfx .\Tools\grayfx\target\debug\grayfx.exe

grayfx .\Resource-Dev\character-wip.svg -i body,head -c color -v _idleFrame `
       | Out-File .\Classes\Generated\CharIdle.h -Encoding utf8
