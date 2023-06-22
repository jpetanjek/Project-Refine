Set-Location ".\ProjectRefine"
Remove-Item ".\addon.gproj"
Copy-Item "addon.gproj_master" "addon.gproj"
Set-Location "..\"

Set-Location ".\ProjectRefineMaps"
Remove-Item ".\addon.gproj"
Copy-Item "addon.gproj_master" "addon.gproj"
Set-Location "..\"