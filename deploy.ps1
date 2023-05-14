$from = "C:\Users\felix\source\github\esphome-mmwave-sensors" 
$to = "\\homeassistant\config\esphome"  

$components = "\custom_components\"
$headers = "\headers\"
$packages = "\packages\"

Copy-Item "$($from)$($components)*" "$($to)$($components)" -recurse -force
Copy-Item "$($from)$($headers)*" "$($to)$($headers)" -recurse -force
Copy-Item "$($from)$($packages)*" "$($to)$($packages)" -recurse -force