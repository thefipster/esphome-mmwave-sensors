$from = "C:\Users\felix\source\github\esphome-mmwave-sensors\custom_components\*" 
$to = "\\homeassistant\config\esphome\custom_components\"  
Copy-Item $from $to -recurse -force