@echo off

robocopy "web" "..\VPS\srv\smart-mailbox" /E /XD "node_modules" ".git" ".vscode" ".astro" "dist" /XF ".*"

pause
