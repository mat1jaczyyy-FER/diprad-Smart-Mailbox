@echo off

robocopy "portal" "..\VPS\srv\smart-mailbox\portal" /E /XD "node_modules" ".git" ".vscode" ".astro" "dist" /XF ".*"
robocopy "server" "..\VPS\srv\smart-mailbox\server" /E /XD "node_modules" ".git" ".vscode" "dist" /XF ".*"

pause
