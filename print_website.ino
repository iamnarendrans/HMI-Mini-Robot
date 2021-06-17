void printWebsite(){
client.println("<html><head><meta charset=\"utf-8\">");
client.println("<meta name=\"viewport\" content=\"width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0\">");
client.println("<script src = \"https://cdn.jsdelivr.net/gh/jeromeetienne/virtualjoystick.js/virtualjoystick.js\"></script>");
client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");
client.println("<style>");
client.println("body {overflow  : hidden;background-color: #BBB;}");
client.println("#container {width: 100%;height: 100%;overflow: hidden;padding:0;margin:0;-webkit-user-select:none;-moz-user-select:none;}");
client.println("</style><title> ESP32 Bot Control Site </title></head><body>");
client.println("<div id=\"container\"><h1>ESP32 Bot Control</h1></div><div id=\"info\"><span id=\"result\"></span></div> <script>");
client.println("console.log(\"touchscreen is\", VirtualJoystick.touchScreenAvailable() ? \"available\" : \"not available\");");
client.println("var joystick  = new VirtualJoystick({");
client.println("container : document.getElementById('container'),");
client.println("mouseSupport  : true,");
client.println("limitStickTravel : true,");
client.println("stickRadius : 100");
client.println("});");
client.println("joystick.addEventListener('touchStart', function(){");
client.println("console.log('down')");
client.println("})");
client.println("joystick.addEventListener('touchEnd', function(){");
client.println("console.log('up')");
client.println("})");
client.println("$.ajaxSetup({timeout:35});"); //the most important line
client.println("setInterval(function(){");
client.println("var message = \"/?M/?X\"+Math.round(joystick.deltaX())+\"/X/?Y\"+Math.round(joystick.deltaY())+\"/Y/M\";$.get(message);var outputEl = document.getElementById('result');}, 75);");
client.println("</script></body></html>");
};
