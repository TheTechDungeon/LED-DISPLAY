//////////////////////////////////////////////////////////////////////////////////////////////
// LED 16x16 Display Module
// Copyright 2023 The Tech Dungeon
//
// This code is licensed under the GPL
//
// We believe in open development. The GPL has rights and obligations.
// You must abide by the terms of the GPL v3 license as noted in the LICENSE file
// If you modify this code you must make your source code available to end users upon request.
//
// Please read the LICENSE file as it defines your specific rights regarding the use of this
// source code, as well as condiditions under which the rights are given to you
//////////////////////////////////////////////////////////////////////////////////////////////

#include <FastLED.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <AnimatedGIF.h>
#include <LittleFS.h>

#include "sprites.h"

int gifPlaying;
int gifPlayingStartMillis;
int gifPlayingDelay;
uint8_t showed_image = 0;
WiFiManager wifiManager;
ESP8266WebServer webServer(80);
AnimatedGIF gif;

#define LED_PIN     2                           // Data pin for controlling LED's
#define LED_XWIDTH  16                          // X Width of Pixels, 16 in our case
#define LED_YWIDTH  16                          // Y Width of Pixels, 16 in our case
#define NUM_LEDS    LED_XWIDTH * LED_YWIDTH     // Total number of pixels
#define BRIGHTNESS  32                          // Brightness setting. We keep this sane to reduce current draw
#define PAUSETIME   5000                        // Pause time between sprites in milliseconds
#define TEMPERATURE_1 Tungsten100W              // Color Temperature to use with FastLED library

CRGB leds[NUM_LEDS];                            // Define the array for our LED's

sprites cur_sprite;                             // Current sprite being displayed
File uploadFile;
File playbackFile;
String rootStr = "/";
const char* serverIndex = "<!DOCTYPE html><html lang=\"en\"><head><title>TechDungeon LED Display</title><style type=\"text/css\" media=\"screen\">.cm{z-index:300;position:absolute;left:5px;border:1px solid #444;background-color:#F5F5F5;display:none;box-shadow:0 0 10px rgba(0,0,0,.4);font-size:12px;font-family:sans-serif;font-weight:700}.cm ul{list-style:none;top:0;left:0;margin:0;padding:0}.cm li{position:relative;min-width:60px;cursor:pointer}.cm span{color:#444;display:inline-block;padding:6px}.cm li:hover{background:#444}.cm li:hover span{color:#EEE}.tvu ul,.tvu li{padding:0;margin:0;list-style:none}.tvu input{position:absolute;opacity:0}.tvu{font:normal 12px Verdana,Arial,Sans-serif;-moz-user-select:none;-webkit-user-select:none;user-select:none;color:#444;line-height:16px}.tvu span{margin-bottom:5px;padding:0 0 0 18px;cursor:pointer;display:inline-block;height:16px;vertical-align:middle;background:url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAQAAAC1+jfqAAAABGdBTUEAAK/INwWK6QAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAADoSURBVBgZBcExblNBGAbA2ceegTRBuIKOgiihSZNTcC5LUHAihNJR0kGKCDcYJY6D3/77MdOinTvzAgCw8ysThIvn/VojIyMjIyPP+bS1sUQIV2s95pBDDvmbP/mdkft83tpYguZq5Jh/OeaYh+yzy8hTHvNlaxNNczm+la9OTlar1UdA/+C2A4trRCnD3jS8BB1obq2Gk6GU6QbQAS4BUaYSQAf4bhhKKTFdAzrAOwAxEUAH+KEM01SY3gM6wBsEAQB0gJ+maZoC3gI6iPYaAIBJsiRmHU0AALOeFC3aK2cWAACUXe7+AwO0lc9eTHYTAAAAAElFTkSuQmCC) no-repeat;background-position:0 0}.tvu span:hover{text-decoration:underline}@media screen and (-webkit-min-device-pixel-ratio:0){.tvu{-webkit-animation:webkit-adjacent-element-selector-bugfix infinite 1s}}@-webkit-keyframes webkit-adjacent-element-selector-bugfix{from{padding:0}to{padding:0}}#uploader{position:absolute;top:0;right:0;left:0;height:28px;line-height:24px;padding-left:10px;background-color:#444;color:#EEE}#tree{position:absolute;top:28px;bottom:0;left:0;width:160px;padding:8px}#editor,#preview{position:absolute;top:28px;right:0;bottom:0;left:160px;border-left:1px solid #EEE}#preview{background-color:#EEE;padding:5px}</style><script>\n"
"eval(function(p,a,c,k,e,r){e=function(c){return(c<a?'':e(parseInt(c/a)))+((c=c%a)>35?String.fromCharCode(c+29):c.toString(36))};if(!''.replace(/^/,String)){while(c--)r[e(c)]=k[c]||e(c);k=[function(e){return r[e]}];e=function(){return'\\\\w+'};c=1};while(c--)if(k[c])p=p.replace(new RegExp('\\\\b'+e(c)+'\\\\b','g'),k[c]);return p}('5 12(d,f){3 h;3 i=2.9(\"13\");i.I=\"14\";i.1z=15;i.B=\"16\";2.s(d).l(i);3 j=2.9(\"13\");j.u=\"1A-J\";j.I=\"1B\";j.B=\"J\";j.1C=\"/\";2.s(d).l(j);3 k=2.9(\"1D\");k.v=\\'1E\\';2.s(d).l(k);5 17(){6(h.K==4){6(h.w!=L)18(\"19[\"+h.w+\"]: \"+h.M);C{f.1a(j.o)}}}k.x=5(e){6(i.D.q===0){p}h=y N();h.O=17;3 a=y 1b();a.1c(\"16\",i.D[0],j.o);h.P(\"1F\",\"/1d\");h.Q(a)};i.1G=5(e){6(i.D.q===0)p;3 a=i.D[0].B;3 b=/(?:\\\\.([^.]+))?$/.R(a)[1];3 c=/(.*)\\\\.[^.]+$/.R(a)[1];6(S c!==T){a=c}6(S b!==T){6(b===\"1H\")b=\"1I\";C 6(b===\"1J\")b=\"1K\";a=a+\".\"+b}6(j.o===\"/\"||j.o.1e(\"/\")===0){j.o=\"/\"+a}C{j.o=j.o.1L(0,j.o.1e(\"/\")+1)+a}}}5 1f(k){3 m=2.s(\"1M\");3 n=2.9(\"1g\");n.1h=\"1N\";2.s(k).l(n);5 1i(a){2.s(\\'1j-1O\\').1k=a+\"?1j=E\"}5 U(a){m.z.V=\"W\";m.v=\\'<1P 1k=\"\\'+a+\\'?1Q=\\'+1R.1S()+\\'\" z=\"1T:1l%;1U:1l%;1V-1W:1X;1Y-1Z:20;21:22;V:W;\" />\\'}5 1m(a,b){3 c=2.9(\"1n\");a.l(c);3 d=2.9(\"F\");c.l(d);6(X(b)){d.v=\"<r>23</r>\";d.x=5(e){U(b);6(2.8.G(\\'A\\').q>0)2.8.t(a)}}3 f=2.9(\"F\");c.l(f);f.v=\"<r>24</r>\";f.x=5(e){1i(b);6(2.8.G(\\'A\\').q>0)2.8.t(a)};3 g=2.9(\"F\");c.l(g);g.v=\"<r>25</r>\";g.x=5(e){1o(b);6(2.8.G(\\'A\\').q>0)2.8.t(a)}}5 1p(e,a,b){3 c=2.9(\"1g\");3 d=2.8.Y?2.8.Y:2.1q.Y;3 f=2.8.Z?2.8.Z:2.1q.Z;3 g=1r.10+f;3 h=1r.11+d;c.1h=\\'A\\';c.z.V=\\'W\\';c.z.26=g+\\'1s\\';c.z.27=h+\\'1s\\';1m(c,a);2.8.l(c);3 i=c.28;3 j=c.29;c.2a=5(e){6(e.10<g||e.10>(g+i)||e.11<h||e.11>(h+j)){6(2.8.G(\\'A\\').q>0)2.8.t(c)}}}5 1t(a,b,c){3 d=2.9(\"F\");d.u=(((a==\"/\")?\"\":a)+\"/\"+b);3 f=2.9(\"r\");f.2b=b;d.l(f);d.x=5(e){6(X(d.u.2c())){U(d.u)}};d.2d=5(e){e.2e();e.2f();1p(e,d.u,E)};p d}5 1u(a,b,c){3 d=2.9(\"1n\");a.l(d);3 e=c.q;2g(3 i=0;i<e;i++){6(c[i].I===\"14\")d.l(1t(b,c[i].B,c[i].2h))}}5 X(a){3 b=/(?:\\\\.([^.]+))?$/.R(a)[1];6(S b!==T){2i(b){2j\"2k\":p E}}p 15}1v.1a=5(a){n.t(n.1w[0]);H(n,\"/\")};5 1x(a){p 5(){6(7.K==4){6(7.w!=L){18(\"19[\"+7.w+\"]: \"+7.M)}C{n.t(n.1w[0]);H(n,\"/\")}}}}5 1o(a){7=y N();7.O=1x(a);3 b=y 1b();b.1c(\"J\",a);7.P(\"2l\",\"/1d\");7.Q(b)}5 1y(a,b){p 5(){6(7.K==4){6(7.w==L)1u(a,b,2m.2n(7.M))}}}5 H(a,b){7=y N(a,b);7.O=1y(a,b);7.P(\"2o\",\"/2p?2q=\"+b,E);7.Q(2r)}H(n,\"/\");p 1v}5 2s(){3 c={};3 d=2t.2u.2v.2w(/[?&]+([^=&]+)=([^&]*)/2x,5(m,a,b){c[a]=b});3 f=1f(\"2y\");12(\"2z\",f)};',62,160,'||document|var||function|if|xmlHttp|body|createElement||||||||||||appendChild|||value|return|length|span|getElementById|removeChild|id|innerHTML|status|onclick|new|style|cm|name|else|files|true|li|getElementsByClassName|httpGet|type|path|readyState|200|responseText|XMLHttpRequest|onreadystatechange|open|send|exec|typeof|undefined|loadPreview|display|block|isImageFile|scrollTop|scrollLeft|clientX|clientY|createFileUploader|input|file|false|data|httpPostProcessRequest|alert|ERROR|refreshPath|FormData|append|edit|lastIndexOf|createTree|div|className|loadDownload|download|src|100|fillFileMenu|ul|httpDelete|showContextMenu|documentElement|event|px|createTreeLeaf|addList|this|childNodes|delCb|getCb|multiple|upload|text|defaultValue|button|Upload|POST|onchange|html|htm|jpeg|jpg|substring|preview|tvu|frame|img|_cb|Date|now|width|height|object|fit|contain|image|rendering|pixelated|margin|auto|Preview|Download|Delete|left|top|offsetWidth|offsetHeight|onmouseout|innerText|toLowerCase|oncontextmenu|preventDefault|stopPropagation|for|size|switch|case|gif|DELETE|JSON|parse|GET|list|dir|null|onBodyLoad|window|location|href|replace|gi|tree|uploader'.split('|'),0,{}))\n"
"</script><script src=\"https://cdnjs.cloudflare.com/ajax/libs/ace/1.1.9/ace.js\" type=\"text/javascript\" charset=\"utf-8\"></script></head><body onload=\"onBodyLoad();\"><div id=\"uploader\"></div><div id=\"tree\"></div><div id=\"preview\" style=\"display:block;\"></div><iframe id=download-frame style='display:none;'></iframe></body></html>\n"
"";

Dir rootDirectory;
unsigned long fileMillis;

//////////////////////////////////////////////////////////////////////////////////////////////
// colors is an array of color values for our pixels. We have 10 defined.  You can
// tweak the color values to get a "better blue" or whatever.  If you add more colors it is 
// recommended to start them after the white. So start at 0x0a and go from there.
//////////////////////////////////////////////////////////////////////////////////////////////

CRGB colors[]={
  CHSV(0x00,0x00,0x00),   // 0x00 BLACK
  CHSV(0x00,0xff,0xFF),   // 0x01 Red
  CHSV(0x60,0xff,0xFF),   // 0x02 Green
  CHSV(0xA0,0xFF,0xFF),   // 0x03 Blue
  CHSV(0x80,0xff,0xFF),   // 0x04 Cyan
  CHSV(0x40,0xff,0xFF),   // 0x05 Yellow
  CHSV(0xc0,0xff,0xFF),   // 0x06 Purple
  CHSV(0x20,0xff,0xFF),   // 0x07 Orange
  CHSV(0x00,0x00,0x46),   // 0x08 Gray
  CHSV(0x00,0x00,0xFF)    // 0x09 White
};

//////////////////////////////////////////////////////////////////////////////////////////////
// Initial Setup of FastLED library
// Counting of total sprites
// Clearing the display
//////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  WiFi.mode(WIFI_STA);

  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  FastLED.setBrightness( BRIGHTNESS );
  FastLED.setTemperature( TEMPERATURE_1 ); // first temperature

  //////////////////////////////////////////////////////////////////////////////////////////////
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //
  // DO NOT CHANGE THE VALUES IN THE LINE BELOW THIS COMMENT UNLESS YOU KNOW WHAT YOU ARE DOING. 
  // THE CURRENT VALUES SET THE FASTLED LIBRARY TO LIMIT TOTAL AMPS USED BY THE 256 LED'S TO 
  // 1000 MILLIAMPS AT 5V. CHANGING THESE VALUES COULD DAMAGE THE MICROCONTROLLER OR YOUR 
  // PC'S USB PORT, YOUR MOTHERBOARD, YOUR USB-C POWER DEVICE, ETC.
  //
  // WE CAN NOT BE RESPONSIBLE FOR DAMAGE YOU MIGHT DO TO THE DISPLAY OR YOUR PC IF YOU MODIFY 
  // THE CODE AND IN PARTICULAR THE FastLED.setMaxPowerInVoltsAndMilliamps(5,1000) line!!!!!! 
  //
  // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
  //////////////////////////////////////////////////////////////////////////////////////////////

  FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 

  // Clear display
  FastLED.clear(true);
  FastLED.show();

  show_system_sprite(0);
  FastLED.delay(500);

  if (!LittleFS.begin()) {
    Serial.println("No filesystem, formatting.");
    LittleFS.format();
    if (!LittleFS.begin()) {
      Serial.println("Unable to format.");
      return;
    }
  }

  wifiManager.setDebugOutput(true);
  wifiManager.setAPCallback(apCallback);
  wifiManager.setConnectTimeout(15);
  wifiManager.setConnectRetries(2);
  wifiManager.setConfigPortalTimeout(180);
  if (wifiManager.autoConnect("TechDungeon")) {
    Serial.println("Connected to wifi...");
    handleWifiConnection();
  } else {
    Serial.println("Unable to connect to wifi.");
  }

  fileMillis = 0;
  gifPlaying = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Main program loop. This goes through all sprites and displays one and then pauses the
// PAUSETIME before moving on to the next sprite.
//////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  Dir rootDirectory = LittleFS.openDir("/");
  for(;;) {
    int newchar = Serial.read();
    if (newchar >= 0) {
      Serial.write(newchar);
      if (newchar == 'r') {
        wifiManager.resetSettings();
        wifiManager.reboot();
      }
      if (newchar == 'b') {
        wifiManager.reboot();
      }
    }

    if (gifPlaying) {
      if (millis() - gifPlayingStartMillis > gifPlayingDelay) {
        gifPlayingStartMillis = millis();
        int frameStatus = gifPlayFrame(&gifPlayingDelay);
        if (1 != frameStatus) {
          if (millis() - fileMillis > 5000) {
            fileMillis = millis();
            selectNextGif();
          } else {
            if (-1 == frameStatus) {
              selectNextGif();
            } else if (0 == frameStatus) {
              if (gif.getLastError() == GIF_EMPTY_FRAME) {
                gif.reset();
              } else if (gif.getLastError() != GIF_SUCCESS) {
                selectNextGif();
              }
            }
          }
        }
      }
    } else {
      if (millis() - fileMillis > 5000) {
        fileMillis = millis();
        selectNextGif();
      }
    }
    MDNS.update();
    webServer.handleClient();
    FastLED.show();
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
// WifiManager: Asking to be added to Wifi
//////////////////////////////////////////////////////////////////////////////////////////////
void selectNextGif(void) {
  bool ready = true;
  if (!rootDirectory.next()) {
    Serial.println("Restarting directory.");
    if (!showed_image) {
      show_system_sprite(2);
    }
    rootDirectory = LittleFS.openDir("/");
    ready = rootDirectory.next();
    showed_image = 0;
  }
  if (ready) {
    if (gifPlaying) {
      gifPlayEnd();
    }
    if (rootDirectory.fileName().endsWith(".gif")) {
      gifPlayBegin(rootDirectory.fileName().c_str());
      gifPlayingStartMillis = millis();
      gifPlayingDelay = 0;
      showed_image = 1;
    } else {
      Serial.printf("Skipping: %s\r\n", rootDirectory.fileName().c_str());
    }
  } else {
    Serial.println("Unable to find any file.");
    show_system_sprite(2);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WifiManager: Asking to be added to Wifi
//////////////////////////////////////////////////////////////////////////////////////////////
void apCallback(WiFiManager *manager) {
  show_system_sprite(1);
  FastLED.delay(500);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// ESP is on the Wifi and ready to go
//////////////////////////////////////////////////////////////////////////////////////////////
void handleWifiConnection(void) {
  if (MDNS.begin("techdungeon")) {
    Serial.println("MDNS started...");
  }

  webServer.on("/", HTTP_GET, handleRootPage);
  webServer.on("/list", HTTP_GET, handleFileList);
  webServer.on("/edit", HTTP_DELETE, handleFileDelete);
  webServer.on("/edit", HTTP_POST, handlePostPage, handlePostUpload);
  webServer.onNotFound(handleNotFound);

  webServer.begin();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Deliver root page
//////////////////////////////////////////////////////////////////////////////////////////////
void handleRootPage(void) {
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/html", serverIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Deliver image, if one exists
//////////////////////////////////////////////////////////////////////////////////////////////
void handleNotFound(void) {
  if(!handleFileRead(webServer.uri())) {
    webServer.send(404, "text/plain", "FileNotFound");
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Unescape percent encoding
//////////////////////////////////////////////////////////////////////////////////////////////
unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

String urldecode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    for (int i =0; i < str.length(); i++){
        c=str.charAt(i);
      if (c == '+'){
        encodedString+=' ';  
      }else if (c == '%') {
        i++;
        code0=str.charAt(i);
        i++;
        code1=str.charAt(i);
        c = (h2int(code0) << 4) | h2int(code1);
        encodedString+=c;
      } else{
        
        encodedString+=c;  
      }
      
      yield();
    }
    
   return encodedString;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: When transferring  file, send as an image, or an octet stream when it's being downloaded
//////////////////////////////////////////////////////////////////////////////////////////////
String getContentType(String filename){
  if(webServer.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".gif")) return "image/gif";
  return "text/plain";
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Read files from LittleFS
//////////////////////////////////////////////////////////////////////////////////////////////
bool handleFileRead(String path){
  String decoded_url = urldecode(path);
  String contentType = getContentType(decoded_url);
  Serial.printf("read: %s\r\n", decoded_url.c_str());
  if(LittleFS.exists(decoded_url)){
    File file = LittleFS.open(decoded_url, "r");
    size_t sent = webServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: List files on LittleFS partition, send as JSON stream
//////////////////////////////////////////////////////////////////////////////////////////////
void handleFileList() {
  if(!webServer.hasArg("dir")) {webServer.send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = webServer.arg("dir");
  Dir dir = LittleFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir)?"dir":"file";
    output += "\",\"name\":\"";
    output += dir.fileName();
    output += "\"}";
  }
  
  output += "]";
  webServer.send(200, "text/json", output);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Delete a file from the LittleFS partition
//////////////////////////////////////////////////////////////////////////////////////////////
void handleFileDelete(){
  if(webServer.args() == 0) return webServer.send(500, "text/plain", "BAD ARGS");
  String path = webServer.arg(0);
  if(path == "/")
    return webServer.send(500, "text/plain", "BAD PATH");
  if(!LittleFS.exists(path))
    return webServer.send(404, "text/plain", "FileNotFound");
  LittleFS.remove(path);
  webServer.send(200, "text/plain", "");
  path = String();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Respond to the POST requests
//////////////////////////////////////////////////////////////////////////////////////////////
void handlePostPage(void) {
  webServer.sendHeader("Connection", "close");
  webServer.send(200, "text/plain", "OK");
}

//////////////////////////////////////////////////////////////////////////////////////////////
// WebServer: Injest the new .gif file and save to LittleFS
//////////////////////////////////////////////////////////////////////////////////////////////
void handlePostUpload(void) {
  HTTPUpload& upload = webServer.upload();
  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Getting image: %s -- Expect %d bytes.\r\n", upload.filename.c_str(), upload.contentLength);
    uploadFile = LittleFS.open((rootStr + upload.filename).c_str(), "w");
    if (!uploadFile) {
      Serial.println("Unable to open file for write.");
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (!uploadFile.write(upload.buf, upload.currentSize)) {
      Serial.println("Unable to write to file.");
    } else {
      Serial.printf("Accepted %d image bytes.\r\n", upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.println("Image accepted.");
    uploadFile.close();
  }
  yield();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GIF player: Hook the animation player to LittleFS
//////////////////////////////////////////////////////////////////////////////////////////////
static void * GIFOpenFile(const char *fname, int32_t *pSize)
{
  playbackFile = LittleFS.open(fname, "r");
  if (playbackFile) {
    *pSize = playbackFile.size();
    return (void *)&playbackFile;
  }
  return NULL;
}

static void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
}


static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
  int32_t iBytesRead;
  iBytesRead = iLen;
  File *f = static_cast<File *>(pFile->fHandle);
  // Note: If you read a file all the way to the last byte, seek() stops working
  if ((pFile->iSize - pFile->iPos) < iLen)
      iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
  if (iBytesRead <= 0)
      return 0;
  iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
  pFile->iPos = f->position();
  return iBytesRead;
}


static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
  //log_d("Seek time = %d us\n", i);
  return pFile->iPos;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GIF Player: RGB565 to 24 bit color for FastLED, and position correctly
//////////////////////////////////////////////////////////////////////////////////////////////
void LEDDrawPixel(int x, int y, uint16_t color) {
  uint16_t unswapped_color =  __builtin_bswap16(color);
  uint8_t red = (unswapped_color & 0xf800) >> 8;
  uint8_t green = (unswapped_color & 0x07e0) >> 3;
  uint8_t blue = (unswapped_color & 0x001f) << 3;
  leds[order[(y * 16) + x]] = CRGB(red, green, blue);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// GIF Player: Handle drawing call given that transparent pixels may exist
//////////////////////////////////////////////////////////////////////////////////////////////
void GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y, iWidth;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }
  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int x, iCount;
    pEnd = s + pDraw->iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < pDraw->iWidth)
    {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      }           // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        for (int xOffset = 0; xOffset < iCount; xOffset++)
        {
          LEDDrawPixel(x, y, usTemp[xOffset]);
        }
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          iCount++;
        else
          s--;
      }
      if (iCount)
      {
        x += iCount; // skip these
        iCount = 0;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    for (x = 0; x < pDraw->iWidth; x++)
    {
      LEDDrawPixel(x, y, usPalette[*s++]);
    }
  }
} /* GIFDraw() */

//////////////////////////////////////////////////////////////////////////////////////////////
// GIF Player: Call the GIF animation player, and play a file from LittleFS
//////////////////////////////////////////////////////////////////////////////////////////////
void gifPlayBegin( const char* gifPath )
{

  gif.begin(BIG_ENDIAN_PIXELS);

  if( ! gif.open( gifPath, GIFOpenFile, GIFCloseFile, GIFReadFile, GIFSeekFile, GIFDraw ) ) {
    Serial.printf("Could not open gif %s:%d\r\n", gifPath, gif.getLastError());
  }

  Serial.printf("Playing %s\r\n", gifPath);
  gifPlaying = 1;
}

int gifPlayFrame(int *inFrameDelay) {
  int frameStatus;
  long lTime = millis();
  int frameDelay = 0; // store delay for the last frame
  if (frameStatus = gif.playFrame(false, &frameDelay)) {
    lTime = millis() - lTime;
    if (lTime < frameDelay) {
      *inFrameDelay = frameDelay - lTime;
    } else {
      *inFrameDelay = 0;
    }
  }
  return frameStatus;
}

void gifPlayEnd(void) {
  gifPlaying = 0;
  gif.close();
  return;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// show_system_sprite loads a system sprite into the LEDs array
//////////////////////////////////////////////////////////////////////////////////////////////
void show_system_sprite(int spriteNum) {
  // Run through all LED's and set their color to the given data for that frame
  for(int x=0;x<NUM_LEDS;x++) {
    leds[order[x]] = colors[systemspritelist[spriteNum].sprite[x]];
  }
}
